#include <random>
#include <algorithm>
#include <queue>
#include <stack>
#include <cmath>

#include <ds/disjoint_set.hpp>

#include "../include/reticula/utils.hpp"
#include "../include/reticula/stats.hpp"

namespace reticula {
  namespace detail {
    template <typename Comp>
    struct component_type_constructor {
      Comp operator()(std::size_t size_est, std::size_t seed);
    };


    template <network_vertex VertT>
    struct component_type_constructor<component<VertT>> {
      component<VertT>
      operator()(std::size_t size_est, std::size_t /* seed */) {
        return component<VertT>(size_est);
      }
    };

    template <network_vertex VertT>
    struct component_type_constructor<component_sketch<VertT>> {
      component_sketch<VertT>
      operator()(std::size_t /* size_est */, std::size_t seed) {
        return component_sketch<VertT>(seed);
      }
    };

    template <
      directed_static_network_edge EdgeT,
      network_component Comp,
      typename Res>
    requires
      std::constructible_from<Res, Comp> &&
      std::same_as<typename Comp::VertexType, typename EdgeT::VertexType>
    std::vector<std::pair<typename EdgeT::VertexType, Res>>
    out_components_dag(
        const network<EdgeT>& dir,
        std::size_t seed,
        bool revert_graph,
        std::vector<typename EdgeT::VertexType> topo) {
      std::vector<std::pair<typename EdgeT::VertexType, Res>> res;
      res.reserve(dir.vertices().size());

      // DAG in-/out-component algorithm
      std::unordered_map<
        typename EdgeT::VertexType, Comp,
        hash<typename EdgeT::VertexType>> ongoing_components;
      std::unordered_map<
        typename EdgeT::VertexType, std::size_t,
        hash<typename EdgeT::VertexType>> in_counts;

      if (!revert_graph)
        ranges::reverse(topo);

      for (auto& vert: topo) {
        Comp comp = detail::component_type_constructor<Comp>{}(0, seed);
        comp.insert(vert);
        ongoing_components.emplace(vert, comp);

        auto in_edges =
          revert_graph ? dir.out_edges(vert) : dir.in_edges(vert);

        in_counts.emplace(vert, 0);
        for (auto& e: in_edges)
          in_counts.at(vert) +=
            (revert_graph ? e.mutated_verts() : e.mutator_verts()).size();

        auto out_edges =
          revert_graph ? dir.in_edges(vert) : dir.out_edges(vert);

        for (auto& oe: out_edges) {
          auto mutated_verts =
            revert_graph ? oe.mutator_verts() : oe.mutated_verts();
          for (auto& other: mutated_verts) {
            ongoing_components.at(vert).merge(ongoing_components.at(other));
            in_counts.at(other)--;

            if (in_counts.at(other) == 0) {
              res.emplace_back(other, ongoing_components.at(other));
              in_counts.erase(other);
              ongoing_components.erase(other);
            }
          }
        }

        if (in_counts.at(vert) == 0) {
          res.emplace_back(vert, ongoing_components.at(vert));
          in_counts.erase(vert);
          ongoing_components.erase(vert);
        }
      }

      return res;
    }

    template <
      directed_static_network_edge EdgeT,
      network_component Comp,
      typename Res>
    requires
      std::constructible_from<Res, Comp> &&
      std::same_as<typename Comp::VertexType, typename EdgeT::VertexType>
    std::vector<std::pair<typename EdgeT::VertexType, Res>>
    out_components(
        const network<EdgeT>& dir,
        std::size_t seed,
        bool revert_graph) {
      if (auto maybe_topo = try_topological_order(dir); maybe_topo)
        return out_components_dag<EdgeT, Comp, Res>(
            dir, seed, revert_graph, *maybe_topo);

      // Non-recursive version of SIMPLE_TC, based on Tarjan's algorithm
      std::unordered_map<
        typename EdgeT::VertexType, Comp,
        hash<typename EdgeT::VertexType>> out_comp;
      out_comp.reserve(dir.vertices().size());

      component<typename EdgeT::VertexType> seen(dir.vertices().size());
      std::stack<
        typename EdgeT::VertexType,
        std::vector<typename EdgeT::VertexType>> scc_stack;

      std::unordered_map<
        typename EdgeT::VertexType, std::size_t,
        hash<typename EdgeT::VertexType>> pre_order;
      pre_order.reserve(dir.vertices().size());
      auto not_preordered = [&pre_order](const typename EdgeT::VertexType& v) {
        return !pre_order.contains(v);
      };

      std::unordered_map<
        typename EdgeT::VertexType, std::size_t,
        hash<typename EdgeT::VertexType>> root_order;
      root_order.reserve(dir.vertices().size());

      std::size_t current_preorder = 0;
      for (auto& source: dir.vertices()) {
        if (!seen.contains(source)) {
          std::stack<
            typename EdgeT::VertexType,
            std::vector<typename EdgeT::VertexType>> stack;
          stack.push(source);
          while (!stack.empty()) {
            auto v = stack.top();
            if (not_preordered(v))
              pre_order.emplace(v, current_preorder++);

            auto succs =
              revert_graph ? dir.predecessors(v) : dir.successors(v);
            Comp comp = detail::component_type_constructor<Comp>{}(0, seed);
            comp.insert(succs);
            comp.insert(v);
            out_comp.emplace(v, comp);
            if (auto child = ranges::find_if(succs, not_preordered);
                child != succs.end()) {
              stack.push(*child);
            } else {
              root_order.emplace(v, pre_order.at(v));
              for (auto& s: succs) {
                out_comp.at(v).merge(out_comp.at(s));
                if (!seen.contains(s)) {
                  if (pre_order.at(s) > pre_order.at(v))
                    root_order.at(v) = std::min(
                        root_order.at(v), root_order.at(s));
                  else
                    root_order.at(v) = std::min(
                        root_order.at(v), pre_order.at(s));
                }
              }
              stack.pop();
              if (root_order.at(v) == pre_order.at(v)) {
                while (!scc_stack.empty() &&
                    pre_order.at(scc_stack.top()) >= pre_order.at(v)) {
                  auto s = scc_stack.top();
                  scc_stack.pop();
                  seen.insert(s);
                  out_comp.at(s) = out_comp.at(v);
                }
              } else {
                scc_stack.push(v);
              }
            }
          }
        }
      }

      return {out_comp.begin(), out_comp.end()};
    }


    template <network_edge EdgeT>
    std::vector<component<typename EdgeT::VertexType>>
    generic_weakly_connected_components(
        const network<EdgeT>& net,
        bool singletons) {
      auto verts = net.vertices();
      auto disj_set = ds::disjoint_set<std::size_t>(verts.size());

      std::unordered_map<
        typename EdgeT::VertexType,
        std::size_t,
        hash<typename EdgeT::VertexType>> vert_idx;
      for (std::size_t i = 0; auto& v: verts)
        vert_idx.emplace(v, i++);

      for (auto e: net.edges())
        for (auto v1: e.mutator_verts())
          for (auto v2: e.mutated_verts())
            if (!is_undirected_v<EdgeT> || v1 < v2)
              disj_set.merge(vert_idx.at(v1), vert_idx.at(v2));

      auto sets = disj_set.sets(singletons);
      std::vector<component<typename EdgeT::VertexType>> comp_vector;
      comp_vector.reserve(sets.size());

      for (const auto& [idx, set]: sets) {
        auto& current_set = comp_vector.emplace_back(set.size());
        for (const auto& event_idx: set)
          current_set.insert(verts.at(event_idx));
      }

      return comp_vector;
    }
  }  // namespace detail

  template <static_network_edge EdgeT, typename DiscoveryF>
  component<typename EdgeT::VertexType>
  breadth_first_search(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert,
      DiscoveryF discovered,
      bool revert_graph = false,
      bool ignore_direction = false,
      std::size_t size_hint = 0) {
    component<typename EdgeT::VertexType> discovered_comp(size_hint);
    discovered_comp.insert(vert);
    std::queue<typename EdgeT::VertexType> queue;
    queue.push(vert);
    while (!queue.empty()) {
      auto v = queue.front();
      queue.pop();
      std::vector<EdgeT> next_edges;
      if (ignore_direction)
        next_edges = net.incident_edges(v);
      else if (revert_graph)
        next_edges = net.in_edges(v);
      else
        next_edges = net.out_edges(v);
      for (auto& e: next_edges) {
        std::vector<typename EdgeT::VertexType> next;
        if (ignore_direction)
          next = e.incident_verts();
        else if (revert_graph)
          next = e.mutator_verts();
        else
          next = e.mutated_verts();

        for (auto& w: next) {
          if (!discovered_comp.contains(w)) {
            discovered_comp.insert(w);
            if (!discovered(v, e, w))
              return discovered_comp;
            queue.push(w);
          }
        }
      }
    }

    return discovered_comp;
  }

  template <directed_static_network_edge EdgeT>
  std::optional<std::vector<typename EdgeT::VertexType>>
  try_topological_order(const network<EdgeT>& dir) {
    auto verts = dir.vertices();

    std::vector<typename EdgeT::VertexType> topo;
    topo.reserve(verts.size());

    std::stack<
      typename EdgeT::VertexType,
      std::vector<typename EdgeT::VertexType>> stack;

    // in order for this to work with hypergraphs, we need to count sum of the
    // number of mutator_verts of all in_edges, not in-degree, i.e. the number
    // of in-edges or even the number predecessor:
    std::unordered_map<
      typename EdgeT::VertexType, std::size_t,
      hash<typename EdgeT::VertexType>> in_counts;
    in_counts.reserve(verts.size());

    for (auto& e: dir.edges())
      for (const auto &v: e.mutated_verts())
        in_counts[v] += e.mutator_verts().size();

    for (auto& v: verts)
      if (!in_counts.contains(v))
        stack.push(v);

    while (!stack.empty()) {
      auto v = stack.top();
      stack.pop();
      topo.push_back(v);
      for (const auto &e: dir.out_edges(v))
        for (const auto &ov: e.mutated_verts())
          if (--in_counts[ov] == 0)
            stack.push(ov);
    }

    if (topo.size() < in_counts.size())
      return std::nullopt;

    return topo;
  }

  template <directed_static_network_edge EdgeT>
  std::vector<typename EdgeT::VertexType>
  topological_order(const network<EdgeT>& dir) {
    auto o = try_topological_order(dir);
    if (o)
      return *o;
    else
      throw utils::not_acyclic_error("argument dir most be acyclic");
  }


  template <directed_static_network_edge EdgeT>
  bool is_acyclic(const network<EdgeT>& dir) {
    return try_topological_order(dir).has_value();
  }


  template <directed_static_network_edge EdgeT>
  component<typename EdgeT::VertexType> out_component(
      const network<EdgeT>& dir,
      const typename EdgeT::VertexType& root,
      std::size_t size_hint) {
    return breadth_first_search(dir, root,
        [](
          const typename EdgeT::VertexType&, const EdgeT&,
          const typename EdgeT::VertexType&){ return true; },
        false, false, size_hint);
  }

  template <directed_static_network_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component<typename EdgeT::VertexType>>>
  out_components(const network<EdgeT>& dir) {
    return detail::out_components<
      EdgeT,
      component<typename EdgeT::VertexType>,
      component<typename EdgeT::VertexType>>(dir, 0, false);
  }

  template <directed_static_network_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size<typename EdgeT::VertexType>>>
  out_component_sizes(const network<EdgeT>& dir) {
    return detail::out_components<
      EdgeT,
      component<typename EdgeT::VertexType>,
      component_size<typename EdgeT::VertexType>>(dir, 0, false);
  }

  template <directed_static_network_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size_estimate<typename EdgeT::VertexType>>>
  out_component_size_estimates(
      const network<EdgeT>& dir,
      std::size_t seed) {
    return detail::out_components<
      EdgeT,
      component_sketch<typename EdgeT::VertexType>,
      component_size_estimate<typename EdgeT::VertexType>>(dir, seed, false);
  }

  template <directed_static_network_edge EdgeT>
  component<typename EdgeT::VertexType> in_component(
      const network<EdgeT>& dir,
      const typename EdgeT::VertexType& root,
      std::size_t size_hint) {
    return breadth_first_search(dir, root,
        [](const typename EdgeT::VertexType&, const EdgeT&,
          const typename EdgeT::VertexType&){ return true; },
        true, false, size_hint);
  }


  template <directed_static_network_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component<typename EdgeT::VertexType>>>
  in_components(const network<EdgeT>& dir) {
    return detail::out_components<
      EdgeT,
      component<typename EdgeT::VertexType>,
      component<typename EdgeT::VertexType>>(dir, 0, true);
  }

  template <directed_static_network_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size<typename EdgeT::VertexType>>>
  in_component_sizes(const network<EdgeT>& dir) {
    return detail::out_components<
      EdgeT,
      component<typename EdgeT::VertexType>,
      component_size<typename EdgeT::VertexType>>(dir, 0, true);
  }

  template <directed_static_network_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size_estimate<typename EdgeT::VertexType>>>
  in_component_size_estimates(
      const network<EdgeT>& dir,
      std::size_t seed) {
    return detail::out_components<
      EdgeT,
      component_sketch<typename EdgeT::VertexType>,
      component_size_estimate<typename EdgeT::VertexType>>(dir, seed, true);
  }

  template <directed_static_network_edge EdgeT>
  std::vector<component<typename EdgeT::VertexType>>
  weakly_connected_components(
      const network<EdgeT>& dir, bool singletons) {
    return detail::generic_weakly_connected_components(dir, singletons);
  }

  template <directed_static_network_edge EdgeT>
  component<typename EdgeT::VertexType>
  largest_weakly_connected_component(const network<EdgeT>& dir) {
    auto comps = detail::generic_weakly_connected_components(dir, true);
    if (comps.empty())
      return component<typename EdgeT::VertexType>();
    else
      return *ranges::max_element(comps, ranges::less{},
          std::size<component<typename EdgeT::VertexType>>);
  }


  template <directed_static_network_edge EdgeT>
  bool is_weakly_connected(const network<EdgeT>& dir) {
    return breadth_first_search(dir, dir.vertices().front(),
        [](const typename EdgeT::VertexType&, const EdgeT&,
          const typename EdgeT::VertexType&){ return true; },
        false, true, 0).size() == dir.vertices().size();
  }

  template <directed_static_network_edge EdgeT>
  component<typename EdgeT::VertexType>
  weakly_connected_component(
      const network<EdgeT>& dir,
      const typename EdgeT::VertexType& vert,
      std::size_t size_hint) {
    return breadth_first_search(dir, vert,
        [](const typename EdgeT::VertexType&, const EdgeT&,
          const typename EdgeT::VertexType&){ return true; },
        false, true, size_hint);
  }

  template <undirected_static_network_edge EdgeT>
  std::vector<component<typename EdgeT::VertexType>>
  connected_components(
      const network<EdgeT>& net,
      bool singletons) {
    return detail::generic_weakly_connected_components(net, singletons);
  }

  template <undirected_static_network_edge EdgeT>
  component<typename EdgeT::VertexType>
  largest_connected_component(const network<EdgeT>& net) {
    auto comps = detail::generic_weakly_connected_components(net, true);
    if (comps.empty())
      return component<typename EdgeT::VertexType>();
    else
      return *ranges::max_element(comps, ranges::less{},
          std::size<component<typename EdgeT::VertexType>>);
  }

  template <undirected_static_network_edge EdgeT>
  bool is_connected(const network<EdgeT>& net) {
    return breadth_first_search(net, net.vertices().front(),
        [](const typename EdgeT::VertexType&, const EdgeT&,
          const typename EdgeT::VertexType&){ return true; },
        false, false, 0).size() == net.vertices().size();
  }

  template <undirected_static_network_edge EdgeT>
  component<typename EdgeT::VertexType>
  connected_component(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert,
      std::size_t size_hint) {
    return breadth_first_search(net, vert,
        [](const typename EdgeT::VertexType&, const EdgeT&,
          const typename EdgeT::VertexType&){ return true; },
        false, false, size_hint);
  }


  template <static_network_edge EdgeT>
  bool is_reachable(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& source,
      const typename EdgeT::VertexType& destination) {
    return breadth_first_search(net, source,
        [destination](
          const typename EdgeT::VertexType&, const EdgeT&,
          const typename EdgeT::VertexType& v){ return v != destination; },
        false, false, 0).contains(destination);
  }

  template <ranges::forward_range Range>
  requires degree_range<Range>
  bool is_graphic(Range&& sequence) {
    std::size_t num_degrees = 0;
    std::size_t max_deg = std::numeric_limits<std::size_t>::min();
    std::size_t min_deg = std::numeric_limits<std::size_t>::max();
    std::size_t total_degrees = 0;

    for (auto deg: sequence) {
      if (deg < 0)
        return false;
      total_degrees += static_cast<std::size_t>(deg);
      min_deg = std::min(min_deg, static_cast<std::size_t>(deg));
      max_deg = std::max(max_deg, static_cast<std::size_t>(deg));
      num_degrees++;
    }

    // an empty sequence is graphic
    if (num_degrees == 0)
      return true;

    if (max_deg >= num_degrees ||
        total_degrees % 2 != 0 ||
        total_degrees > num_degrees*(num_degrees - 1))
      return false;

    if (4 * num_degrees * min_deg <
        (max_deg + min_deg + 1)*(max_deg + min_deg + 1))
      return true;

    std::vector<size_t> degree_dist(max_deg + 1, 0);
    for (auto deg: sequence)
      degree_dist[static_cast<std::size_t>(deg)]++;

    std::size_t k = 0, partial_deg_sum = 0;
    std::size_t partial_n_j_sum = 0, partial_jn_j_sum = 0;
    for (std::size_t d_k = max_deg; d_k >= min_deg; d_k--) {
      if (d_k < k + 1)
        return true;

      if (degree_dist[d_k] > 0) {
        std::size_t run = degree_dist[d_k];
        if (d_k < k + run)
          run = d_k - k;

        partial_deg_sum += run*d_k;
        for (std::size_t i = 0; i < run; i++) {
          partial_n_j_sum += degree_dist[k + i];
          partial_jn_j_sum += (k + i)*degree_dist[k + i];
        }

        k += run;
        if (partial_deg_sum >
            k*(num_degrees - 1) -
            k*partial_n_j_sum + partial_jn_j_sum)
          return false;
      }
    }

    return true;
  }

  template <ranges::input_range PairRange>
  requires degree_pair_range<PairRange>
  bool is_digraphic(
      PairRange&& in_out_sequence) {
    using InType = std::tuple_element_t<0,
          ranges::range_value_t<PairRange>>;
    using OutType = std::tuple_element_t<1,
          ranges::range_value_t<PairRange>>;

    InType sum_in{}, max_in{};
    OutType sum_out{};

    std::vector<std::pair<OutType, InType>> zero_heap, nonzero_heap;
    for (auto& [in, out]: in_out_sequence) {
      if (in < 0 || out < 0)
        return false;

      max_in = std::max(in, max_in);
      sum_in += in;
      sum_out += out;

      if (in == 0)
        zero_heap.emplace_back(out, in);
      else
        nonzero_heap.emplace_back(out, in);
    }

    if (sum_in != sum_out)
      return false;

    ranges::make_heap(zero_heap);
    ranges::make_heap(nonzero_heap);

    std::vector<std::pair<OutType, InType>> modified_stubs;
    modified_stubs.reserve(static_cast<std::size_t>(max_in));

    while (!nonzero_heap.empty()) {
      ranges::pop_heap(nonzero_heap);
      auto [out, in] = nonzero_heap.back();
      nonzero_heap.pop_back();

      if (static_cast<std::size_t>(in) > nonzero_heap.size() + zero_heap.size())
        return false;

      for (InType i{}; i < in; i++) {
        OutType stub_out;
        InType stub_in;
        if (!zero_heap.empty() &&
            (nonzero_heap.empty() ||
             nonzero_heap.front() < zero_heap.front())) {
          ranges::pop_heap(zero_heap);
          std::tie(stub_out, stub_in) = zero_heap.back();
          zero_heap.pop_back();
        } else {
          ranges::pop_heap(nonzero_heap);
          std::tie(stub_out, stub_in) = nonzero_heap.back();
          nonzero_heap.pop_back();
        }

        if (stub_out == 0)
          return false;
        else if (stub_out > 1 || stub_in > 0)
          modified_stubs.emplace_back(stub_out - 1, stub_in);
      }

      while (!modified_stubs.empty()) {
        auto [mout, min] = modified_stubs.back();
        if (min > 0) {
          nonzero_heap.emplace_back(mout, min);
          ranges::push_heap(nonzero_heap);
        } else {
          zero_heap.emplace_back(mout, min);
          ranges::push_heap(zero_heap);
        }

        modified_stubs.pop_back();
      }

      if (out > 0) {
        zero_heap.emplace_back(out, 0);
        ranges::push_heap(zero_heap);
      }
    }

    return true;
  }

  template <network_vertex VertT>
  double density(const undirected_network<VertT>& net) {
    double n = static_cast<double>(net.vertices().size());
    return static_cast<double>(net.edges().size())/(n*(n-1)/2.0);
  }

  template <network_vertex VertT>
  double density(const directed_network<VertT>& net) {
    double n = static_cast<double>(net.vertices().size());
    return static_cast<double>(net.edges().size())/(n*(n-1));
  }

  template <static_network_edge EdgeT>
  std::unordered_map<
      typename EdgeT::VertexType, std::size_t,
      hash<typename EdgeT::VertexType>>
  shortest_path_lengths_from(
          const network<EdgeT>& net,
          const typename EdgeT::VertexType& vert) {
    std::unordered_map<
        typename EdgeT::VertexType, std::size_t,
        hash<typename EdgeT::VertexType>> lengths{{vert, 0}};
    breadth_first_search(net, vert,
        [&lengths](
          const typename EdgeT::VertexType& from, const EdgeT&,
          const typename EdgeT::VertexType& to) {
            lengths.try_emplace(to, lengths.at(from) + 1);
            return true;
        }, false, false, std::size_t{});
    return lengths;
  }

  template <static_network_edge EdgeT>
  std::unordered_map<
      typename EdgeT::VertexType, std::size_t,
      hash<typename EdgeT::VertexType>>
  shortest_path_lengths_to(
          const network<EdgeT>& net,
          const typename EdgeT::VertexType& vert) {
    std::unordered_map<
        typename EdgeT::VertexType, std::size_t,
        hash<typename EdgeT::VertexType>> lengths{{vert, 0}};
    breadth_first_search(net, vert,
        [&lengths](
          const typename EdgeT::VertexType& from, const EdgeT&,
          const typename EdgeT::VertexType& to) {
            lengths.try_emplace(to, lengths.at(from) + 1);
            return true;
        }, true, false, std::size_t{});
    return lengths;
  }

  template <network_edge EdgeT>
  std::size_t in_degree(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert) {
    return net.in_degree(vert);
  }

  template <network_edge EdgeT>
  std::size_t out_degree(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert) {
    return net.out_degree(vert);
  }

  template <network_edge EdgeT>
  std::size_t incident_degree(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert) {
    return net.degree(vert);
  }

  template <undirected_network_edge EdgeT>
  std::size_t degree(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert) {
    return net.degree(vert);
  }


  template <network_edge EdgeT>
  std::vector<std::size_t>
  in_degree_sequence(const network<EdgeT>& net) {
    std::vector<std::size_t> seq;
    seq.reserve(net.vertices().size());
    for (auto&& v: net.vertices())
      seq.push_back(in_degree(net, v));

    return seq;
  }

  template <network_edge EdgeT>
  std::vector<std::size_t>
  out_degree_sequence(const network<EdgeT>& net) {
    std::vector<std::size_t> seq;
    seq.reserve(net.vertices().size());
    for (auto&& v: net.vertices())
      seq.push_back(out_degree(net, v));

    return seq;
  }

  template <network_edge EdgeT>
  std::vector<std::size_t>
  incident_degree_sequence(const network<EdgeT>& net) {
    std::vector<std::size_t> seq;
    seq.reserve(net.vertices().size());
    for (auto&& v: net.vertices())
      seq.push_back(incident_degree(net, v));

    return seq;
  }

  template <network_edge EdgeT>
  std::vector<std::pair<std::size_t, std::size_t>>
  in_out_degree_pair_sequence(const network<EdgeT>& net) {
    std::vector<std::pair<std::size_t, std::size_t>> seq;
    seq.reserve(net.vertices().size());
    for (auto&& v: net.vertices())
      seq.emplace_back(in_degree(net, v), out_degree(net, v));

    return seq;
  }

  template <undirected_network_edge EdgeT>
  std::vector<std::size_t>
  degree_sequence(const network<EdgeT>& net) {
    std::vector<std::size_t> seq;
    seq.reserve(net.vertices().size());
    for (auto&& v: net.vertices())
      seq.push_back(degree(net, v));

    return seq;
  }


  template <
    undirected_static_network_edge EdgeT,
    std::invocable<const typename EdgeT::VertexType&> AttrFun>
  requires std::convertible_to<
      std::invoke_result_t<AttrFun, const typename EdgeT::VertexType&>, double>
  double attribute_assortativity(
      const network<EdgeT>& net, AttrFun&& attr_fun){
    std::vector<std::pair<double, double>> f;
    f.reserve(net.edges().size());
    for (auto& e: net.edges())
      for (auto& i: e.mutator_verts())
        for (auto& j: e.mutated_verts())
          if (i != j)
            f.emplace_back(attr_fun(i), attr_fun(j));

    return pearson_correlation_coefficient(f);
  }


  template <
    undirected_static_network_edge EdgeT,
    mapping<typename EdgeT::VertexType, double> MapT>
  double attribute_assortativity(
      const network<EdgeT>& net,
      const MapT& attribute_map,
      double default_value) {
    return attribute_assortativity(net,
        [&net, &attribute_map, default_value](
          const typename EdgeT::VertexType& v) -> double {
            auto it = attribute_map.find(v);
            if (it == attribute_map.end())
              return default_value;
            else
              return it->second;
        });
  }


  template <undirected_static_network_edge EdgeT>
  double degree_assortativity(const network<EdgeT>& net) {
    return attribute_assortativity(net,
        [&net](const typename EdgeT::VertexType& v) {
          return static_cast<double>(net.degree(v));
        });
  }

  template <
    directed_static_network_edge EdgeT,
    std::invocable<const typename EdgeT::VertexType&> AttrFun1,
    std::invocable<const typename EdgeT::VertexType&> AttrFun2>
  requires
    std::convertible_to<
      std::invoke_result_t<
        AttrFun1, const typename EdgeT::VertexType&>, double> &&
    std::convertible_to<
      std::invoke_result_t<
        AttrFun2, const typename EdgeT::VertexType&>, double>
  double attribute_assortativity(
      const network<EdgeT>& net,
      AttrFun1&& mutator_attr_fun,
      AttrFun2&& mutated_attr_fun) {
    std::vector<std::pair<double, double>> f;
    f.reserve(net.edges().size());
    for (auto& e: net.edges())
      for (auto& i: e.mutator_verts())
        for (auto& j: e.mutated_verts())
          f.emplace_back(mutator_attr_fun(i), mutated_attr_fun(j));

    return pearson_correlation_coefficient(f);
  }

  template <
    directed_static_network_edge EdgeT,
    mapping<typename EdgeT::VertexType, double> MapT1,
    mapping<typename EdgeT::VertexType, double> MapT2>
  double attribute_assortativity(
      const network<EdgeT>& net,
      const MapT1& mutator_attribute_map,
      const MapT2& mutated_attribute_map,
      double mutator_default_value,
      double mutated_default_value) {
    return attribute_assortativity(net,
        [&net, &mutator_attribute_map, mutator_default_value](
          const typename EdgeT::VertexType& v) -> double {
            auto it = mutator_attribute_map.find(v);
            if (it == mutator_attribute_map.end())
              return mutator_default_value;
            else
              return it->second;
        }, [&net, &mutated_attribute_map, mutated_default_value](
          const typename EdgeT::VertexType& v) -> double {
            auto it = mutated_attribute_map.find(v);
            if (it == mutated_attribute_map.end())
              return mutated_default_value;
            else
              return it->second;
        });
  }

  template <directed_static_network_edge EdgeT>
  double in_in_degree_assortativity(const network<EdgeT>& net) {
    return attribute_assortativity(net,
        [&net](const typename EdgeT::VertexType& v) {
          return net.in_degree(v);
        }, [&net](const typename EdgeT::VertexType& v) {
          return net.in_degree(v);
        });
  }

  template <directed_static_network_edge EdgeT>
  double in_out_degree_assortativity(const network<EdgeT>& net) {
    return attribute_assortativity(net,
        [&net](const typename EdgeT::VertexType& v) {
          return net.in_degree(v);
        }, [&net](const typename EdgeT::VertexType& v) {
          return net.out_degree(v);
        });
  }

  template <directed_static_network_edge EdgeT>
  double out_in_degree_assortativity(const network<EdgeT>& net) {
    return attribute_assortativity(net,
        [&net](const typename EdgeT::VertexType& v) {
          return net.out_degree(v);
        }, [&net](const typename EdgeT::VertexType& v) {
          return net.in_degree(v);
        });
  }

  template <directed_static_network_edge EdgeT>
  double out_out_degree_assortativity(const network<EdgeT>& net) {
    return attribute_assortativity(net,
        [&net](const typename EdgeT::VertexType& v) {
          return net.out_degree(v);
        }, [&net](const typename EdgeT::VertexType& v) {
          return net.out_degree(v);
        });
  }
}  // namespace reticula
