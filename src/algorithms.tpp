#include <random>
#include <queue>
#include <cmath>


namespace dag {
  template <typename EdgeT>
  directed_network<EdgeT>
  event_graph(const network<EdgeT>& temp, typename EdgeT::TimeType max_delta_t) {
    std::unordered_set<typename EdgeT::VertexType> verts = temp.vertices();
    directed_network<EdgeT> eg;
    auto events = temp.incident_edges();
    for (const auto &v: verts) {
      auto events_set = events[v];
      std::vector<EdgeT> events_vec(
          events_vec.begin(), events_vec.end());
      std::sort(events_vec.begin(), events_vec.end());

      for (auto e1 = events.begin(); e1 < events.end(); e1++)
        for (auto e2 = e1; e2 < events.end() &&
              (e2->time < e1->effect_time() ||
               e2.time - e1.effect_time() < max_delta_t); e2++)
          if (e1->is_adjacent_to(*e2))
            eg.add_edge(directed_edge<EdgeT>(*e1, *e2));
    }
    return eg;
  }


  template <typename VertT>
  std::vector<VertT> topological_order(
      const directed_network<VertT>& dir) {

    auto succs = dir.successors();
    auto in_degrees = dir.in_degree();

    std::vector<VertT> topo;
    std::queue<VertT> queue;

    for (const auto &p: in_degrees)
      if (p.second == 0) queue.push(p.first);

    while (!queue.empty()) {
      VertT v = queue.front();
      queue.pop();
      topo.push_back(v);
      for (const auto &ov: succs[v])
        if (--in_degrees[ov] == 0) queue.push(ov);
    }

    if (topo.size() < in_degrees.size())
      throw utils::not_acyclic_error("argument dir most be acyclic");

    return topo;
  }

  template <typename VertT>
  std::unordered_set<VertT> out_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint,
      bool revert_graph) {
    std::unordered_set<VertT> out_component;
    if (size_hint > 0) out_component.reserve(size_hint);
    out_component.insert(vert);
    auto topo = topological_order(dir);
    auto out_edges = (revert_graph ? dir.in_edges() : dir.out_edges());
    for (const VertT& vert: topo)
      if (out_component.find(vert) != out_component.end())
        for (const auto& edge: out_edges[vert])
          out_component.insert(edge.head_vert());
    return out_component;
  }

  template <typename VertT>
  std::unordered_set<VertT> in_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint) {
    return out_component(dir, vert, size_hint, true);
  }
};
