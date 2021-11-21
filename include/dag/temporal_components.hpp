#ifndef INCLUDE_DAG_TEMPORAL_COMPONENTS_HPP_
#define INCLUDE_DAG_TEMPORAL_COMPONENTS_HPP_

#include "network_concepts.hpp"
#include "implicit_event_graph.hpp"

namespace dag {
  template <temporal_edge EdgeT,
          template<typename> class NodeEstimatorT,
          template<typename> class EdgeEstimatorT = NodeEstimatorT>
  class temporal_component {
  public:
    using EdgeType = EdgeT;
    using TimeType = typename EdgeType::TimeType;
    using VertexType = typename EdgeType::VertexType;

    explicit temporal_component(size_t seed,
        size_t node_est = 0,
        size_t edge_est = 0):
      _node_set(seed, node_est),
      _edge_set(seed, edge_est),
      _min_cause_time(std::numeric_limits<TimeType>::max()),
      _max_cause_time(std::numeric_limits<TimeType>::lowest()),
      _min_effect_time(std::numeric_limits<TimeType>::max()),
      _max_effect_time(std::numeric_limits<TimeType>::lowest()) {}

    template <template<typename> class OtherNodeEstimatorT,
            template<typename> class OtherEdgeEstimatorT>
    temporal_component(const temporal_component<EdgeType,
        OtherNodeEstimatorT,
        OtherEdgeEstimatorT>& other):
      _node_set(other.node_set()),
      _edge_set(other.edge_set()),
      _min_cause_time(other.cause_lifetime().first),
      _max_cause_time(other.cause_lifetime().second),
      _min_effect_time(other.effect_lifetime().first),
      _max_effect_time(other.effect_lifetime().second) {}

    void insert(const EdgeType& e, const std::vector<VertexType>& nodes);
    void merge(
      const temporal_component<
        EdgeType, NodeEstimatorT, EdgeEstimatorT>& other);

    const NodeEstimatorT<VertexType>& node_set() const { return _node_set; }
    const EdgeEstimatorT<EdgeType>& edge_set() const { return _edge_set; }

    std::pair<TimeType, TimeType> cause_lifetime() const {
      return std::make_pair(_min_cause_time, _max_cause_time);
    }
    std::pair<TimeType, TimeType> effect_lifetime() const {
      return std::make_pair(_min_effect_time, _max_effect_time);
    }

  private:
    NodeEstimatorT<VertexType> _node_set;
    EdgeEstimatorT<EdgeType> _edge_set;

    TimeType _min_cause_time, _max_cause_time;
    TimeType _min_effect_time, _max_effect_time;
  };
}  // namespace dag

#include "../../src/temporal_components.tpp"


namespace dag {
  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT,
           template<typename> class ReadOnlyEstimatorT>
  std::vector<std::pair<EdgeT,
    temporal_component<EdgeT, ReadOnlyEstimatorT>>>
  in_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      size_t seed,
      bool only_roots = false);


  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT,
           template<typename> class ReadOnlyEstimatorT>
  std::vector<std::pair<EdgeT,
    temporal_component<EdgeT, ReadOnlyEstimatorT>>>
  out_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      size_t seed,
      bool only_roots = false);

  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  out_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      size_t seed,
      size_t node_size_hint = 0,
      size_t edge_size_hint = 0);


  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  in_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      size_t seed,
      size_t node_size_hint = 0,
      size_t edge_size_hint = 0);

  template <temporal_edge EdgeT,
           adjacency_prob::adjacency_prob AdjacencyProbT,
           template<typename> class EstimatorT>
  std::vector<temporal_component<EdgeT, EstimatorT>>
  weakly_connected_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      size_t seed,
      bool singletons = true);
}  // namespace dag

#include "../../src/temporal_component_sizes.tpp"

#endif  // INCLUDE_DAG_TEMPORAL_COMPONENTS_HPP_
