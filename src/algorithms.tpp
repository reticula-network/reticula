#include <random>
#include <algorithm>
#include <queue>
#include <cmath>

#include <iostream>

#include <ds/disjoint_set.hpp>

namespace dag {
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
  std::vector<VertT> _out_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      std::size_t size_hint,
      bool revert_graph) {
    std::unordered_set<VertT, hash<VertT>> out_component;
    if (size_hint > 0) out_component.reserve(size_hint);
    out_component.insert(vert);
    auto topo = topological_order(dir);
    if (revert_graph)
      std::reverse(topo.begin(), topo.end());

    for (auto&& vert: topo) {
      if (out_component.find(vert) != out_component.end()) {
        auto edges = (revert_graph ? dir.in_edges(vert) : dir.out_edges(vert));
        for (const auto& edge: edges) {
          auto verts = (revert_graph ?
                          edge.mutator_verts() : edge.mutated_verts());
          for (auto&& v: verts)
            out_component.insert(v);
        }
      }
    }
    return std::vector<VertT>(out_component.begin(), out_component.end());
  }

  template <network_vertex VertT>
  std::vector<VertT> in_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      std::size_t size_hint) {
    return _out_component(dir, vert, size_hint, true);
  }

  template <network_vertex VertT>
  std::vector<VertT> out_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      std::size_t size_hint) {
    return _out_component(dir, vert, size_hint, false);
  }

  template <network_vertex VertT>
  std::vector<std::vector<VertT>> weakly_connected_components(
      const directed_network<VertT>& dir,
      bool singleton) {
    std::vector<VertT> verts = dir.vertices();
    auto disj_set = ds::disjoint_set<std::size_t>(verts.size());

    auto vert_iter = verts.begin();
    while (vert_iter < verts.end()) {
      std::size_t vert_idx = static_cast<std::size_t>(
          std::distance(verts.begin(), vert_iter));

      for (auto&& other: dir.successors(*vert_iter)) {
        auto other_it = std::lower_bound(vert_iter+1, verts.end(), other);
        std::size_t other_idx = static_cast<std::size_t>(
            std::distance(verts.begin(), other_it));
        disj_set.merge(vert_idx, other_idx);
      }

      vert_iter++;
    }

    auto sets = disj_set.sets(singleton);
    std::vector<std::vector<VertT>> sets_vector;
    sets_vector.reserve(sets.size());

    for (const auto& [idx, set]: sets) {
      sets_vector.emplace_back();
      auto& current_set = sets_vector.back();
      current_set.reserve(set.size());
      for (const auto& event_idx: set)
        current_set.push_back(verts.at(event_idx));
    }

    return sets_vector;
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


  template <network_vertex OutVertT, network_vertex InVertT>
  requires std::numeric_limits<OutVertT>::is_integer
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
  requires std::numeric_limits<std::ranges::range_value_t<Range>>::is_integer
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
}  // namespace dag
