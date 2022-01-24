#ifndef INCLUDE_DAG_TEMPORAL_COMPONENTS_HPP_
#define INCLUDE_DAG_TEMPORAL_COMPONENTS_HPP_

#include "network_concepts.hpp"
#include "implicit_event_graph.hpp"

namespace dag {
  // Event graph in-components
  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<std::pair<EdgeT,
    component<directed_edge<EdgeT>>>
  in_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg);

  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<std::pair<EdgeT,
    component_size<directed_edge<EdgeT>>>
  in_component_sizes(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg);

  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<std::pair<EdgeT, component_size_estimate<EdgeT>>
  in_component_size_estimates(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      std::size_t estimator_seed = 0);

  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  component<EdgeT>
  in_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      std::size_t node_size_hint = 0,
      std::size_t edge_size_hint = 0);

  // Event graph out-components
  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<std::pair<EdgeT, component<EdgeT>>
  out_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg);

  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<std::pair<EdgeT, component_size<EdgeT>>
  out_component_sizes(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg);

  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<std::pair<EdgeT, component_size_estimate<EdgeT>>
  out_component_size_estimates(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      std::size_t estimator_seed = 0);

  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  component<EdgeT>
  out_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      std::size_t node_size_hint = 0,
      std::size_t edge_size_hint = 0);


  // event graph weekly connected components
  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<component<EdgeT>>
  weakly_connected_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      bool singletons = true);

  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<component_size<EdgeT>>
  weakly_connected_component_sizes(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      bool singletons = true);

  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<component_size_estimate<EdgeT>>
  weakly_connected_component_size_estimates(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      bool singletons = true,
      size_t estimator_seed = 0);

  template <
    temporal_edge EdgeT,
    adjacency_prob::adjacency_prob AdjacencyProbT>
  component<EdgeT>
  weakly_connected_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      std::size_t node_size_hint = 0,
      std::size_t edge_size_hint = 0);
}  // namespace dag

#include "../../src/implicit_event_graph_components.tpp"

#endif  // INCLUDE_DAG_TEMPORAL_COMPONENTS_HPP_
