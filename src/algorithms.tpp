#include <random>
#include <algorithm>
#include <queue>
#include <cmath>

#include "../../../include/dag/adjacency_prob.hpp"

namespace dag {
  template <class EdgeT, class AdjacencyProbT>
  directed_network<EdgeT>
  event_graph(
      const network<EdgeT>& temp,
      const AdjacencyProbT& prob,
      size_t seed) {

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


  template <typename VertT>
  std::vector<VertT> topological_order(
      const directed_network<VertT>& dir) {
    auto verts = dir.vertices();

    std::unordered_map<VertT, size_t> in_degrees;
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

  template <typename VertT>
  std::vector<VertT> out_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint,
      bool revert_graph) {
    std::unordered_set<VertT> out_component;
    if (size_hint > 0) out_component.reserve(size_hint);
    out_component.insert(vert);
    auto topo = topological_order(dir);
    if (revert_graph)
      std::reverse(topo.begin(), topo.end());

    for (const VertT& vert: topo) {
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

  template <typename VertT>
  std::vector<VertT> in_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint) {
    return out_component(dir, vert, size_hint, true);
  }
}  // namespace dag
