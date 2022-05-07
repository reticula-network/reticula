#ifndef INCLUDE_RETICULA_IMPLICIT_EVENT_GRAPH_COMPONENTS_HPP_
#define INCLUDE_RETICULA_IMPLICIT_EVENT_GRAPH_COMPONENTS_HPP_

#include <limits>
#include <utility>
#include <vector>

#include "network_concepts.hpp"
#include "temporal_adjacency.hpp"
#include "components.hpp"
#include "implicit_event_graphs.hpp"

namespace reticula {
  // Event graph in-components
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component<EdgeT>>>
  in_components(
      const implicit_event_graph<EdgeT, AdjT>& eg);

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component_size<EdgeT>>>
  in_component_sizes(
      const implicit_event_graph<EdgeT, AdjT>& eg);

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component_size_estimate<EdgeT>>>
  in_component_size_estimates(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      std::size_t estimator_seed = 0);

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  component<EdgeT>
  in_component(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      const EdgeT& root);

  // Event graph out-components
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component<EdgeT>>>
  out_components(
      const implicit_event_graph<EdgeT, AdjT>& eg);

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component_size<EdgeT>>>
  out_component_sizes(
      const implicit_event_graph<EdgeT, AdjT>& eg);

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, component_size_estimate<EdgeT>>>
  out_component_size_estimates(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      std::size_t estimator_seed);

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  component<EdgeT>
  out_component(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      const EdgeT& root);


  // event graph weekly connected components
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<component<EdgeT>>
  weakly_connected_components(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      bool singletons = true);

  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  component<EdgeT>
  weakly_connected_component(
      const implicit_event_graph<EdgeT, AdjT>& eg,
      const EdgeT& vert);
}  // namespace reticula

#include "../../src/implicit_event_graph_components.tpp"

#endif  // INCLUDE_RETICULA_IMPLICIT_EVENT_GRAPH_COMPONENTS_HPP_
