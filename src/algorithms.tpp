#include <random>
#include <algorithm>
#include <queue>
#include <stack>
#include <cmath>

#include <ds/disjoint_set.hpp>

namespace dag {
  template <static_edge EdgeT, typename DiscoveryF>
  component<typename EdgeT::VertexType>
  breadth_first_search(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert,
      DiscoveryF discovered,
      std::size_t size_hint) {
    component<typename EdgeT::VertexType> discovered_comp(size_hint);
    discovered_comp.insert(vert);
    std::queue<typename EdgeT::VertexType> queue;
    queue.push(vert);
    while (!queue.empty()) {
      auto v = queue.front();
      queue.pop();
      for (auto& w: net.successors(v)) {
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
  std::vector<VertT> topological_order(
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
      throw utils::not_acyclic_error("argument dir most be acyclic");

    return topo;
  }


  template <network_vertex VertT>
  component<VertT> _out_component(
      const directed_network<VertT>& dir,
      const VertT& root,
      std::size_t size_hint,
      bool revert_graph) {
    component<VertT> out_component(size_hint);
    out_component.insert(root);
    auto topo = topological_order(dir);

    if (revert_graph)
      std::ranges::reverse(topo);

    for (auto&& vert: topo) {
      if (out_component.contains(vert)) {
        auto edges = (revert_graph ? dir.in_edges(vert) : dir.out_edges(vert));
        for (auto& edge: edges)
          for (auto& v: edge.incident_verts())
            out_component.insert(v);
      }
    }

    return out_component;
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
    std::vector<std::pair<VertT, Res>> res;
    res.reserve(dir.vertices().size());
    std::unordered_map<VertT, Comp, hash<VertT>> ongoing_components;
    std::unordered_map<VertT, std::size_t, hash<VertT>> in_degrees;

    auto topo = topological_order(dir);

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


  template <network_vertex VertT>
  component<VertT> out_component(
      const directed_network<VertT>& dir,
      const VertT& root,
      std::size_t size_hint) {
    return _out_component(dir, root, size_hint, false);
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
    return _out_component(dir, root, size_hint, true);
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
      const VertT& vert) {
    return breadth_first_search(net, vert,
        [](const VertT&, const VertT&){ return true; }, 0);
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
