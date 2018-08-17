#include <queue>
#include <random>
#include <cmath>


namespace dag {
  template <typename VertT, typename TimeT>
  directed_network<undirected_temporal_edge<VertT, TimeT>>
  event_graph(const undirected_temporal_network<VertT, TimeT>& temp,
      TimeT max_delta_t) {
    std::unordered_set<VertT> verts = temp.vertices();
    directed_network<undirected_temporal_edge<VertT, TimeT>> eg;
    auto incident_events = temp.incident_edges();
    for (const auto &v: verts) {
      auto events_set = incident_events[v];
      std::vector<undirected_temporal_edge<VertT, TimeT>> events(
          events_set.begin(), events_set.end());
      std::sort(events.begin(), events.end());
      for (size_t i = 0; i < events.size(); i++)
        for (size_t j = i+1;
            j < events.size() &&
            events[j].time > events[i].time &&
            events[j].time - events[i].time <= max_delta_t;
            j++) {
          directed_edge<undirected_temporal_edge<VertT, TimeT>>
            e(events[i], events[j]);
          eg.add_edge(e);
        }
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
      throw not_acyclic_error("argument dir most be acyclic");

    return topo;
  }
};
