#ifndef INCLUDE_DAG_TEMPORAL_ALGORITHMS_HPP_
#define INCLUDE_DAG_TEMPORAL_ALGORITHMS_HPP_

#include <vector>
#include <utility>

#include "network_concepts.hpp"
#include "networks.hpp"
#include "temporal_adjacency.hpp"
#include "temporal_clusters.hpp"

namespace dag {
  /**
    Finds the range of times present in the events. If temporal edges have
    different cause and effect times (i.e. they have delays) this would be
    equivalent to minimum cause time and maximum effect time.
  */
  template <temporal_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  time_window(const network<EdgeT>& temp);

  /**
    Finds the range of cause times present in the events. If temporal edges have
    don't haver different cause and effect times (i.e. they are not delayed)
    this would be equivalent to time_window.
  */
  template <temporal_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  cause_time_window(const network<EdgeT>& temp);

  /**
    Finds the range of effect times present in the events. If temporal edges
    have don't haver different cause and effect times (i.e. they are not
    delayed) this would be equivalent to time_window.
  */
  template <temporal_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  effect_time_window(const network<EdgeT>& temp);


  /**
    Generates the event graph representation of the temporal network.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  directed_network<EdgeT>
  event_graph(
      const network<EdgeT>& temp,
      const AdjT& adj);


  /**
    Finds the set of events that transmit a spreading process starting at the
    source node at the specified time.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
    @param v Initial spreading vector vertex
    @param t Starting time of the spreading process
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>
  out_cluster(
          const network<EdgeT>& temp,
          const AdjT& adj,
          const typename EdgeT::VertexType& v,
          typename EdgeT::TimeType t);

  /**
    Finds the set of events that transmit a spreading process starting by an
    event.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
    @param e Initial spreading vector event
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>
  out_cluster(
          const network<EdgeT>& temp,
          const AdjT& adj,
          const EdgeT& e);

  /**
    Finds the set of events that transmit a spreading process starting at each
    event.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, temporal_cluster<EdgeT, AdjT>>>
  out_clusters(
          const network<EdgeT>& temp,
          const AdjT& adj);

  /**
    Finds the set of events that transmit a spreading process starting at each
    event.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, temporal_cluster_size<EdgeT, AdjT>>>
  out_cluster_sizes(
          const network<EdgeT>& temp,
          const AdjT& adj);


  /**
    Finds the estimated set of events that transmit a spreading process starting
    at each event.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT,
    typename EdgeT::TimeType dt = static_cast<typename EdgeT::TimeType>(1)>
  std::vector<
    std::pair<EdgeT, temporal_cluster_size_estimate<EdgeT, AdjT, dt>>>
  out_cluster_size_estimates(
          const network<EdgeT>& temp,
          const AdjT& adj,
          std::size_t seed);

  /**
    Finds the set of events where a spreading process starting there would be
    transmitted to the specified node at the specified time.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
    @param v Destination node
    @param t Destination time
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>
  in_cluster(
          const network<EdgeT>& temp,
          const AdjT& adj,
          const typename EdgeT::VertexType& v,
          typename EdgeT::TimeType t);

  /**
    Finds the set of events that transmit a spreading process starting there
    would eventually involve a specific event.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
    @param e Initial spreading vector event
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>
  in_cluster(
          const network<EdgeT>& temp,
          const AdjT& adj,
          const EdgeT& e);

  /**
    For each event, finds the set of initial events that a spreading process
    starting there would spread to the event in question.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, temporal_cluster<EdgeT, AdjT>>>
  in_clusters(
          const network<EdgeT>& temp,
          const AdjT& adj);

  /**
    For each event, finds the set of initial events that a spreading process
    starting there would spread to the event in question.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, temporal_cluster_size<EdgeT, AdjT>>>
  in_cluster_sizes(
          const network<EdgeT>& temp,
          const AdjT& adj);

  /**
    For each event, finds the estimated set of initial events that a spreading
    process starting there would spread to the event in question.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT,
    typename EdgeT::TimeType dt = static_cast<typename EdgeT::TimeType>(1.0)>
  std::vector<
    std::pair<EdgeT, temporal_cluster_size_estimate<EdgeT, AdjT, dt>>>
  in_cluster_size_estimates(
          const network<EdgeT>& temp,
          const AdjT& adj,
          std::size_t seed);

  /**
    Returns true if node `destination` can be reached at time `t1` by following
    temporal events starting from node `source` at time `t0`. 

    @param net The network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    @param source The starting point of the reachability query
    @param t0 The starting time of the reachability query
    @param destination The end point of the reachability query
    @param t1 The end time of the reachability query
  */
  template <
    temporal_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  bool is_reachable(
      const network<EdgeT>& net,
      const AdjT& adj,
      const typename EdgeT::VertexType& source,
      typename EdgeT::TimeType t0,
      const typename EdgeT::VertexType& destination,
      typename EdgeT::TimeType t1);
}  // namespace dag

#include "../../src/temporal_algorithms.tpp"

#endif  // INCLUDE_DAG_TEMPORAL_ALGORITHMS_HPP_
