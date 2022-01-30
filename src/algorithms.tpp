#include <random>
#include <algorithm>
#include <queue>
#include <stack>
#include <cmath>

#include <iostream>

#include <ds/disjoint_set.hpp>

namespace dag {
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

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  directed_network<EdgeT>
  event_graph(
      const network<EdgeT>& temp,
      const AdjacencyProbT& prob,
      std::size_t seed) {
    auto verts = temp.vertices();
    std::vector<directed_edge<EdgeT>> eg;
    for (const auto &v: verts) {
      auto events = temp.incident_edges(v);

      for (auto e1 = events.begin(); e1 < events.end(); e1++) {
        constexpr double cutoff = 1e-20;
        double last_prob = 1.0;
        for (auto e2 = e1; e2 < events.end() && last_prob > cutoff; e2++) {
          if (adjacent(*e1, *e2)) {
            last_prob = prob.p(*e1, *e2);
            if (adjacency_prob::bernoulli_trial(
                  *e1, *e2, last_prob, seed))
              eg.emplace_back(*e1, *e2);
          }
        }
      }
    }

    return directed_network<EdgeT>(eg);
  }

  template <network_vertex VertT>
  std::optional<std::vector<VertT>>
  try_topological_order(
      const directed_network<VertT>& dir) {
    auto verts = dir.vertices();

    std::unordered_map<VertT, std::size_t, hash<VertT>> in_degrees;
    in_degrees.reserve(verts.size());
    for (auto&& v: verts)
      in_degrees[v] = dir.in_degree(v);

    std::vector<VertT> topo;
    topo.reserve(verts.size());

    std::queue<VertT> queue;

    for (auto&& [v, deg]: in_degrees)
      if (deg == 0) queue.push(v);

    while (!queue.empty()) {
      VertT v = queue.front();
      queue.pop();
      topo.push_back(v);
      for (const auto &ov: dir.successors(v))
        if (--in_degrees[ov] == 0) queue.push(ov);
    }

    if (topo.size() < in_degrees.size())
      return std::nullopt;

    return topo;
  }

  template <network_vertex VertT>
  std::vector<VertT>
  topological_order(
      const directed_network<VertT>& dir) {
    auto o = try_topological_order(dir);
    if (o)
      return *o;
    else
      throw utils::not_acyclic_error("argument dir most be acyclic");
  }


  template <network_vertex VertT>
  bool is_acyclic(
      const directed_network<VertT>& dir) {
    return try_topological_order(dir).has_value();
  }

  template <
    network_vertex VertT,
    network_component Comp,
    typename Res>
  requires
    std::constructible_from<Res, Comp> &&
    std::same_as<typename Comp::VertexType, VertT>
  std::vector<std::pair<VertT, Res>>
  _out_components_dag(
      const directed_network<VertT>& dir,
      std::size_t seed,
      bool revert_graph,
      std::vector<VertT> topo) {
    std::vector<std::pair<VertT, Res>> res;
    res.reserve(dir.vertices().size());

    // DAG in-/out-component algorithm
    std::unordered_map<VertT, Comp, hash<VertT>> ongoing_components;
    std::unordered_map<VertT, std::size_t, hash<VertT>> in_degrees;

    if (!revert_graph)
      std::ranges::reverse(topo);

    for (auto& vert: topo) {
      Comp comp(0, seed);
      comp.insert(vert);
      ongoing_components[vert] = comp;

      std::size_t in_degree =
        revert_graph ? dir.out_degree(vert) : dir.in_degree(vert);
      std::vector<VertT> successors =
        revert_graph ? dir.predecessors(vert) : dir.successors(vert);

      in_degrees[vert] = in_degree;

      for (auto& other: successors) {
        ongoing_components.at(vert).merge(ongoing_components.at(other));
        in_degrees.at(other)--;
        if (in_degrees.at(other) == 0) {
          res.emplace_back(other, ongoing_components.at(other));
          in_degrees.erase(other);
          ongoing_components.erase(other);
        }
      }

      if (in_degrees.at(vert) == 0) {
        res.emplace_back(vert, ongoing_components.at(vert));
        in_degrees.erase(vert);
        ongoing_components.erase(vert);
      }
    }

    return res;
  }

  template <
    network_vertex VertT,
    network_component Comp,
    typename Res>
  requires
    std::constructible_from<Res, Comp> &&
    std::same_as<typename Comp::VertexType, VertT>
  std::vector<std::pair<VertT, Res>>
  _out_components(
      const directed_network<VertT>& dir,
      std::size_t seed,
      bool revert_graph) {
    if (auto maybe_topo = try_topological_order(dir); maybe_topo)
      return _out_components_dag<VertT, Comp, Res>(
          dir, seed, revert_graph, *maybe_topo);

    // Non-recursive version of SIMPLE_TC, based on Tarjan's algorithm
    std::unordered_map<VertT, Comp, hash<VertT>> out_comp;
    out_comp.reserve(dir.vertices().size());

    dag::component<VertT> seen(dir.vertices().size());
    std::stack<VertT, std::vector<VertT>> scc_stack;

    std::unordered_map<VertT, std::size_t, hash<VertT>> pre_order;
    pre_order.reserve(dir.vertices().size());
    auto not_preordered = [&pre_order](const VertT& v) {
      return !pre_order.contains(v);
    };

    std::unordered_map<VertT, std::size_t, hash<VertT>> root_order;
    root_order.reserve(dir.vertices().size());

    std::size_t current_preorder = 0;
    for (auto& source: dir.vertices()) {
      if (!seen.contains(source)) {
        std::stack<VertT, std::vector<VertT>> stack;
        stack.push(source);
        while (!stack.empty()) {
          VertT v = stack.top();
          if (not_preordered(v))
            pre_order[v] = current_preorder++;

          std::vector<VertT> succs =
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


  template <network_vertex VertT>
  component<VertT> out_component(
      const directed_network<VertT>& dir,
      const VertT& root,
      std::size_t size_hint) {
    return breadth_first_search(dir, root,
        [](const VertT&, const VertT&){ return true; },
        false, false, size_hint);
  }

  template <network_vertex VertT>
  std::vector<std::pair<VertT, component<VertT>>>
  out_components(const directed_network<VertT>& dir) {
    return _out_components<
      VertT,
      component<VertT>,
      component<VertT>>(dir, 0, false);
  }

  template <network_vertex VertT>
  std::vector<std::pair<VertT, component_size<VertT>>>
  out_component_sizes(const directed_network<VertT>& dir) {
    return _out_components<
      VertT,
      component<VertT>,
      component_size<VertT>>(dir, 0, false);
  }

  template <network_vertex VertT>
  std::vector<std::pair<VertT, component_size_estimate<VertT>>>
  out_component_size_estimates(const directed_network<VertT>& dir) {
    return _out_components<
      VertT,
      component_sketch<VertT>,
      component_size_estimate<VertT>>(dir, 0, false);
  }

  template <network_vertex VertT>
  component<VertT> in_component(
      const directed_network<VertT>& dir,
      const VertT& root,
      std::size_t size_hint) {
    return breadth_first_search(dir, root,
        [](const VertT&, const VertT&){ return true; },
        true, false, size_hint);
  }


  template <network_vertex VertT>
  std::vector<
    std::pair<VertT, component<VertT>>>
  in_components(const directed_network<VertT>& dir) {
    return _out_components<
      VertT,
      component<VertT>,
      component<VertT>>(dir, 0, true);
  }

  template <network_vertex VertT>
  std::vector<
    std::pair<VertT, component_size<VertT>>>
  in_component_sizes(const directed_network<VertT>& dir) {
    return _out_components<
      VertT,
      component<VertT>,
      component_size<VertT>>(dir, 0, true);
  }

  template <network_vertex VertT>
  std::vector<std::pair<VertT, component_size_estimate<VertT>>>
  in_component_size_estimates(const directed_network<VertT>& dir) {
    return _out_components<
      VertT,
      component_sketch<VertT>,
      component_size_estimate<VertT>>(dir, 0, true);
  }

  template <network_edge EdgeT>
  std::vector<component<typename EdgeT::VertexType>>
  _generic_weakly_connected_components(
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
            disj_set.merge(vert_idx[v1], vert_idx[v2]);

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

  template <network_vertex VertT>
  std::vector<component<VertT>>
  weakly_connected_components(
      const directed_network<VertT>& dir,
      bool singletons) {
    return _generic_weakly_connected_components(dir, singletons);
  }

  template <network_vertex VertT>
  component<VertT>
  weakly_connected_component(
      const directed_network<VertT>& dir,
      const VertT& vert,
      std::size_t size_hint) {
    return breadth_first_search(dir, vert,
        [](const VertT&, const VertT&){ return true; },
        false, true, size_hint);
  }

  template <network_vertex VertT>
  std::vector<component<VertT>>
  connected_components(
      const undirected_network<VertT>& net,
      bool singletons) {
    return _generic_weakly_connected_components(net, singletons);
  }

  template <network_vertex VertT>
  component<VertT>
  connected_component(
      const undirected_network<VertT>& net,
      const VertT& vert,
      std::size_t size_hint) {
    return breadth_first_search(net, vert,
        [](const VertT&, const VertT&){ return true; },
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
