#ifndef INCLUDE_RETICULA_IMPLICIT_EVENT_GRAPHS_HPP_
#define INCLUDE_RETICULA_IMPLICIT_EVENT_GRAPHS_HPP_

#include <utility>
#include <unordered_map>

#include "temporal_edges.hpp"
#include "networks.hpp"
#include "network_concepts.hpp"
#include "temporal_adjacency.hpp"

namespace reticula {
  /**
    Generic class representing event graph of a temporal network, specifically
    for temporal networks large enough so that the event graph would not fit in
    memory. This class generates successors and predecessors of an event on the
    fly.

    @tparam EdgeT Edge type (i.e. event type) for this network. Different edge
    types encapsulate different behaviour in networks, e.g. a directed temporal
    network is a temporal network with directed events.
    @tparam AdjT Temporal adjacency class that determines whether two logically
    adjacent events are actually adjacent, i.e. that an effect transmitted in
    the first event is also tranmitted in the second event.
   */
  template <temporal_edge EdgeT, temporal_adjacency::temporal_adjacency AdjT>
  class implicit_event_graph {
  public:
    using EdgeType = EdgeT;
    using TimeType = typename EdgeType::TimeType;
    using VertexType = typename EdgeType::VertexType;

    implicit_event_graph() = default;

    /**
       Create an implicit event graph representation of a temporal network,
       constructed from a sequence of events. This variation is specifically
       created so that a brace-enclosed initializer list can be used to
       initialize the class.

       @param events An event sequence
       @param adj AdjT instance determinimg probability of whether two events
       are adjacent or not based on the events and the time delta.
     */
    implicit_event_graph(
        const std::initializer_list<EdgeType>& events,
        const AdjT& adj);
    /**
       Create an implicit event graph representation of a temporal network,
       constructed from a sequence of events and a supplementary set of vertices
       that might have no neighbours. This variation is specifically
       created so that a brace-enclosed initializer list can be used to
       initialize the class.

       @param events An event sequence
       @param verts A supplementary set of vertices that might have no
       neighbours and thereby not be present in `events`.
       @param adj AdjT instance determinimg whether two events are adjacent or
       not based on the events and the time delta.
     */
    implicit_event_graph(
        const std::initializer_list<EdgeType>& events,
        const std::initializer_list<VertexType>& verts,
        const AdjT& adj);

    /**
       Create an implicit event graph representation of a temporal network,
       constructed from a sequence of events.

       @param events An event sequence
       @param adj AdjT instance determinimg whether two events are adjacent or
       not based on the events and the time delta.
     */
    template <std::ranges::input_range Range>
    requires std::convertible_to<std::ranges::range_value_t<Range>, EdgeType>
    implicit_event_graph(
        Range&& events,
        const AdjT& prob);

    /**
       Create an implicit event graph representation of a temporal network,
       constructed from a sequence of events and a supplementary set of vertices
       that might have no neighbours.

       @param events An event sequence
       @param verts A supplementary set of vertices that might have no
       neighbours and thereby not be present in `events`.
       @param adj AdjT instance determinimg whether two events are adjacent or
       not based on the events and the time delta.
     */
    template <
      std::ranges::input_range EdgeRange,
      std::ranges::input_range VertRange>
    requires
      std::convertible_to<std::ranges::range_value_t<EdgeRange>, EdgeType> &&
      std::convertible_to<std::ranges::range_value_t<VertRange>, VertexType>
    implicit_event_graph(
        EdgeRange&& events,
        VertRange&& verts,
        const AdjT& adj);

    /**
       Create an implicit event graph representation of a temporal network,
       constructed from a pre-existing temporal network 

       @param temp A temporal network
       @param adj AdjT instance determinimg whether two events are adjacent or
       not based on the events and the time delta.
     */
    implicit_event_graph(
        const network<EdgeType>& temp,
        const AdjT& adj);

    /**
       The set of events represented in the original temporal network, sorted by
       operator<.
     */
    const std::vector<EdgeType>& events_cause() const;

    /**
       The set of events represented in the original temporal network, sorted by
       effect_lt.
     */
    const std::vector<EdgeType>& events_effect() const;

    /**
       The set of vertices represented in the original temporal network.
     */
    std::vector<VertexType> temporal_net_vertices() const;

    AdjT temporal_adjacency() const;


    std::pair<TimeType, TimeType> time_window() const;

    std::vector<EdgeType>
    predecessors(const EdgeType& e, bool just_first = false) const;

    std::vector<EdgeType>
    successors(const EdgeType& e, bool just_first = false) const;

    std::vector<EdgeType>
    neighbours(const EdgeType& e, bool just_first = false) const;

  private:
    network<EdgeType> _temp;
    AdjT _adj;

    std::vector<EdgeType>
    successors_vert(const EdgeType& e, VertexType v, bool just_first) const;

    std::vector<EdgeType>
    predecessors_vert(const EdgeType& e, VertexType v, bool just_first) const;
  };

}  // namespace reticula

#include "../../src/implicit_event_graphs.tpp"

#endif  // INCLUDE_RETICULA_IMPLICIT_EVENT_GRAPHS_HPP_
