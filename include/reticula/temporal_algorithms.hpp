#ifndef INCLUDE_RETICULA_TEMPORAL_ALGORITHMS_HPP_
#define INCLUDE_RETICULA_TEMPORAL_ALGORITHMS_HPP_

#include <vector>
#include <utility>

#include "network_concepts.hpp"
#include "networks.hpp"
#include "temporal_adjacency.hpp"
#include "temporal_clusters.hpp"

namespace reticula {
  /**
    Finds the range of times present in the events. If temporal edges have
    different cause and effect times (i.e. they have delays) this would be
    equivalent to minimum cause time and maximum effect time.
  */
  template <temporal_network_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  time_window(const network<EdgeT>& temp);

  /**
    Finds the range of cause times present in the events. If temporal edges have
    don't haver different cause and effect times (i.e. they are not delayed)
    this would be equivalent to time_window.
  */
  template <temporal_network_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  cause_time_window(const network<EdgeT>& temp);

  /**
    Finds the range of effect times present in the events. If temporal edges
    have don't haver different cause and effect times (i.e. they are not
    delayed) this would be equivalent to time_window.
  */
  template <temporal_network_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  effect_time_window(const network<EdgeT>& temp);


  /**
    Generates the event graph representation of the temporal network.

    @param temp A temporal network
    @param adj A `temporal_adjacency` class limiting the adjacency relationship
    between two otherwise adjacent events.
  */
  template <
    temporal_network_edge EdgeT,
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
    temporal_network_edge EdgeT,
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
    temporal_network_edge EdgeT,
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
    temporal_network_edge EdgeT,
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
    temporal_network_edge EdgeT,
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
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<
    std::pair<EdgeT, temporal_cluster_size_estimate<EdgeT, AdjT>>>
  out_cluster_size_estimates(
          const network<EdgeT>& temp,
          const AdjT& adj,
          typename EdgeT::TimeType temporal_resolution,
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
    temporal_network_edge EdgeT,
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
    temporal_network_edge EdgeT,
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
    temporal_network_edge EdgeT,
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
    temporal_network_edge EdgeT,
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
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<
    std::pair<EdgeT, temporal_cluster_size_estimate<EdgeT, AdjT>>>
  in_cluster_size_estimates(
          const network<EdgeT>& temp,
          const AdjT& adj,
          typename EdgeT::TimeType temporal_resolution,
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
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  bool is_reachable(
      const network<EdgeT>& net,
      const AdjT& adj,
      const typename EdgeT::VertexType& source,
      typename EdgeT::TimeType t0,
      const typename EdgeT::VertexType& destination,
      typename EdgeT::TimeType t1);

  /**
    Return the static projection of the temporal network. The resulting static
    network has the same set vertices as the temporal network and two nodes are
    connected if there is at least one event connecting the two.
  */
  template <temporal_network_edge EdgeT>
  network<typename EdgeT::StaticProjectionType>
  static_projection(const network<EdgeT>& temp);

  /**
    Returns a vector of unique events, sorted by cause time, that have a static
    projection equal to the parameter `link`, i.e., the timeline of that link.
  */
  template <temporal_network_edge EdgeT>
  std::vector<EdgeT>
  link_timeline(
      const network<EdgeT>& net,
      const typename EdgeT::StaticProjectionType& link);

  /**
    Returns the sorted vector or events, or the timeline, for each edge of the
    static projection of the temporal networks.
  */
  template <temporal_network_edge EdgeT>
  std::vector<
    std::pair<typename EdgeT::StaticProjectionType, std::vector<EdgeT>>>
  link_timelines(const network<EdgeT>& net);
}  // namespace reticula

// Implementation
#include "implicit_event_graphs.hpp"
#include "implicit_event_graph_components.hpp"

namespace reticula {
  namespace detail {
    template <temporal_network_edge EdgeT>
    struct temporal_loop {
      EdgeT
      operator()(typename EdgeT::VertexType v, typename EdgeT::TimeType t);
    };

    template <network_vertex VertT, typename TimeT>
    struct temporal_loop<undirected_temporal_edge<VertT, TimeT>> {
      undirected_temporal_edge<VertT, TimeT>
      operator()(VertT v, TimeT t) {
        return {v, v, t};
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct temporal_loop<directed_temporal_edge<VertT, TimeT>> {
      directed_temporal_edge<VertT, TimeT>
      operator()(VertT v, TimeT t) {
        return {v, v, t};
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct temporal_loop<directed_delayed_temporal_edge<VertT, TimeT>> {
      directed_delayed_temporal_edge<VertT, TimeT>
      operator()(VertT v, TimeT t) {
        return {v, v, t, t};
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct temporal_loop<undirected_temporal_hyperedge<VertT, TimeT>> {
      undirected_temporal_hyperedge<VertT, TimeT>
      operator()(VertT v, TimeT t) {
        return {{v}, t};
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct temporal_loop<directed_temporal_hyperedge<VertT, TimeT>> {
      directed_temporal_hyperedge<VertT, TimeT>
      operator()(VertT v, TimeT t) {
        return {{v}, {v}, t};
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct temporal_loop<
        directed_delayed_temporal_hyperedge<VertT, TimeT>> {
      directed_delayed_temporal_hyperedge<VertT, TimeT>
      operator()(VertT v, TimeT t) {
        return {{v}, {v}, t, t};
      }
    };

    template <temporal_adjacency::temporal_adjacency AdjT>
    struct ieg_component_type_constructor<
        temporal_cluster<typename AdjT::EdgeType, AdjT>,
        AdjT, typename AdjT::EdgeType::TimeType> {
      temporal_cluster<typename AdjT::EdgeType, AdjT>
      operator()(AdjT adj, typename AdjT::EdgeType::TimeType /* dt */,
          std::size_t size_est, std::size_t /* seed */) {
        return temporal_cluster<typename AdjT::EdgeType, AdjT>(
            adj, size_est);
      }
    };

    template <temporal_adjacency::temporal_adjacency AdjT>
    struct ieg_component_type_constructor<
        temporal_cluster_sketch<typename AdjT::EdgeType, AdjT>,
        AdjT, typename AdjT::EdgeType::TimeType> {
      temporal_cluster_sketch<typename AdjT::EdgeType, AdjT>
      operator()(AdjT adj, typename AdjT::EdgeType::TimeType dt,
          std::size_t /* size_est */, std::size_t seed) {
        return temporal_cluster_sketch<typename AdjT::EdgeType, AdjT>(
            adj, dt, seed);
      }
    };
  }  // namespace detail

  template <temporal_network_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  time_window(const network<EdgeT>& temp) {
    if (temp.edges_cause().empty())
      throw std::invalid_argument(
          "time window of a temporal network with no events is undefined.");
    return {
      temp.edges_cause().front().cause_time(),
      temp.edges_effect().back().effect_time()};
  }

  template <temporal_network_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  cause_time_window(const network<EdgeT>& temp) {
    if (temp.edges_cause().empty())
      throw std::invalid_argument(
          "time window of a temporal network with no events is undefined.");
    return {
      temp.edges_cause().front().cause_time(),
      temp.edges_cause().back().cause_time()};
  }

  template <temporal_network_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  effect_time_window(const network<EdgeT>& temp) {
    if (temp.edges_effect().empty())
      throw std::invalid_argument(
          "time window of a temporal network with no events is undefined.");
    return {
      temp.edges_effect().front().effect_time(),
      temp.edges_effect().back().effect_time()};
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  directed_network<EdgeT>
  event_graph(
      const network<EdgeT>& temp,
      const AdjT& adj) {
    auto verts = temp.vertices();
    std::vector<directed_edge<EdgeT>> eg;
    for (const auto &v: verts) {
      auto events = temp.incident_edges(v);

      for (auto e1 = events.begin(); e1 < events.end(); e1++) {
        typename EdgeT::TimeType cutoff{};
        for (auto& v: e1->mutated_verts())
          cutoff = std::max(cutoff, adj.linger(*e1, v));

        for (auto e2 = e1 + 1;
            e2 < events.end() &&
            e2->cause_time() - e1->effect_time() <= cutoff; e2++) {
          if (adjacent(*e1, *e2))
            eg.emplace_back(*e1, *e2);
        }
      }
    }

    return directed_network<EdgeT>(eg);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>
  out_cluster(
          const network<EdgeT>& temp,
          const AdjT& adj,
          const typename EdgeT::VertexType& v,
          typename EdgeT::TimeType t) {
    return out_cluster(temp, adj, detail::temporal_loop<EdgeT>{}(v, t));
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>
  out_cluster(
          const network<EdgeT>& temp,
          const AdjT& adj,
          const EdgeT& e) {
    return temporal_cluster<EdgeT, AdjT>(
        out_component(implicit_event_graph(temp, adj), e), adj);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, temporal_cluster<EdgeT, AdjT>>>
  out_clusters(
          const network<EdgeT>& temp,
          const AdjT& adj) {
    return detail::out_components<
      EdgeT, AdjT,
      temporal_cluster<EdgeT, AdjT>,
      temporal_cluster<EdgeT, AdjT>>(implicit_event_graph(temp, adj),
          0, 0);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, temporal_cluster_size<EdgeT, AdjT>>>
  out_cluster_sizes(
          const network<EdgeT>& temp,
          const AdjT& adj) {
    return detail::out_components<
      EdgeT, AdjT,
      temporal_cluster<EdgeT, AdjT>,
      temporal_cluster_size<EdgeT, AdjT>>(implicit_event_graph(temp, adj),
          0, 0);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<
    std::pair<EdgeT, temporal_cluster_size_estimate<EdgeT, AdjT>>>
  out_cluster_size_estimates(
          const network<EdgeT>& temp,
          const AdjT& adj,
          typename EdgeT::TimeType temporal_resolution,
          std::size_t seed) {
    return detail::out_components<
      EdgeT, AdjT,
      temporal_cluster_sketch<EdgeT, AdjT>,
      temporal_cluster_size_estimate<EdgeT, AdjT>>(
          implicit_event_graph(temp, adj), temporal_resolution, seed);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>
  in_cluster(
          const network<EdgeT>& temp,
          const AdjT& adj,
          const typename EdgeT::VertexType& v,
          typename EdgeT::TimeType t) {
    return in_cluster(temp, adj, detail::temporal_loop<EdgeT>{}(v, t));
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  temporal_cluster<EdgeT, AdjT>
  in_cluster(
          const network<EdgeT>& temp,
          const AdjT& adj,
          const EdgeT& e) {
    return temporal_cluster<EdgeT, AdjT>(
        in_component(implicit_event_graph(temp, adj), e), adj);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, temporal_cluster<EdgeT, AdjT>>>
  in_clusters(
          const network<EdgeT>& temp,
          const AdjT& adj) {
    return detail::in_components<
      EdgeT, AdjT,
      temporal_cluster<EdgeT, AdjT>,
      temporal_cluster<EdgeT, AdjT>>(implicit_event_graph(temp, adj),
          0, 0);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<std::pair<EdgeT, temporal_cluster_size<EdgeT, AdjT>>>
  in_cluster_sizes(
          const network<EdgeT>& temp,
          const AdjT& adj) {
    return detail::in_components<
      EdgeT, AdjT,
      temporal_cluster<EdgeT, AdjT>,
      temporal_cluster_size<EdgeT, AdjT>>(implicit_event_graph(temp, adj),
          0, 0);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<
    std::pair<EdgeT, temporal_cluster_size_estimate<EdgeT, AdjT>>>
  in_cluster_size_estimates(
          const network<EdgeT>& temp,
          const AdjT& adj,
          typename EdgeT::TimeType temporal_resolution,
          std::size_t seed) {
    return detail::in_components<
      EdgeT, AdjT,
      temporal_cluster_sketch<EdgeT, AdjT>,
      temporal_cluster_size_estimate<EdgeT, AdjT>>(
          implicit_event_graph(temp, adj), temporal_resolution, seed);
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  bool is_reachable(
      const network<EdgeT>& net,
      const AdjT& adj,
      const typename EdgeT::VertexType& source,
      typename EdgeT::TimeType t0,
      const typename EdgeT::VertexType& destination,
      typename EdgeT::TimeType t1) {
    if (t1 < t0)
      return false;

    return out_cluster(
        net, adj, detail::temporal_loop<EdgeT>{}(source, t0)).covers(
          destination, t1);
  }

  template <temporal_network_edge EdgeT>
  network<typename EdgeT::StaticProjectionType>
  static_projection(const network<EdgeT>& temp) {
    return network<typename EdgeT::StaticProjectionType>(
        temp.edges_cause() | views::transform([](const auto& e){
          return e.static_projection();
        }), temp.vertices());
  }

  template <temporal_network_edge EdgeT>
  std::vector<EdgeT>
  link_timeline(
      const network<EdgeT>& net,
      const typename EdgeT::StaticProjectionType& link) {
    auto node = ranges::min(
        link.mutator_verts(), ranges::less{},
        [&net](const auto& n) { return net.out_edges(n).size(); });

    std::vector<EdgeT> res;
    res.reserve(std::min(
          net.edges().size()*3/net.vertices().size(),
          net.out_edges(node).size()));

    for (auto& event: net.out_edges(node))
      if (event.static_projection() == link)
        res.push_back(event);

    return res;
  }

  template <temporal_network_edge EdgeT>
  std::vector<
    std::pair<typename EdgeT::StaticProjectionType, std::vector<EdgeT>>>
  link_timelines(const network<EdgeT>& net) {
    std::unordered_map<
      typename EdgeT::StaticProjectionType, std::vector<EdgeT>,
      hash<typename EdgeT::StaticProjectionType>> timelines;

    for (auto& event: net.edges_cause())
      timelines[event.static_projection()].push_back(event);

    return std::vector<
      std::pair<
          typename EdgeT::StaticProjectionType, std::vector<EdgeT>>>(
        timelines.begin(),
        timelines.end());
  }
}  // namespace reticula


#endif  // INCLUDE_RETICULA_TEMPORAL_ALGORITHMS_HPP_
