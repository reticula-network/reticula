#include <random>
#include <algorithm>
#include <queue>
#include <stack>
#include <cmath>

#include <ds/disjoint_set.hpp>

#include "../include/dag/utils.hpp"

namespace dag {
  namespace detail {
    template <
      static_directed_edge EdgeT,
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
        std::ranges::reverse(topo);

      for (auto& vert: topo) {
        Comp comp(0, seed);
        comp.insert(vert);
        ongoing_components[vert] = comp;

        auto in_edges =
          revert_graph ? dir.out_edges(vert) : dir.in_edges(vert);

        in_counts[vert] = 0;
        for (auto& e: in_edges)
          in_counts[vert] +=
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
      static_directed_edge EdgeT,
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

      dag::component<typename EdgeT::VertexType> seen(dir.vertices().size());
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
              pre_order[v] = current_preorder++;

            auto succs =
              revert_graph ? dir.predecessors(v) : dir.successors(v);
            out_comp.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(v),
                std::forward_as_tuple(succs, 0, seed));
            out_comp.at(v).insert(v);
            if (auto child = std::ranges::find_if(succs, not_preordered);
                child != succs.end()) {
              stack.push(*child);
            } else {
              root_order[v] = pre_order.at(v);
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
                    pre_order[scc_stack.top()] >= pre_order[v]) {
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

  template <static_edge EdgeT, typename DiscoveryF>
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
      std::vector<typename EdgeT::VertexType> next;
      if (ignore_direction)
        next = net.neighbours(v);
      else if (revert_graph)
        next = net.predecessors(v);
      else
        next = net.successors(v);
      for (auto& w: next) {
        if (!discovered_comp.contains(w)) {
          discovered_comp.insert(w);
          if (!discovered(w, v))
            return discovered_comp;
          queue.push(w);
        }
      }
    }

    return discovered_comp;
  }

  template <network_edge EdgeT, std::ranges::input_range Range>
  requires std::same_as<
            std::ranges::range_value_t<Range>,
            typename EdgeT::VertexType>
  network<EdgeT>
  vertex_induced_subgraph(
      const network<EdgeT>& net,
      const Range& verts) {
    component<typename EdgeT::VertexType> verts_comp(verts);

    std::vector<EdgeT> es;
    std::ranges::copy_if(net.edges(), std::back_inserter(es),
        [&verts_comp](const EdgeT& e) {
          for (auto& v: e.incident_verts())
            if (!verts_comp.contains(v))
              return false;
          return true;
        });

    std::vector<typename EdgeT::VertexType> vs;
    std::ranges::copy_if(net.vertices(), std::back_inserter(vs),
        [&verts_comp](const typename EdgeT::VertexType& v) {
          return verts_comp.contains(v);
        });

    return network<EdgeT>(es, vs);
  }

  template <network_edge EdgeT, std::ranges::input_range Range>
  requires std::same_as<std::ranges::range_value_t<Range>, EdgeT>
  network<EdgeT>
  edge_induced_subgraph(
      const network<EdgeT>& net,
      const Range& edges) {
    component<EdgeT> edge_comp(edges);

    std::vector<EdgeT> es;
    std::ranges::copy_if(net.edges(), std::back_inserter(es),
        [&edge_comp](const EdgeT& e) {
          return edge_comp.contains(e);
        });

    return network<EdgeT>(es);
  }

  template <static_directed_edge EdgeT>
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

  template <static_directed_edge EdgeT>
  std::vector<typename EdgeT::VertexType>
  topological_order(const network<EdgeT>& dir) {
    auto o = try_topological_order(dir);
    if (o)
      return *o;
    else
      throw utils::not_acyclic_error("argument dir most be acyclic");
  }


  template <static_directed_edge EdgeT>
  bool is_acyclic(const network<EdgeT>& dir) {
    return try_topological_order(dir).has_value();
  }


  template <static_directed_edge EdgeT>
  component<typename EdgeT::VertexType> out_component(
      const network<EdgeT>& dir,
      const typename EdgeT::VertexType& root,
      std::size_t size_hint) {
    return breadth_first_search(dir, root,
        [](const typename EdgeT::VertexType&,
          const typename EdgeT::VertexType&){ return true; },
        false, false, size_hint);
  }

  template <static_directed_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component<typename EdgeT::VertexType>>>
  out_components(const network<EdgeT>& dir) {
    return detail::out_components<
      EdgeT,
      component<typename EdgeT::VertexType>,
      component<typename EdgeT::VertexType>>(dir, 0, false);
  }

  template <static_directed_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size<typename EdgeT::VertexType>>>
  out_component_sizes(const network<EdgeT>& dir) {
    return detail::out_components<
      EdgeT,
      component<typename EdgeT::VertexType>,
      component_size<typename EdgeT::VertexType>>(dir, 0, false);
  }

  template <static_directed_edge EdgeT>
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

  template <static_directed_edge EdgeT>
  component<typename EdgeT::VertexType> in_component(
      const network<EdgeT>& dir,
      const typename EdgeT::VertexType& root,
      std::size_t size_hint) {
    return breadth_first_search(dir, root,
        [](const typename EdgeT::VertexType&,
          const typename EdgeT::VertexType&){ return true; },
        true, false, size_hint);
  }


  template <static_directed_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component<typename EdgeT::VertexType>>>
  in_components(const network<EdgeT>& dir) {
    return detail::out_components<
      EdgeT,
      component<typename EdgeT::VertexType>,
      component<typename EdgeT::VertexType>>(dir, 0, true);
  }

  template <static_directed_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size<typename EdgeT::VertexType>>>
  in_component_sizes(const network<EdgeT>& dir) {
    return detail::out_components<
      EdgeT,
      component<typename EdgeT::VertexType>,
      component_size<typename EdgeT::VertexType>>(dir, 0, true);
  }

  template <static_directed_edge EdgeT>
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

  template <static_directed_edge EdgeT>
  std::vector<component<typename EdgeT::VertexType>>
  weakly_connected_components(
      const network<EdgeT>& dir, bool singletons) {
    return detail::generic_weakly_connected_components(dir, singletons);
  }

  template <static_directed_edge EdgeT>
  bool is_weakly_connected(const network<EdgeT>& dir) {
    return breadth_first_search(dir, dir.vertices().front(),
        [](const typename EdgeT::VertexType&,
          const typename EdgeT::VertexType&){ return true; },
        false, true, 0).size() == dir.vertices().size();
  }

  template <static_directed_edge EdgeT>
  component<typename EdgeT::VertexType>
  weakly_connected_component(
      const network<EdgeT>& dir,
      const typename EdgeT::VertexType& vert,
      std::size_t size_hint) {
    return breadth_first_search(dir, vert,
        [](const typename EdgeT::VertexType&,
          const typename EdgeT::VertexType&){ return true; },
        false, true, size_hint);
  }

  template <static_undirected_edge EdgeT>
  std::vector<component<typename EdgeT::VertexType>>
  connected_components(
      const network<EdgeT>& net,
      bool singletons) {
    return detail::generic_weakly_connected_components(net, singletons);
  }

  template <static_undirected_edge EdgeT>
  bool is_connected(const network<EdgeT>& net) {
    return breadth_first_search(net, net.vertices().front(),
        [](const typename EdgeT::VertexType&,
          const typename EdgeT::VertexType&){ return true; },
        false, false, 0).size() == net.vertices().size();
  }

  template <static_undirected_edge EdgeT>
  component<typename EdgeT::VertexType>
  connected_component(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert,
      std::size_t size_hint) {
    return breadth_first_search(net, vert,
        [](const typename EdgeT::VertexType&,
          const typename EdgeT::VertexType&){ return true; },
        false, false, size_hint);
  }

  template <network_vertex VertT1, network_vertex VertT2>
  undirected_network<std::pair<VertT1, VertT2>>
  cartesian_product(
      const undirected_network<VertT1>& g1,
      const undirected_network<VertT2>& g2) {
    std::vector<undirected_edge<std::pair<VertT1, VertT2>>> edges;
    edges.reserve(
        g1.vertices().size()*g2.edges().size() +
        g1.edges().size()*g2.vertices().size());

    for (auto&& e: g1.edges()) {
      auto verts = e.mutator_verts();
      if (verts.size() > 1)
        for (auto&& n: g2.vertices())
          edges.emplace_back(std::make_pair(verts[0], n),
              std::make_pair(verts[1], n));
    }

    for (auto&& n: g1.vertices()) {
      for (auto&& e: g2.edges()) {
        auto verts = e.mutator_verts();
        if (verts.size() > 1)
          edges.emplace_back(
              std::make_pair(n, verts[0]),
              std::make_pair(n, verts[1]));
      }
    }

    return undirected_network<std::pair<VertT1, VertT2>>(edges);
  }

  template <integer_vertex OutVertT, network_vertex InVertT>
  undirected_network<OutVertT>
  relabel_nodes(const undirected_network<InVertT>& g) {
    std::unordered_map<InVertT, OutVertT, hash<InVertT>> new_labels;
    new_labels.reserve(g.vertices().size());

    OutVertT i = {};
    for (auto&& n: g.vertices())
      new_labels.emplace(n, i++);

    std::vector<undirected_edge<OutVertT>> edges;
    for (auto&& e: g.edges()) {
      auto verts = e.mutator_verts();
      if (verts.size() > 1)
        edges.emplace_back(new_labels.at(verts[0]), new_labels.at(verts[1]));
    }

    return undirected_network<OutVertT>(edges);
  }

  template <std::ranges::forward_range Range>
  requires degree_range<Range>
  bool is_graphic(const Range& sequence) {
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

  template <std::ranges::input_range PairRange>
  requires degree_pair_range<PairRange>
  bool is_digraphic(
      const PairRange& in_out_sequence) {
    using InType = std::tuple_element_t<0,
          std::ranges::range_value_t<PairRange>>;
    using OutType = std::tuple_element_t<1,
          std::ranges::range_value_t<PairRange>>;

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

    std::ranges::make_heap(zero_heap);
    std::ranges::make_heap(nonzero_heap);

    std::vector<std::pair<OutType, InType>> modified_stubs;
    modified_stubs.reserve(static_cast<std::size_t>(max_in));

    while (!nonzero_heap.empty()) {
      std::ranges::pop_heap(nonzero_heap);
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
          std::ranges::pop_heap(zero_heap);
          std::tie(stub_out, stub_in) = zero_heap.back();
          zero_heap.pop_back();
        } else {
          std::ranges::pop_heap(nonzero_heap);
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
          std::ranges::push_heap(nonzero_heap);
        } else {
          zero_heap.emplace_back(mout, min);
          std::ranges::push_heap(zero_heap);
        }

        modified_stubs.pop_back();
      }

      if (out > 0) {
        zero_heap.emplace_back(out, 0);
        std::ranges::push_heap(zero_heap);
      }
    }

    return true;
  }
}  // namespace dag
