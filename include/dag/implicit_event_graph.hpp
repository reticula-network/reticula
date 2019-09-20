#ifndef INCLUDE_DAG_IMPLICIT_EVENT_GRAPH_HPP_
#define INCLUDE_DAG_IMPLICIT_EVENT_GRAPH_HPP_

#include <utility>
#include <vector>
#include <unordered_map>

#include "temporal_edges.hpp"
#include "networks.hpp"

namespace dag {
  template <class EdgeT, class AdjacencyProbT>
  class implicit_event_graph {
  public:
    using TimeType = typename EdgeT::TimeType;
    using VertexType = typename EdgeT::VertexType;

    implicit_event_graph() = default;
    implicit_event_graph(
        const std::vector<EdgeT>& events,
        const AdjacencyProbT& prob,
        size_t seed);
    implicit_event_graph(
        const network<EdgeT>& temp,
        const AdjacencyProbT& prob,
        size_t seed);

    const std::vector<EdgeT>& events_cause() const;
    const std::vector<EdgeT>& events_effect() const;
    AdjacencyProbT adjacency_prob() const;

    std::pair<TimeType, TimeType> time_window() const;

    std::vector<EdgeT>
    predecessors(const EdgeT& e, bool just_first = false) const;

    std::vector<EdgeT>
    successors(const EdgeT& e, bool just_first = false) const;

  private:
    size_t _seed;
    network<EdgeT> _temp;
    AdjacencyProbT _prob;

    bool bernoulli_trial(const EdgeT& a, const EdgeT& b, double p) const;

    std::vector<EdgeT>
    successors_vert(const EdgeT& e, VertexType v, bool just_first) const;

    std::vector<EdgeT>
    predecessors_vert(const EdgeT& e, VertexType v, bool just_first) const;
  };

}  // namespace dag

#include "../../src/implicit_event_graph.tpp"

#endif  // INCLUDE_DAG_IMPLICIT_EVENT_GRAPH_HPP_
