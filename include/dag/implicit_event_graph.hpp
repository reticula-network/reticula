#ifndef INCLUDE_DAG_IMPLICIT_EVENT_GRAPH_HPP_
#define INCLUDE_DAG_IMPLICIT_EVENT_GRAPH_HPP_

#include <utility>
#include <vector>
#include <unordered_map>

#include "temporal_edges.hpp"
#include "networks.hpp"
#include "network_concepts.hpp"
#include "adjacency_prob.hpp"

namespace dag {
  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  class implicit_event_graph {
  public:
    using EdgeType = EdgeT;
    using TimeType = typename EdgeType::TimeType;
    using VertexType = typename EdgeType::VertexType;

    implicit_event_graph() = default;
    implicit_event_graph(
        const std::vector<EdgeType>& events,
        const AdjacencyProbT& prob,
        size_t seed);
    implicit_event_graph(
        const network<EdgeType>& temp,
        const AdjacencyProbT& prob,
        size_t seed);

    const std::vector<EdgeType>& events_cause() const;
    const std::vector<EdgeType>& events_effect() const;

    AdjacencyProbT adjacency_prob() const;
    size_t seed() const;

    std::pair<TimeType, TimeType> time_window() const;

    std::vector<EdgeType>
    predecessors(const EdgeType& e, bool just_first = false) const;

    std::vector<EdgeType>
    successors(const EdgeType& e, bool just_first = false) const;

  private:
    size_t _seed;
    network<EdgeType> _temp;
    AdjacencyProbT _prob;

    std::vector<EdgeType>
    successors_vert(const EdgeType& e, VertexType v, bool just_first) const;

    std::vector<EdgeType>
    predecessors_vert(const EdgeType& e, VertexType v, bool just_first) const;
  };

}  // namespace dag

#include "../../src/implicit_event_graph.tpp"

#endif  // INCLUDE_DAG_IMPLICIT_EVENT_GRAPH_HPP_
