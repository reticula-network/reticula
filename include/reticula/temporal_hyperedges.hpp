#ifndef INCLUDE_RETICULA_TEMPORAL_HYPEREDGES_HPP_
#define INCLUDE_RETICULA_TEMPORAL_HYPEREDGES_HPP_

#include <tuple>
#include <vector>
#include <algorithm>
#include <initializer_list>

#include <hll/hyperloglog.hpp>

#include "type_traits.hpp"
#include "network_concepts.hpp"
#include "static_hyperedges.hpp"

namespace reticula {
  template <network_vertex VertT, class TimeT>
  class undirected_temporal_hyperedge;

  template <network_vertex VertT, class TimeT>
  struct is_instantaneous<undirected_temporal_hyperedge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <network_vertex VertT, class TimeT>
  struct is_undirected<undirected_temporal_hyperedge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <network_vertex VertT, class TimeT>
  struct is_dyadic<undirected_temporal_hyperedge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <network_vertex VertT, class TimeT>
  class directed_temporal_hyperedge;

  template <network_vertex VertT, class TimeT>
  struct is_instantaneous<directed_temporal_hyperedge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <network_vertex VertT, class TimeT>
  struct is_undirected<directed_temporal_hyperedge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <network_vertex VertT, class TimeT>
  struct is_dyadic<directed_temporal_hyperedge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <network_vertex VertT, class TimeT>
  class directed_delayed_temporal_hyperedge;

  template <network_vertex VertT, class TimeT>
  struct is_instantaneous<directed_delayed_temporal_hyperedge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <network_vertex VertT, class TimeT>
  struct is_undirected<directed_delayed_temporal_hyperedge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <network_vertex VertT, class TimeT>
  struct is_dyadic<directed_delayed_temporal_hyperedge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  /**
    A directed temporal hyperedge (or hyperevent) indicate instantaneous
    unsymertic relation or orientation between the two nodes attributable to a
    specific point in time. Tail and head vertices allude to a representation of
    the hyperedge as an arrow from one vertex to another.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
    @tparam TimeT Type used for timestamps. Highly recommanded to use a
    numerical type like `size_t`, `int` or `double`.
  */
  template <network_vertex VertT, typename TimeT>
  class directed_temporal_hyperedge {
  public:
    /**
      Type used for labelling vertices.
     */
    using VertexType = VertT;

    /**
      Type used for labelling timestamps.
     */
    using TimeType = TimeT;

    /**
      Static projection of the temporal hyperedge.
     */
    using StaticProjectionType = directed_hyperedge<VertT>;

    directed_temporal_hyperedge() = default;

    /**
      Create an directed temporal hyperedge. This variation is specifically
      created so that a brace-enclosed initializer list can be used to
      initialize this class.

      @param tails The set of vertices at the tail end of the hyperedge, often
      the initiator or cause of the action or the relation.
      @param heads The set of vertices at the head end of the hyperedge, often
      the receiving end of an effect.
      @param time Timestamp at which the event "happened".
     */
    directed_temporal_hyperedge(
        std::initializer_list<VertexType> tails,
        std::initializer_list<VertexType> heads,
        TimeType time);

    /**
      Create a directed temporal hyperedge.

      @param tails The set of vertices at the tail end of the hyperedge, often
      the initiator or cause of the action or the relation.
      @param heads The set of nodes at the head end of the hyperedge, often in
      the receiving end of an effect.
      @param time Timestamp at which the event "happened".
     */
    template <std::ranges::input_range R1, std::ranges::input_range R2>
    requires
      std::convertible_to<std::ranges::range_value_t<R1>, VertT> &&
      std::convertible_to<std::ranges::range_value_t<R2>, VertT>
    directed_temporal_hyperedge(
        const R1& tails, const R2& heads, TimeType time);

    /**
      Create a directed temporal hyperedge from a static projection and a time.
      The resulting edge would have the same endpoints as projection and the
      time from the parameter `time`.

      @param projection The static projection of this temporal hyperedge.
      @param time Timestamp at which the event "happened"
     */
    directed_temporal_hyperedge(
        const StaticProjectionType& projection, TimeType time);

    /**
      Static hyperedge that encompasses all the non-temporal information about this
      hyperedge.
     */
    [[nodiscard]]
    StaticProjectionType
    static_projection() const;

    /**
      The timestamp that the effect is received by head vertex. For directed
      temporal hyperedges this is equal to the `cause_time` of the hyperedge.
     */
    [[nodiscard]]
    TimeType effect_time() const;

    /**
      The timestamp that the effect is initiated by tail vertex.
     */
    [[nodiscard]]
    TimeType cause_time() const;

    /**
      A directed temporal hyperedge is out_incident to vertex `v` iff `v` is in
      the tail set of vertices of the hyperedge.

      @param vert Vertex to check the out_incident relationship with.
     */
    [[nodiscard]]
    bool is_out_incident(const VertexType& vert) const;

    /**
      A directed temporal hyperedge is in_incident to vertex `v` iff `v` is in
      the head set of vertices of the hyperedge.

      @param vert Vertex to check the in_incident relationship with.
     */
    [[nodiscard]]
    bool is_in_incident(const VertexType& vert) const;

    /**
      A directed temporal hyperedge is incident to vertex `v` iff `v` is in the
      head or tail set of vertices of the hyperedge.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    bool is_incident(const VertexType& vert) const;

    /**
      List of all vertices that can initiate (cause) the effects of the
      relationship. For directed temporal hyperedges this is equal to the tail
      set of vertices.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const;

    /**
      List of all vertices that can receive (affected by) the effects of the
      relationship. For directed temporal hyperedges this is equal to the head
      set of vertices.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const;

    /**
      List of all vertices that can initiate (cause) or receive (be affected by)
      the effects of the relationship. For directed hyperedges this is equal to
      the union of tail and head sets of vertices.
     */
    [[nodiscard]]
    std::vector<VertexType> incident_verts() const;

    /**
      The same as `mutated_verts()`.
     */
    [[nodiscard]]
    std::vector<VertexType> tails() const;

    /**
      The same as `mutator_verts()`.
     */
    [[nodiscard]]
    std::vector<VertexType> heads() const;

    /**
      Defines a strong lexicographic ordering where cause times are compare then
      tail set and then head set.
     */
    auto operator<=>(
        const directed_temporal_hyperedge<
          VertexType, TimeType>&) const = default;

    /**
      Defines a strong lexicographic ordering along with `operator==` where
      cause times are compared then head vertices and then tail vertices.
     */
    template <network_vertex VertexType, typename TimeType>
    friend bool effect_lt(
        const directed_temporal_hyperedge<VertexType, TimeType>& a,
        const directed_temporal_hyperedge<VertexType, TimeType>& b);

    /**
      Two directed temporal hyperedges are adjacent if head of the first one has
      at least one element in common with the tail set of the second one and the
      cause time of the second one is after the first. Lack of an adjacency
      relation between hyperedges ususlly mean that an effect transmitted
      through one hyperedge logically cannot be transmitted through the other.
     */
    template <network_vertex VertexType, typename TimeType>
    friend bool adjacent(
        const directed_temporal_hyperedge<VertexType, TimeType>& a,
        const directed_temporal_hyperedge<VertexType, TimeType>& b);

  private:
    TimeType _time;
    std::vector<VertexType> _tails, _heads;

    friend struct std::hash<directed_temporal_hyperedge<VertexType, TimeType>>;
    friend struct hll::hash<directed_temporal_hyperedge<VertexType, TimeType>>;
  };

  /**
    A directed delayed temporal hyperedges (or hyperevent) indicate
    instantaneous unsymertic relation or orientation between the two sets of
    nodes attributable to a specific point in time as cause time and a delay.
    Tail and head sets of vertices allude to a representation of the hyperedge
    as an arrow from one set of vertices to another. Delay referes to the
    duration of time between start of the event to when the effect is
    transmitted to the head vertex.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
    @tparam TimeT Type used for timestamps. Highly recommanded to use a
    numerical type like `size_t`, `int` or `double`.
  */
  template <network_vertex VertT, typename TimeT>
  class directed_delayed_temporal_hyperedge {
  public:
    /**
      Type used for labelling vertices.
     */
    using VertexType = VertT;

    /**
      Type used for labelling timestamps.
     */
    using TimeType = TimeT;

    /**
      Static projection of the temporal hyperedge.
     */
    using StaticProjectionType = directed_hyperedge<VertT>;

    directed_delayed_temporal_hyperedge() = default;

    /**
      Create an directed temporal hyperedge. This variation is specifically
      created so that a brace-enclosed initializer list can be used to
      initialize this class.

      @param tails The set of vertices at the tail end of the hyperedge, often
      the initiator or cause of the action or the relation.
      @param heads The set of vertices at the head end of the hyperedge, often
      the receiving end of an effect.
      @param time Timestamp at which the event "happened".
      @param delay Timestamp at which the event was "received".
     */
    directed_delayed_temporal_hyperedge(
        std::initializer_list<VertexType> tails,
        std::initializer_list<VertexType> heads,
        TimeType cause_time, TimeType effect_time);

    /**
      Create a directed delayed temporal hyperedge.

      You can also think of a directed delayed temporal hyperedge as a "trip" of a
      bus between two consecutive stops in public transportation network or
      travel of a plain between two airports. In this case "the effect" is the
      arrival of passengers, the change is caused by departure of passengers
      from departure stop and the state of the destination stop is mutated by
      this effect.

      Mutation of state is a key abstraction here. Each node is conceptualised
      as a finite state machine. The only thing that can affect a transition (a
      mutation) in the state of a node is either internal to that node or comes
      in form of a temporal edge. In case of the latter the "cause" for that
      mutation would be one of the nodes that are in the tail end of the event.


      @param tails The set of vertices at the tail end of the hyperedge, often
      the initiator or cause of the action or the relation.
      @param heads The set of nodes at the head end of the hyperedge, often in
      the receiving end of an effect.
      @param cause_time Timestamp at which the event "happened".
      @param effect_time Timestamp at which the event was "received".
     */
    template <std::ranges::input_range R1, std::ranges::input_range R2>
    requires
      std::convertible_to<std::ranges::range_value_t<R1>, VertT> &&
      std::convertible_to<std::ranges::range_value_t<R2>, VertT>
    directed_delayed_temporal_hyperedge(
        const R1& tails, const R2& heads,
        TimeType cause_time, TimeType effect_time);

    /**
      Create a directed delayed temporal hyperedge from a static projection, a
      cause time and an effect time. The resulting edge would have the same
      endpoints as the projection and the cause and effect times from the
      parameters `cause_time` and `effect_time`.

      @param projection The static projection of this temporal hyperedge.
      @param cause_time The timestamp at which the event "happened".
      @param effect_time The timestamp at which the event was "received".
     */
    directed_delayed_temporal_hyperedge(
        const StaticProjectionType& projection,
        TimeType cause_time, TimeType effect_time);

    /**
      Static hyperedge that encompasses all the non-temporal information about
      this hyperedge.
     */
    [[nodiscard]]
    StaticProjectionType
    static_projection() const;

    /**
      The timestamp that the effect is received by head vertices. For directed
      delayed temporal hyperedges this is equal to the `cause_time` plus delay
      of the hyperedge.
     */
    [[nodiscard]]
    TimeType effect_time() const;

    /**
      The timestamp that the effect is initiated by tail vertex.
     */
    [[nodiscard]]
    TimeType cause_time() const;

    /**
      A directed delayed temporal hyperedge is out_incident to vertex `v` iff
      `v` is in the tail set of vertices of the hyperedge.

      @param vert Vertex to check the out_incident relationship with.
     */
    [[nodiscard]]
    bool is_out_incident(const VertexType& vert) const;

    /**
      A directed delayed temporal hyperedge is in_incident to vertex `v` iff
      `v` is the head set of vertices of the hyperedge.

      @param vert Vertex to check the in_incident relationship with.
     */
    [[nodiscard]]
    bool is_in_incident(const VertexType& vert) const;

    /**
      A directed delayed temporal hyperedge is incident to vertex `v` iff `v` is
      in the head or tail set of vertices of the hyperedge.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    bool is_incident(const VertexType& vert) const;

    /**
      List of all vertices that initiate (cause) the effects of the
      relationship. For directed delayed temporal hyperedges this is equal to
      the tail set of vertices.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const;

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed delayed temporal hyperedges this is equal to
      the head set of vertices.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const;

    /**
      List of all vertices that can initiate (cause) or receive (be affected by)
      the effects of the relationship. For directed hyperedges this is equal to
      the union of tail and head sets of vertices.
     */
    [[nodiscard]]
    std::vector<VertexType> incident_verts() const;

    /**
      The same as `mutated_verts()`.
     */
    [[nodiscard]]
    std::vector<VertexType> tails() const;

    /**
      The same as `mutator_verts()`.
     */
    [[nodiscard]]
    std::vector<VertexType> heads() const;

    /**
      Defines a strong lexicographic ordering where cause times are compare then
      effect times then tail vertices and finally head vertices.
     */
    auto operator<=>(
        const directed_delayed_temporal_hyperedge<
          VertexType, TimeType>&) const = default;

    template <network_vertex VertexType, typename TimeType>
    friend bool effect_lt(
        const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
        const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b);

    /**
      Two directed delayed temporal hyperedges are adjacent if head of the first one
      is the tail of the second one and the cause time of the second one is
      after the effect time of the first. Lack of an adjacency relation between
      hyperedges ususlly mean that an effect transmitted through one hyperedge logically
      cannot be transmitted through the other.
     */
    template <network_vertex VertexType, typename TimeType>
    friend bool adjacent(
        const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
        const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b);

  private:
    TimeType _cause_time, _effect_time;
    std::vector<VertexType> _tails, _heads;

    friend struct
    std::hash<directed_delayed_temporal_hyperedge<VertexType, TimeType>>;

    friend struct
    hll::hash<directed_delayed_temporal_hyperedge<VertexType, TimeType>>;
  };

  /**
    An undirected temporal hyperedges (or hyperevent) indicate instantaneous
    symertic relation or lack of orientation in the relationship between the
    two sets of nodes attributable to a specific point in time.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
    @tparam TimeT Type used for timestamps. Highly recommanded to use a
    numerical type like `size_t`, `int` or `double`.
  */
  template <network_vertex VertT, typename TimeT>
  class undirected_temporal_hyperedge {
  public:
    /**
      Type used for labelling vertices
     */
    using VertexType = VertT;
    /**
      Type used for labelling timestamps
     */
    using TimeType = TimeT;

    /**
      Static projection of the temporal edge.
     */
    using StaticProjectionType = undirected_hyperedge<VertT>;

    undirected_temporal_hyperedge() = default;

    /**
      Create an undirected temporal hyperedge. This variation is specifically
      created so that a brace-enclosed initializer list can be used to
      initialize this class.

      @param verts The set of vertices involved in the hyperedge.
      @param time Timestamp at which the event "happened".
     */
    undirected_temporal_hyperedge(
        std::initializer_list<VertexType> verts, TimeType time);

    /**
      Create an undirected temporal hyperedge.

      @param verts The set of vertices involved in the hyperedge.
      @param time Timestamp at which the event "happened".
     */
    template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, VertT>
    undirected_temporal_hyperedge(const R& verts, TimeType time);

    /**
      Create an undirected temporal hyperedge from a static projection and a
      time.  The resulting edge would have the same endpoints as projection and
      the time from the parameter `time`.

      @param projection The static projection of this temporal hyperedge.
      @param time Timestamp at which the event "happened"
    */
    undirected_temporal_hyperedge(
        const StaticProjectionType& projection, TimeType time);


    /**
      Static hyperedge that encompasses all the non-temporal information about
      this hyperedge.
     */
    [[nodiscard]]
    StaticProjectionType
    static_projection() const;

    /**
      The timestamp that the effect is received by head vertex. For undirected
      temporal hyperedges this is equal to the `cause_time` of the hyperedge.
     */
    [[nodiscard]]
    TimeType effect_time() const;

    /**
      The timestamp that the effect is initiated by tail vertex.
     */
    [[nodiscard]]
    TimeType cause_time() const;

    /**
      An undirected temporal hyperedge is incident to vertex `v` iff `v` is
      in its set of vertices.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    bool is_incident(const VertexType& vert) const;

    /**
      Exactly the same as `is_incident`

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    bool is_in_incident(const VertexType& vert) const;

    /**
      Exactly the same as `is_incident`

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    bool is_out_incident(const VertexType& vert) const;

    /**
      In an undirected temporal hyperedge all involved vertices might act as
      source or cause of an effect.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const;

    /**
      In an undirected hyperedge both involved vertices might act as target of
      an effect.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const;

    /**
      List of all vertices that can initiate (cause) or receive (be affected by)
      the effects of the relationship. For and undirected hyperedges this is
      the same as `mutator_verts()` or `mutated_verts()`.
     */
    [[nodiscard]]
    std::vector<VertexType> incident_verts() const;

    /**
      Defines a strong ordering that would order events based on cause times
      first, then lexographically by the sorted set of verticess.
     */
    auto operator<=>(
        const undirected_temporal_hyperedge<
          VertexType, TimeType>&) const = default;

    /**
      Exactly the same as `operator<`.
     */
    template <network_vertex VertexType, typename TimeType>
    friend bool effect_lt(
        const undirected_temporal_hyperedge<VertexType, TimeType>& a,
        const undirected_temporal_hyperedge<VertexType, TimeType>& b);

    /**
      Two undirected temporal hyperedges are adjacent if they share at least one
      node and the cause time of the second one is after the first. Lack of an
      adjacency relation between hyperedges ususlly mean that an effect
      transmitted through one hyperedge logically cannot be transmitted through
      the other.
     */
    template <network_vertex VertexType, typename TimeType>
    friend bool adjacent(
        const undirected_temporal_hyperedge<VertexType, TimeType>& a,
        const undirected_temporal_hyperedge<VertexType, TimeType>& b);

  private:
    TimeType _time;
    std::vector<VertexType> _verts;

    std::tuple<TimeType, std::vector<VertexType>> comp_tuple() const;

    friend struct std::hash<
      undirected_temporal_hyperedge<VertexType, TimeType>>;
    friend struct hll::hash<
      undirected_temporal_hyperedge<VertexType, TimeType>>;
  };
}  // namespace reticula

#include "../../src/temporal_hyperedges.tpp"

#endif  // INCLUDE_RETICULA_TEMPORAL_HYPEREDGES_HPP_
