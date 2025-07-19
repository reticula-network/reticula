#ifndef INCLUDE_RETICULA_TEMPORAL_EDGES_HPP_
#define INCLUDE_RETICULA_TEMPORAL_EDGES_HPP_

#include <tuple>
#include <span>
#include <array>
#include <algorithm>
#include <compare>

#include <hll/hyperloglog.hpp>

#include "network_concepts.hpp"
#include "static_edges.hpp"

namespace reticula {
  template <network_vertex VertT, class TimeT>
  class undirected_temporal_edge;

  template <network_vertex VertT, class TimeT>
  struct is_instantaneous<undirected_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <network_vertex VertT, class TimeT>
  struct is_undirected<undirected_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <network_vertex VertT, class TimeT>
  struct is_dyadic<undirected_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <network_vertex VertT, class TimeT>
  class directed_temporal_edge;

  template <network_vertex VertT, class TimeT>
  struct is_instantaneous<directed_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <network_vertex VertT, class TimeT>
  struct is_undirected<directed_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <network_vertex VertT, class TimeT>
  struct is_dyadic<directed_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <network_vertex VertT, class TimeT>
  class directed_delayed_temporal_edge;

  template <network_vertex VertT, class TimeT>
  struct is_instantaneous<directed_delayed_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <network_vertex VertT, class TimeT>
  struct is_undirected<directed_delayed_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <network_vertex VertT, class TimeT>
  struct is_dyadic<directed_delayed_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  /**
    A directed temporal edges (or event) indicate instantaneous unsymertic
    relation or orientation between the two nodes attributable to a specific
    point in time. Tail and head vertices allude to a representation of the edge
    as an arrow from one vertex to another.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
    @tparam TimeT Type used for timestamps. Highly recommanded to use a
    numerical type like `size_t`, `int` or `double`.
  */
  template <network_vertex VertT, typename TimeT>
  class directed_temporal_edge {
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
      Static projection of the temporal edge.
     */
    using StaticProjectionType = directed_edge<VertT>;

    directed_temporal_edge() = default;
    /**
      Create a directed temporal edge.

      @param tail The tail end of the edge, often the initiator or cause of the
      action or the relation.
      @param head The head end of the edge, often the receiving end of an
      effect.
      @param time Timestamp at which the event "happened".
     */
    directed_temporal_edge(
        const VertexType& tail, const VertexType& head, TimeType time);
    /**
      Create a directed temporal edge from a static projection and a time. The
      resulting edge would have the same endpoints as projection and the time
      from the parameter `time`.

      @param projection The static projection of this temporal edge.
      @param time Timestamp at which the event "happened"
     */
    directed_temporal_edge(
        const StaticProjectionType& projection, TimeType time);

    /**
      Create a directed temporal edge from mutator and mutated vertex ranges.

      @param tag Uniform construction tag.
      @param mutators Range of mutator vertices (tail vertices).
      @param mutated Range of mutated vertices (head vertices).
      @param cause_time Timestamp at which the event "happened".
      @param effect_time Timestamp at which the event was "received" (must equal cause_time for instantaneous edges).
     */
    template <
      ranges::input_range MutatorRange, ranges::input_range MutatedRange>
    requires
      std::convertible_to<ranges::range_value_t<MutatorRange>, VertexType> &&
      std::convertible_to<ranges::range_value_t<MutatedRange>, VertexType>
    directed_temporal_edge(
        uniform_const_t,
        MutatorRange&& mutators,
        MutatedRange&& mutated,
        TimeType cause_time,
        TimeType effect_time);

    /**
      Static edge that encompasses all the non-temporal information about this
      edge.
     */
    [[nodiscard]]
    StaticProjectionType
    static_projection() const;

    /**
      The timestamp that the effect is received by head vertex. For directed
      temporal edges this is equal to the `cause_time` of the edge.
     */
    [[nodiscard]]
    TimeType effect_time() const;

    /**
      The timestamp that the effect is initiated by tail vertex.
     */
    [[nodiscard]]
    TimeType cause_time() const;

    /**
      A directed temporal edge is out_incident to vertex `v` iff `v` is the
      tail vertex of the edge.

      @param vert Vertex to check the out_incident relationship with.
     */
    [[nodiscard]]
    bool is_out_incident(const VertexType& vert) const;

    /**
      A directed temporal edge is in_incident to vertex `v` iff `v` is the head
      vertex of the edge.

      @param vert Vertex to check the in_incident relationship with.
     */
    [[nodiscard]]
    bool is_in_incident(const VertexType& vert) const;

    /**
      A directed temporal edge is incident to vertex `v` iff `v` is the head or
      tail vertex of the edge.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    bool is_incident(const VertexType& vert) const;

    /**
      List of all vertices that initiate (cause) the effects of the
      relationship. For directed temporal edges this is equal to the tail
      vertex.
     */
    [[nodiscard]]
    std::span<const VertexType, 1> mutator_verts() const;

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed temporal edges this is equal to the head
      vertex.
     */
    [[nodiscard]]
    std::span<const VertexType, 1> mutated_verts() const;

    /**
      List of all vertices that initiate (cause) or receive (affected by) the
      effects of the relationship. For directed edges this is equal to the union
      of results of `mutator_verts()` and `mutated_verts()`.
     */
    [[nodiscard]]
    std::span<const VertexType> incident_verts() const;

    /**
      Returns the single vertex at the tail of the directed link arrow.
     */
    [[nodiscard]]
    VertexType tail() const;

    /**
      Returns the single vertex at the head of the directed link arrow.
     */
    [[nodiscard]]
    VertexType head() const;

    /**
      Two directed temporal edges are adjacent if head of the first one is the
      tail of the second one and the cause time of the second one is after the
      first. Lack of an adjacency relation between edges ususlly mean that an
      effect transmitted through one edge logically cannot be transmitted
      through the other.
     */
    template <network_vertex VertexType, typename TimeType>
    friend bool adjacent(
        const directed_temporal_edge<VertexType, TimeType>& a,
        const directed_temporal_edge<VertexType, TimeType>& b);

    /**
      Defines a strong lexicographic ordering where
      cause times are compare then tail vertices and then head vertices.
     */
#if (_LIBCPP_VERSION)
    bool operator==(
        const directed_temporal_edge<VertexType, TimeType>&)
        const noexcept = default;
    auto operator<=>(
        const directed_temporal_edge<VertexType, TimeType>&) const;
#else
    auto operator<=>(
        const directed_temporal_edge<VertexType, TimeType>&) const = default;
#endif

    /**
      Defines a strong lexicographic ordering along with `operator==` where
      cause times are compared then head vertices and then tail vertices.
     */
    template <network_vertex VertexType, typename TimeType>
    friend bool effect_lt(
        const directed_temporal_edge<VertexType, TimeType>& a,
        const directed_temporal_edge<VertexType, TimeType>& b);

    /**
      Inserts undirected edge formatted as `v1 v2 time` where `v1` and `v2` are
      tail and head vertex respectively and `time` is the cause time.
     */
    template <network_vertex VertexType, typename TimeType>
    friend std::ostream& operator<<(
        std::ostream& os,
        const directed_temporal_edge<VertexType, TimeType>& e);

    /**
      Extracts directed edge from an input stream formatted as `v1 v2 time`
      where `v1` and `v2` are tail and head vertex respectively and time is the
      cause time.
     */
    template <network_vertex VertexType, typename TimeType>
    friend std::istream& operator>>(
        std::istream& is,
        directed_temporal_edge<VertexType, TimeType>& e);

  private:
    TimeType _time;
    // _tail, _head
    std::array<VertexType, 2> _verts;

    friend struct std::hash<directed_temporal_edge<VertexType, TimeType>>;
    friend struct hll::hash<directed_temporal_edge<VertexType, TimeType>>;
  };

  /**
    A directed delayed temporal edges (or event) indicate instantaneous
    unsymertic relation or orientation between the two nodes attributable to a
    specific point in time as cause time and a delay. Tail and head vertices
    allude to a representation of the edge as an arrow from one vertex to
    another. Delay referes to the duration of time between start of the event
    to when the effect is transmitted to the head vertex.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
    @tparam TimeT Type used for timestamps. Highly recommanded to use a
    numerical type like `size_t`, `int` or `double`.
  */
  template <network_vertex VertT, typename TimeT>
  class directed_delayed_temporal_edge {
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
      Static projection of the temporal edge.
     */
    using StaticProjectionType = directed_edge<VertT>;

    directed_delayed_temporal_edge() = default;
    /**
      Create a directed delayed temporal edge.

      You can also think of a directed delayed temporal edge as a "trip" of a
      bus between two consecutive stops in public transportation network or
      travel of a plain between two airports. In this case "the effect" is the
      arrival of passengers, the change is caused by departure of passengers
      from departure stop and the state of the destination stop is mutated by
      this effect.

      Mutation of state is a key abstraction here. Each node is conceptualised
      as a finite state machine. The only thing that can affect a transition (a
      mutation) in the state of a node is either internal to that node or comes
      in form of a temporal edge. In case of the latter the "cause" for that
      mutation would be the node that is at the tail end of the event.

      @param tail The tail end of the edge, often the initiator or cause of the
      action or the relation.
      @param head The head end of the edge, often the receiving end of an
      effect.
      @param cause_time The timestamp at which the event "happened".
      @param effect_time The timestamp at which the event was "received".
     */
    directed_delayed_temporal_edge(
        const VertexType& tail, const VertexType& head,
        TimeType cause_time, TimeType effect_time);
    /**
      Create a directed delayed temporal edge from a static projection, a cause
      time and an effect time. The resulting edge would have the same endpoints
      as the projection and the cause and effect times from the parameters
      `cause_time` and `effect_time`.

      @param projection The static projection of this temporal edge.
      @param cause_time The timestamp at which the event "happened".
      @param effect_time The timestamp at which the event was "received".
     */
    directed_delayed_temporal_edge(
        const StaticProjectionType& projection,
        TimeType cause_time, TimeType effect_time);

    /**
      Create a directed delayed temporal edge from mutator and mutated vertex ranges.

      @param tag Uniform construction tag.
      @param mutators Range of mutator vertices (tail vertices).
      @param mutated Range of mutated vertices (head vertices).
      @param cause_time The timestamp at which the event "happened".
      @param effect_time The timestamp at which the event was "received".
     */
    template <
      ranges::input_range MutatorRange, ranges::input_range MutatedRange>
    requires
      std::convertible_to<ranges::range_value_t<MutatorRange>, VertexType> &&
      std::convertible_to<ranges::range_value_t<MutatedRange>, VertexType>
    directed_delayed_temporal_edge(
        uniform_const_t,
        MutatorRange&& mutators,
        MutatedRange&& mutated,
        TimeType cause_time,
        TimeType effect_time);

    /**
      Static edge that encompasses all the non-temporal information about this
      edge.
     */
    [[nodiscard]]
    StaticProjectionType
    static_projection() const;

    /**
      The timestamp that the effect is received by head vertex. For directed
      delayed temporal edges this is equal to the `cause_time` plus delay of the
      edge.
     */
    [[nodiscard]]
    TimeType effect_time() const;

    /**
      The timestamp that the effect is initiated by tail vertex.
     */
    [[nodiscard]]
    TimeType cause_time() const;

    /**
      A directed delayed temporal edge is out_incident to vertex `v` iff `v` is
      the tail vertex of the edge.

      @param vert Vertex to check the out_incident relationship with.
     */
    [[nodiscard]]
    bool is_out_incident(const VertexType& vert) const;

    /**
      A directed delayed temporal edge is in_incident to vertex `v` iff `v` is
      the head vertex of the edge.

      @param vert Vertex to check the in_incident relationship with.
     */
    [[nodiscard]]
    bool is_in_incident(const VertexType& vert) const;

    /**
      A directed delayed temporal edge is incident to vertex `v` iff `v` is the
      head or tail vertex of the edge.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    bool is_incident(const VertexType& vert) const;

    /**
      List of all vertices that initiate (cause) the effects of the
      relationship. For directed delayed temporal edges this is equal to the
      tail vertex.
     */
    [[nodiscard]]
    std::span<const VertexType, 1> mutator_verts() const;

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed delayed temporal edges this is equal to the
      head vertex.
     */
    [[nodiscard]]
    std::span<const VertexType, 1> mutated_verts() const;

    /**
      List of all vertices that initiate (cause) or receive (affected by) the
      effects of the relationship. For directed edges this is equal to the union
      of results of `mutator_verts()` and `mutated_verts()`.
     */
    [[nodiscard]]
    std::span<const VertexType> incident_verts() const;

    /**
      Returns the single vertex at the tail of the directed link arrow.
     */
    [[nodiscard]]
    VertexType tail() const;

    /**
      Returns the single vertex at the head of the directed link arrow.
     */
    [[nodiscard]]
    VertexType head() const;

    /**
      Two directed delayed temporal edges are adjacent if head of the first one
      is the tail of the second one and the cause time of the second one is
      after the effect time of the first. Lack of an adjacency relation between
      edges ususlly mean that an effect transmitted through one edge logically
      cannot be transmitted through the other.
     */
    template <network_vertex VertexType, typename TimeType>
    friend bool adjacent(
        const directed_delayed_temporal_edge<VertexType, TimeType>& a,
        const directed_delayed_temporal_edge<VertexType, TimeType>& b);

    /**
      Defines a strong lexicographic ordering where cause times are compare
      then effect times then tail vertices and finally head vertices.
     */
#if (_LIBCPP_VERSION)
    bool operator==(
        const directed_delayed_temporal_edge<
          VertexType, TimeType>&) const noexcept = default;
    auto operator<=>(
        const directed_delayed_temporal_edge<
          VertexType, TimeType>&) const;
#else
    auto operator<=>(
        const directed_delayed_temporal_edge<
          VertexType, TimeType>&) const = default;
#endif

    template <network_vertex VertexType, typename TimeType>
    friend bool effect_lt(
        const directed_delayed_temporal_edge<VertexType, TimeType>& a,
        const directed_delayed_temporal_edge<VertexType, TimeType>& b);

    template <network_vertex VertexType, typename TimeType>
    friend std::ostream& operator<<(
        std::ostream& os,
        const directed_delayed_temporal_edge<VertexType, TimeType>& e);

    template <network_vertex VertexType, typename TimeType>
    friend std::istream& operator>>(
        std::istream& is,
        directed_delayed_temporal_edge<VertexType, TimeType>& e);

  private:
    TimeType _cause_time, _effect_time;
    // _tail, _head
    std::array<VertexType, 2> _verts;

    friend struct
    std::hash<directed_delayed_temporal_edge<VertexType, TimeType>>;

    friend struct
    hll::hash<directed_delayed_temporal_edge<VertexType, TimeType>>;
  };

  /**
    An undirected temporal edges (or event) indicate instantaneous symertic
    relation or lack of orientation in the relationship between the two nodes
    attributable to a specific point in time.

    @tparam VertT Type used for distinguishing vertices. Recommanded to use
    integral types like `int`, `size_t` or `uint8_t`.
    @tparam TimeT Type used for timestamps. Highly recommanded to use a
    numerical type like `size_t`, `int` or `double`.
  */
  template <network_vertex VertT, typename TimeT>
  class undirected_temporal_edge {
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
    using StaticProjectionType = undirected_edge<VertT>;

    undirected_temporal_edge() = default;
    /**
      Create an undirected temporal edge.

      @param v1 First incident vertex of the edge.
      @param v2 Second incident vertex of the edge.
      @param time Timestamp at which the event "happened".
    */
    undirected_temporal_edge(
        const VertexType& v1, const VertexType& v2, TimeType time);
    /**
      Create an undirected temporal edge from a static projection and a time.
      The resulting edge would have the same endpoints as projection and the
      time from the parameter `time`.

      @param projection The static projection of this temporal edge.
      @param time Timestamp at which the event "happened"
    */
    undirected_temporal_edge(
        const StaticProjectionType& projection, TimeType time);

    /**
      Create an undirected temporal edge from mutator and mutated vertex ranges.

      @param tag Uniform construction tag.
      @param mutators Range of mutator vertices.
      @param mutated Range of mutated vertices.
      @param cause_time Timestamp at which the event "happened".
      @param effect_time Timestamp at which the event was "received" (must equal cause_time for instantaneous edges).
     */
    template <
      ranges::input_range MutatorRange, ranges::input_range MutatedRange>
    requires
      std::convertible_to<ranges::range_value_t<MutatorRange>, VertexType> &&
      std::convertible_to<ranges::range_value_t<MutatedRange>, VertexType>
    undirected_temporal_edge(
        uniform_const_t,
        MutatorRange&& mutators,
        MutatedRange&& mutated,
        TimeType cause_time,
        TimeType effect_time);

    /**
      Static edge that encompasses all the non-temporal information about this
      edge.
     */
    [[nodiscard]]
    StaticProjectionType
    static_projection() const;

    /**
      The timestamp that the effect is received by head vertex. For undirected
      temporal edges this is equal to the `cause_time` of the edge.
     */
    [[nodiscard]]
    TimeType effect_time() const;

    /**
      The timestamp that the effect is initiated by tail vertex.
     */
    [[nodiscard]]
    TimeType cause_time() const;

    /**
      An undirected temporal edge is incident to vertex `v` iff `v` is either of
      its vertices.

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
      In an undirected temporal edge both vertices might act as source or cause
      of an effect.
     */
    [[nodiscard]]
    std::span<const VertexType> mutator_verts() const;

    /**
      In an undirected temporal edge both vertices might act as target of an
      effect.
     */
    [[nodiscard]]
    std::span<const VertexType> mutated_verts() const;

    /**
      In an undirected temporal edge both vertices are incident to the edge.
     */
    [[nodiscard]]
    std::span<const VertexType> incident_verts() const;

    /**
      Defines a strong ordering along with that would rank events
      based on cause times first.
     */
#if (_LIBCPP_VERSION)
    bool operator==(
        const undirected_temporal_edge<VertexType, TimeType>&)
        const noexcept = default;
    auto operator<=>(
        const undirected_temporal_edge<VertexType, TimeType>&) const;
#else
    auto operator<=>(
        const undirected_temporal_edge<VertexType, TimeType>&) const = default;
#endif

    /**
      Exactly the same as `operator<`.
     */
    template <network_vertex VertexType, typename TimeType>
    friend bool effect_lt(
        const undirected_temporal_edge<VertexType, TimeType>& a,
        const undirected_temporal_edge<VertexType, TimeType>& b);


    /**
      Two undirected temporal edges are adjacent if they share at least on node
      and the cause time of the second one is after the first. Lack of an
      adjacency relation between edges ususlly mean that an effect transmitted
      through one edge logically cannot be transmitted through the other.
     */
    template <network_vertex VertexType, typename TimeType>
    friend bool adjacent(
        const undirected_temporal_edge<VertexType, TimeType>& a,
        const undirected_temporal_edge<VertexType, TimeType>& b);

    /**
      Inserts undirected temporal edge formatted as `v1 v2 time` where `v1` and
      `v2` are vertex in the same order as initialiser and `time` is the cause
      time.
     */
    template <network_vertex VertexType, typename TimeType>
    requires output_streamable<VertexType> && output_streamable<TimeType>
    friend std::ostream& operator<<(
        std::ostream& os,
        const undirected_temporal_edge<VertexType, TimeType>& e);

    /**
      Extracts undirected temporal edge from an input stream formatted as
      `v1 v2 time`.
     */
    template <network_vertex VertexType, typename TimeType>
    requires input_streamable<VertexType> && input_streamable<TimeType>
    friend std::istream& operator>>(
        std::istream& is,
        undirected_temporal_edge<VertexType, TimeType>& e);

  private:
    TimeType _time;
    std::array<VertexType, 2> _verts;

    friend struct std::hash<undirected_temporal_edge<VertexType, TimeType>>;
    friend struct hll::hash<undirected_temporal_edge<VertexType, TimeType>>;
  };
}  // namespace reticula

// Implementation
#include <functional>
#include <istream>
#include <ostream>

#include "utils.hpp"
#include "network_concepts.hpp"

namespace std {
  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::undirected_temporal_edge<VertexType, TimeType>> {
    std::size_t
    operator()(
        const reticula::undirected_temporal_edge<
          VertexType, TimeType>& e) const {
      return reticula::utils::combine_hash<TimeType, reticula::hash>(
          reticula::utils::combine_hash<VertexType, reticula::hash>(
            reticula::hash<VertexType>{}(e._verts[0]), e._verts[1]), e._time);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_temporal_edge<VertexType, TimeType>> {
    std::size_t
    operator()(
        const reticula::directed_temporal_edge<
          VertexType, TimeType>& e) const {
      return reticula::utils::combine_hash<TimeType, reticula::hash>(
          reticula::utils::combine_hash<VertexType, reticula::hash>(
            reticula::hash<VertexType>{}(e._verts[0]), e._verts[1]), e._time);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_delayed_temporal_edge<VertexType, TimeType>> {
    std::size_t
    operator()(
        const reticula::directed_delayed_temporal_edge<
          VertexType, TimeType>& e) const {
      return reticula::utils::combine_hash<TimeType, reticula::hash>(
              reticula::utils::combine_hash<TimeType, reticula::hash>(
                reticula::utils::combine_hash<VertexType, reticula::hash>(
                  reticula::hash<VertexType>{}(e._verts[0]), e._verts[1]),
                e._cause_time),
              e._effect_time);
    }
  };
}  // namespace std


namespace hll {
  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::undirected_temporal_edge<VertexType, TimeType>> {
    std::size_t
    operator()(
        const reticula::undirected_temporal_edge<VertexType, TimeType>& e,
        std::uint64_t seed) const {
      return hll::hash<std::size_t>{}(
          std::hash<reticula::undirected_temporal_edge<
            VertexType, TimeType>>{}(e), seed);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_temporal_edge<VertexType, TimeType>> {
    std::uint64_t
    operator()(
        const reticula::directed_temporal_edge<VertexType, TimeType>& e,
        std::uint64_t seed) const {
      return hll::hash<std::size_t>{}(
          std::hash<reticula::directed_temporal_edge<
            VertexType, TimeType>>{}(e), seed);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_delayed_temporal_edge<VertexType, TimeType>> {
    std::uint64_t
    operator()(
        const reticula::directed_delayed_temporal_edge<VertexType, TimeType>& e,
        std::uint64_t seed) const {
      return hll::hash<std::size_t>{}(
          std::hash<
            reticula::directed_delayed_temporal_edge<
              VertexType, TimeType>>{}(e), seed);
    }
  };
}  // namespace hll

namespace reticula {
  // properties of directed temporal edge:

  template <network_vertex VertexType, typename TimeType>
  directed_temporal_edge<VertexType, TimeType>::directed_temporal_edge(
    const VertexType& tail, const VertexType& head, TimeType time)
    : _time(time), _verts({tail, head}) {}

  template <network_vertex VertexType, typename TimeType>
  directed_temporal_edge<VertexType, TimeType>::directed_temporal_edge(
    const directed_edge<VertexType>& projection, TimeType time)
    : _time(time), _verts({projection.tail(), projection.head()}) {}

  template <network_vertex VertexType, typename TimeType>
  template <ranges::input_range MutatorRange, ranges::input_range MutatedRange>
  requires
    std::convertible_to<ranges::range_value_t<MutatorRange>, VertexType> &&
    std::convertible_to<ranges::range_value_t<MutatedRange>, VertexType>
  directed_temporal_edge<VertexType, TimeType>::directed_temporal_edge(
      uniform_const_t,
      MutatorRange&& mutators,
      MutatedRange&& mutated,
      TimeType cause_time,
      TimeType effect_time) : _time(cause_time) {
    if (cause_time != effect_time)
      throw std::invalid_argument(
        "unequal cause_time and effect_time");

    std::vector<VertexType> mutator_verts(
      ranges::begin(mutators), ranges::end(mutators));
    std::vector<VertexType> mutated_verts(
      ranges::begin(mutated), ranges::end(mutated));

    if (mutator_verts.empty() || mutated_verts.empty())
      throw std::invalid_argument(
        "directed_temporal_edge requires non-empty mutator and mutated ranges");

    if (mutator_verts.size() != 1 || mutated_verts.size() != 1)
      throw std::invalid_argument(
        "exactly one element must be provided in each range");

    _verts[0] = mutator_verts[0];
    _verts[1] = mutated_verts[0];
  }

  template <network_vertex VertexType, typename TimeType>
  directed_edge<VertexType>
  directed_temporal_edge<VertexType, TimeType>::static_projection() const {
    return directed_edge<VertexType>(_verts[0], _verts[1]);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_temporal_edge<VertexType, TimeType>::effect_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_temporal_edge<VertexType, TimeType>::cause_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_edge<VertexType, TimeType>::is_out_incident(
      const VertexType& vert) const {
    return (_verts[0] == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_edge<VertexType, TimeType>::is_in_incident(
      const VertexType& vert) const {
    return (_verts[1] == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_edge<VertexType, TimeType>::is_incident(
      const VertexType& vert) const {
    return (_verts[0] == vert || _verts[1] == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType, 1>
  directed_temporal_edge<VertexType, TimeType>::mutator_verts() const {
    return std::span<const VertexType, 1>(&_verts[0], 1);
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType, 1>
  directed_temporal_edge<VertexType, TimeType>::mutated_verts() const {
    return std::span<const VertexType, 1>(&_verts[1], 1);
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  directed_temporal_edge<VertexType, TimeType>::incident_verts() const {
    std::size_t n = (_verts[0] == _verts[1]) ? 1 : 2;
    return {_verts.data(), n};
  }

  template <network_vertex VertexType, typename TimeType>
  VertexType directed_temporal_edge<VertexType, TimeType>::tail() const {
    return _verts[0];
  }

  template <network_vertex VertexType, typename TimeType>
  VertexType directed_temporal_edge<VertexType, TimeType>::head() const {
    return _verts[1];
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType, typename TimeType>
  auto directed_temporal_edge<VertexType, TimeType>::operator<=>(
      const directed_temporal_edge<VertexType, TimeType>& o) const {
    return utils::compare(
      std::tie(_time, _verts[1], _verts[0]),
      std::tie(o._time, o._verts[1], o._verts[0]));
  }
#endif

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_temporal_edge<VertexType, TimeType>& a,
      const directed_temporal_edge<VertexType, TimeType>& b) {
    return ((b._time > a._time) && (a._verts[1] == b._verts[0]));
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_temporal_edge<VertexType, TimeType>& a,
      const directed_temporal_edge<VertexType, TimeType>& b) {
    return std::make_tuple(a._time, a._verts[1], a._verts[0]) <
      std::make_tuple(b._time, b._verts[1], b._verts[0]);
  }

  template <network_vertex VertexType, typename TimeType>
  std::ostream& operator<<(std::ostream& os,
      const directed_temporal_edge<VertexType, TimeType>& e) {
    return os << e._verts[0] << " " << e._verts[1] << " " << e._time;
  }

  template <network_vertex VertexType, typename TimeType>
  std::istream& operator>>(
      std::istream& is,
      directed_temporal_edge<VertexType, TimeType>& e) {
    return is >> e._verts[0] >> e._verts[1] >> e._time;
  }

  // properties of directed delayed temporal edges:

  template <network_vertex VertexType, typename TimeType>
  directed_delayed_temporal_edge<VertexType, TimeType>::
    directed_delayed_temporal_edge(
      const VertexType& tail, const VertexType& head,
      TimeType cause_time, TimeType effect_time) :
      _cause_time(cause_time), _effect_time(effect_time),
      _verts({tail, head}) {
    if (_effect_time < _cause_time)
      throw std::invalid_argument("directed_delayed_temporal_edge cannot have a"
          " cause_time larger than effect_time");
  }

  template <network_vertex VertexType, typename TimeType>
  directed_delayed_temporal_edge<VertexType, TimeType>::
    directed_delayed_temporal_edge(
      const directed_edge<VertexType>& projection,
      TimeType cause_time, TimeType effect_time) :
      _cause_time(cause_time), _effect_time(effect_time),
      _verts({projection.tail(), projection.head()}) {
    if (_effect_time < _cause_time)
      throw std::invalid_argument("directed_delayed_temporal_edge cannot have a"
          " cause_time larger than effect_time");
  }

  template <network_vertex VertexType, typename TimeType>
  template <ranges::input_range MutatorRange, ranges::input_range MutatedRange>
  requires
    std::convertible_to<ranges::range_value_t<MutatorRange>, VertexType> &&
    std::convertible_to<ranges::range_value_t<MutatedRange>, VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::
    directed_delayed_temporal_edge(
      uniform_const_t,
      MutatorRange&& mutators,
      MutatedRange&& mutated,
      TimeType cause_time,
      TimeType effect_time) :
      _cause_time(cause_time), _effect_time(effect_time) {
    if (_effect_time < _cause_time)
      throw std::invalid_argument("directed_delayed_temporal_edge cannot have a"
        " cause_time larger than effect_time");

    std::vector<VertexType> mutator_verts(
      ranges::begin(mutators), ranges::end(mutators));
    std::vector<VertexType> mutated_verts(
      ranges::begin(mutated), ranges::end(mutated));

    if (mutator_verts.empty() || mutated_verts.empty())
      throw std::invalid_argument(
        "directed_delayed_temporal_edge requires non-empty mutator and mutated "
        "ranges");

    if (ranges::equal(mutator_verts, mutated_verts)) {
      if (mutator_verts.size() == 2) {
        _verts[0] = mutator_verts[0];
        _verts[1] = mutator_verts[1];
        return;
      } else if (mutator_verts.size() != 1) {
        throw std::invalid_argument(
          "directed_delayed_temporal_edge requires exactly one or two elements "
          "when ranges are equal");
      }
    }

    if (mutator_verts.size() != 1 || mutated_verts.size() != 1)
      throw std::invalid_argument(
        "directed_delayed_temporal_edge requires exactly one element in each "
        "range when ranges are different");

    _verts[0] = mutator_verts[0];
    _verts[1] = mutated_verts[0];
  }

  template <network_vertex VertexType, typename TimeType>
  directed_edge<VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::
      static_projection() const {
    return directed_edge<VertexType>(_verts[0], _verts[1]);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_edge<VertexType, TimeType>::effect_time() const {
    return _effect_time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_edge<VertexType, TimeType>::cause_time() const {
    return _cause_time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_edge<VertexType, TimeType>::is_out_incident(
      const VertexType& vert) const {
    return (_verts[0] == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_edge<VertexType, TimeType>::is_in_incident(
      const VertexType& vert) const {
    return (_verts[1] == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_edge<VertexType, TimeType>::is_incident(
      const VertexType& vert) const {
    return (_verts[0] == vert || _verts[1] == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType, 1>
  directed_delayed_temporal_edge<VertexType, TimeType>::mutator_verts() const {
    return std::span<const VertexType, 1>(&_verts[0], 1);
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType, 1>
  directed_delayed_temporal_edge<VertexType, TimeType>::mutated_verts() const {
    return std::span<const VertexType, 1>(&_verts[1], 1);
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::incident_verts() const {
    std::size_t n = (_verts[0] == _verts[1]) ? 1 : 2;
    return {_verts.data(), n};
  }

  template <network_vertex VertexType, typename TimeType>
  VertexType
  directed_delayed_temporal_edge<VertexType, TimeType>::tail() const {
    return _verts[0];
  }

  template <network_vertex VertexType, typename TimeType>
  VertexType
  directed_delayed_temporal_edge<VertexType, TimeType>::head() const {
    return _verts[1];
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType, typename TimeType>
  auto directed_delayed_temporal_edge<VertexType, TimeType>::operator<=>(
      const directed_delayed_temporal_edge<VertexType, TimeType>& o) const {
    return utils::compare(
      std::tie(_cause_time, _effect_time, _verts[1], _verts[0]),
      std::tie(o._cause_time, o._effect_time, o._verts[1], o._verts[0]));
  }
#endif

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_delayed_temporal_edge<VertexType, TimeType>& a,
      const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
    return (b._cause_time > a._effect_time) && (a._verts[1] == b._verts[0]);
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_delayed_temporal_edge<VertexType, TimeType>& a,
      const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
    return std::make_tuple(
        a._effect_time, a._cause_time, a._verts[1], a._verts[0]) <
      std::make_tuple(
        b._effect_time, b._cause_time, b._verts[1], b._verts[0]);
  }

  template <network_vertex VertexType, typename TimeType>
  std::ostream& operator<<(std::ostream& os,
      const directed_delayed_temporal_edge<VertexType, TimeType>& e) {
    return os << e._verts[0] << " " << e._verts[1] << " "
      << e._cause_time << " " << e._effect_time;
  }

  template <network_vertex VertexType, typename TimeType>
  std::istream& operator>>(
      std::istream& is,
      directed_delayed_temporal_edge<VertexType, TimeType>& e) {
    return is >> e._verts[0] >> e._verts[1] >> e._cause_time >> e._effect_time;
  }


  // properties of undirected temporal edges:

  template <network_vertex VertexType, typename TimeType>
  undirected_temporal_edge<VertexType, TimeType>::undirected_temporal_edge(
      const VertexType& v1, const VertexType& v2,
      TimeType time) : _time(time) {
    auto [l, h] = std::minmax(v1, v2);
    _verts = {l, h};
  }

  template <network_vertex VertexType, typename TimeType>
  undirected_temporal_edge<VertexType, TimeType>::undirected_temporal_edge(
      const undirected_edge<VertexType>& projection,
      TimeType time) : _time(time) {
    auto verts = projection.incident_verts();

    _verts[0] = verts[0];
    if (verts.size() > 1)
      _verts[1] = verts[1];
    else
      _verts[1] = verts[0];
  }

  template <network_vertex VertexType, typename TimeType>
  template <ranges::input_range MutatorRange, ranges::input_range MutatedRange>
  requires
    std::convertible_to<ranges::range_value_t<MutatorRange>, VertexType> &&
    std::convertible_to<ranges::range_value_t<MutatedRange>, VertexType>
  undirected_temporal_edge<VertexType, TimeType>::undirected_temporal_edge(
      uniform_const_t,
      MutatorRange&& mutators,
      MutatedRange&& mutated,
      TimeType cause_time,
      TimeType effect_time) : _time(cause_time) {
    if (cause_time != effect_time)
      throw std::invalid_argument("unequal cause_time and effect_time");

    std::vector<VertexType> mutator_verts(
      ranges::begin(mutators), ranges::end(mutators));
    std::vector<VertexType> mutated_verts(
      ranges::begin(mutated), ranges::end(mutated));

    if (mutator_verts.empty() || mutated_verts.empty())
      throw std::invalid_argument(
        "undirected_temporal_edge requires non-empty mutator and mutated "
        "ranges");

    if (!ranges::equal(mutator_verts, mutated_verts))
      throw std::invalid_argument(
        "undirected_temporal_edge requires mutator and mutated ranges to be "
        "equal");

    if (mutator_verts.size() == 1) {
      _verts = {mutator_verts[0], mutator_verts[0]};
    } else if (mutator_verts.size() == 2) {
      auto [l, h] = std::minmax(mutator_verts[0], mutator_verts[1]);
      _verts = {l, h};
    } else {
      throw std::invalid_argument(
        "undirected_temporal_edge requires exactly 1 or 2 vertices");
    }
  }

  template <network_vertex VertexType, typename TimeType>
  undirected_edge<VertexType>
  undirected_temporal_edge<VertexType, TimeType>::static_projection() const {
    return undirected_edge<VertexType>(_verts[0], _verts[1]);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType undirected_temporal_edge<VertexType, TimeType>::effect_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType undirected_temporal_edge<VertexType, TimeType>::cause_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_edge<VertexType, TimeType>::is_incident(
      const VertexType& vert) const {
    return (_verts[0] == vert || _verts[1] == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_edge<VertexType, TimeType>::is_in_incident(
      const VertexType& vert) const {
    return (_verts[0] == vert || _verts[1] == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_edge<VertexType, TimeType>::is_out_incident(
      const VertexType& vert) const {
    return (_verts[0] == vert || _verts[1] == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  undirected_temporal_edge<VertexType, TimeType>::incident_verts() const {
    std::size_t n = (_verts[0] == _verts[1]) ? 1 : 2;
    return {_verts.data(), n};
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  undirected_temporal_edge<VertexType, TimeType>::mutator_verts() const {
    return incident_verts();
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  undirected_temporal_edge<VertexType, TimeType>::mutated_verts() const {
    return incident_verts();
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType, typename TimeType>
  auto undirected_temporal_edge<VertexType, TimeType>::operator<=>(
      const undirected_temporal_edge<VertexType, TimeType>& o) const {
    return utils::compare(
      std::tie(_time, _verts[0], _verts[1]),
      std::tie(o._time, o._verts[0], o._verts[1]));
  }
#endif

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const undirected_temporal_edge<VertexType, TimeType>& a,
      const undirected_temporal_edge<VertexType, TimeType>& b) {
    return ((b._time > a._time) &&
              (a._verts[0] == b._verts[0] ||
                a._verts[0] == b._verts[1] ||
                a._verts[1] == b._verts[0] ||
                a._verts[1] == b._verts[1]));
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const undirected_temporal_edge<VertexType, TimeType>& a,
      const undirected_temporal_edge<VertexType, TimeType>& b) {
    return a < b;
  }

  template <network_vertex VertexType, typename TimeType>
  requires output_streamable<VertexType> && output_streamable<TimeType>
  std::ostream& operator<<(
      std::ostream& os,
      const undirected_temporal_edge<VertexType, TimeType>& e) {
    return os << e._verts[0] << " " << e._verts[1] << " " << e._time;
  }

  template <network_vertex VertexType, typename TimeType>
  requires input_streamable<VertexType> && input_streamable<TimeType>
  std::istream& operator>>(
      std::istream& is,
      undirected_temporal_edge<VertexType, TimeType>& e) {
    return is >> e._verts[0] >> e._verts[1] >> e._time;
  }
}  // namespace reticula


#endif  // INCLUDE_RETICULA_TEMPORAL_EDGES_HPP_
