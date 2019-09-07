#ifndef DAG_IMPLICIT_EVENT_GRAPH_H
#define DAG_IMPLICIT_EVENT_GRAPH_H

#include <unordered_set>
#include "temporal_edges.hpp"

namespace dag {
  namespace adjacency_prob {
    template <class EdgeT>
    class deterministic {
    public:
      deterministic(typename EdgeT::TimeType dt);
      double p(const EdgeT& a, const EdgeT& b) const;
    private:
      typename EdgeT::TimeType _dt;
    };


    template <class EdgeT>
    class exponential {
    public:
      exponential(typename EdgeT::TimeType expected_dt);
      double p(const EdgeT& a, const EdgeT& b) const;
    private:
      typename EdgeT::TimeType _dt;
    };
  }


  template <class EdgeT, class AdjacencyProbT>
  class implicit_event_graph {
  public:
    using TimeType = typename EdgeT::TimeType;
    using VertexType = typename EdgeT::VertexType;

    implicit_event_graph() = default;
    implicit_event_graph(std::vector<EdgeT> events,
        const AdjacencyProbT& prob, size_t seed);

    const std::vector<EdgeT>& topo() const;
    size_t event_count() const;
    size_t node_count() const;
    std::pair<TimeType, TimeType> time_window() const;

    std::vector<EdgeT>
    predecessors(const EdgeT& e, bool just_first=false) const;

    std::vector<EdgeT>
    successors(const EdgeT& e, bool just_first=false) const;


    // TODO: should I even have this? I kinda wanna go full immutable on this
    void remove_events(const std::unordered_set<EdgeT>& events);
  private:
    size_t seed;
    std::vector<EdgeT> _topo;
    std::unordered_map<VertexType, std::vector<EdgeT>> inc_in_map, inc_out_map;

    AdjacencyProbT prob;

    bool bernoulli_trial(const EdgeT& a, const EdgeT& b, double p) const;

    std::vector<EdgeT>
    successors_vert(const EdgeT& e, VertexType v, bool just_first) const;

    std::vector<EdgeT>
    predecessors_vert(const EdgeT& e, VertexType v, bool just_first) const;

    // Only enable deterministic shortcut (i.e. arXiv 1908.11831 appendix B) for
    // undirected temporal edges
    static constexpr bool enable_deterministic_shortcut = std::is_same<
      EdgeT, dag::undirected_temporal_edge<VertexType, TimeType>>::value;

    static constexpr bool is_prob_deterministic = std::is_same<
      AdjacencyProbT, dag::adjacency_prob::deterministic<EdgeT>>::value;
  };

}

#include "../src/implicit_event_graph.tpp"

#endif /* DAG_IMPLICIT_EVENT_GRAPH_H */
