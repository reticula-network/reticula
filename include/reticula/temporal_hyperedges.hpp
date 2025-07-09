#ifndef INCLUDE_RETICULA_TEMPORAL_HYPEREDGES_HPP_
#define INCLUDE_RETICULA_TEMPORAL_HYPEREDGES_HPP_

#include <tuple>
#include <vector>
#include <span>
#include <algorithm>
#include <compare>
#include <initializer_list>

#include <hll/hyperloglog.hpp>

#include "ranges.hpp"
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
    template <ranges::input_range R1, ranges::input_range R2>
    requires
      std::convertible_to<ranges::range_value_t<R1>, VertT> &&
      std::convertible_to<ranges::range_value_t<R2>, VertT>
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
    std::span<const VertexType> mutator_verts() const;

    /**
      List of all vertices that can receive (affected by) the effects of the
      relationship. For directed temporal hyperedges this is equal to the head
      set of vertices.
     */
    [[nodiscard]]
    std::span<const VertexType> mutated_verts() const;

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
    std::span<const VertexType> tails() const;

    /**
      The same as `mutator_verts()`.
     */
    [[nodiscard]]
    std::span<const VertexType> heads() const;

    /**
      Defines a strong lexicographic ordering where cause times are compare then
      tail set and then head set.
     */
#if (_LIBCPP_VERSION)
    bool operator==(const directed_temporal_hyperedge<
      VertexType, TimeType>&) const noexcept = default;
    auto operator<=>(const directed_temporal_hyperedge<
      VertexType, TimeType>& o) const;
#else
    auto operator<=>(const directed_temporal_hyperedge<
          VertexType, TimeType>&) const = default;
#endif

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
    template <ranges::input_range R1, ranges::input_range R2>
    requires
      std::convertible_to<ranges::range_value_t<R1>, VertT> &&
      std::convertible_to<ranges::range_value_t<R2>, VertT>
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
    std::span<const VertexType> mutator_verts() const;

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed delayed temporal hyperedges this is equal to
      the head set of vertices.
     */
    [[nodiscard]]
    std::span<const VertexType> mutated_verts() const;

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
    std::span<const VertexType> tails() const;

    /**
      The same as `mutator_verts()`.
     */
    [[nodiscard]]
    std::span<const VertexType> heads() const;

    /**
      Defines a strong lexicographic ordering where cause times are compare then
      effect times then tail vertices and finally head vertices.
     */
#if (_LIBCPP_VERSION)
    bool operator==(const directed_delayed_temporal_hyperedge<
      VertexType, TimeType>&) const noexcept = default;
    auto operator<=>(const directed_delayed_temporal_hyperedge<
      VertexType, TimeType>& o) const;
#else
    auto operator<=>(const directed_delayed_temporal_hyperedge<
          VertexType, TimeType>&) const = default;
#endif

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
    template <ranges::input_range R>
    requires std::convertible_to<ranges::range_value_t<R>, VertT>
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
    std::span<const VertexType> incident_verts() const;

    /**
      Defines a strong ordering that would order events based on cause times
      first, then lexographically by the sorted set of verticess.
     */
#if (_LIBCPP_VERSION)
    bool operator==(const undirected_temporal_hyperedge<
      VertexType, TimeType>&) const noexcept = default;
    auto operator<=>(const undirected_temporal_hyperedge<
      VertexType, TimeType>& o) const;
#else
    auto operator<=>(const undirected_temporal_hyperedge<
          VertexType, TimeType>&) const = default;
#endif

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

// Implementation
#include <functional>
#include <algorithm>

#include "utils.hpp"

namespace std {
  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::undirected_temporal_hyperedge<VertexType, TimeType>> {
    std::size_t
    operator()(
        const reticula::undirected_temporal_hyperedge<
          VertexType, TimeType>& e) const {
      std::size_t verts_hash = std::accumulate(
          e._verts.begin(), e._verts.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
          return reticula::utils::combine_hash<
            VertexType, reticula::hash>(h, v);});
      return reticula::utils::combine_hash<
        TimeType, reticula::hash>(verts_hash, e._time);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_temporal_hyperedge<VertexType, TimeType>> {
    std::size_t
    operator()(
        const reticula::directed_temporal_hyperedge<
          VertexType, TimeType>& e) const {
      std::size_t heads_hash = std::accumulate(
          e._heads.begin(), e._heads.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
      std::size_t tails_hash = std::accumulate(
          e._tails.begin(), e._tails.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
      return reticula::utils::combine_hash<TimeType, reticula::hash>(
          reticula::utils::combine_hash<std::size_t, reticula::hash>(
            heads_hash, tails_hash), e._time);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_delayed_temporal_hyperedge<
      VertexType, TimeType>> {
    std::size_t
    operator()(
        const
        reticula::directed_delayed_temporal_hyperedge<
          VertexType, TimeType>& e) const {
      std::size_t heads_hash = std::accumulate(
          e._heads.begin(), e._heads.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
      std::size_t tails_hash = std::accumulate(
          e._tails.begin(), e._tails.end(), std::size_t{},
          [](std::size_t h, const VertexType& v) {
            return reticula::utils::combine_hash<
              VertexType, reticula::hash>(h, v);});
      return reticula::utils::combine_hash<TimeType, reticula::hash>(
              reticula::utils::combine_hash<TimeType, reticula::hash>(
                reticula::utils::combine_hash<std::size_t, reticula::hash>(
                  heads_hash, tails_hash),
                e._cause_time),
              e._effect_time);
    }
  };
}  // namespace std


namespace hll {
  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::undirected_temporal_hyperedge<VertexType, TimeType>> {
    std::size_t
    operator()(
        const reticula::undirected_temporal_hyperedge<VertexType, TimeType>& e,
        std::uint64_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<reticula::undirected_temporal_hyperedge<
            VertexType, TimeType>>{}(e), seed);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_temporal_hyperedge<VertexType, TimeType>> {
    uint64_t
    operator()(
        const reticula::directed_temporal_hyperedge<VertexType, TimeType>& e,
        std::uint64_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<reticula::directed_temporal_hyperedge<
            VertexType, TimeType>>{}(e), seed);
    }
  };

  template<reticula::network_vertex VertexType, typename TimeType>
  struct hash<reticula::directed_delayed_temporal_hyperedge<
      VertexType, TimeType>> {
    uint64_t
    operator()(
        const reticula::directed_delayed_temporal_hyperedge<
          VertexType, TimeType>& e,
        std::uint64_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<reticula::directed_delayed_temporal_hyperedge<
              VertexType, TimeType>>{}(e), seed);
    }
  };
}  // namespace hll

namespace reticula {
  // properties of directed temporal edge:

  template <network_vertex VertexType, typename TimeType>
  directed_temporal_hyperedge<VertexType, TimeType>::
    directed_temporal_hyperedge(
      std::initializer_list<VertexType> tails,
      std::initializer_list<VertexType> heads,
      TimeType time) :
    directed_temporal_hyperedge(
        std::vector<VertexType>(tails),
        std::vector<VertexType>(heads), time) {}

  template <network_vertex VertexType, typename TimeType>
  template <ranges::input_range R1, ranges::input_range R2>
  requires
    std::convertible_to<ranges::range_value_t<R1>, VertexType> &&
    std::convertible_to<ranges::range_value_t<R2>, VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::
    directed_temporal_hyperedge(
      const R1& tails, const R2& heads, TimeType time) : _time(time) {
    if constexpr (ranges::sized_range<R1>)
      _heads.reserve(ranges::size(heads));
    ranges::copy(heads, std::back_inserter(_heads));

    if constexpr (ranges::sized_range<R2>)
      _tails.reserve(ranges::size(tails));
    ranges::copy(tails, std::back_inserter(_tails));

    ranges::sort(_heads);
    auto [hfirst, hlast] = ranges::unique(_heads);
    _heads.erase(hfirst, hlast);
    _heads.shrink_to_fit();

    ranges::sort(_tails);
    auto [tfirst, tlast] = ranges::unique(_tails);
    _tails.erase(tfirst, tlast);
    _tails.shrink_to_fit();
  }

  template <network_vertex VertexType, typename TimeType>
  directed_temporal_hyperedge<VertexType, TimeType>::
  directed_temporal_hyperedge(
    const directed_hyperedge<VertexType>& projection, TimeType time)
    : _time(time), _tails(projection.tails()), _heads(projection.heads()) {}

  template <network_vertex VertexType, typename TimeType>
  directed_hyperedge<VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::static_projection() const {
    return directed_hyperedge<VertexType>(_tails, _heads);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_temporal_hyperedge<VertexType, TimeType>::effect_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_temporal_hyperedge<VertexType, TimeType>::cause_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_hyperedge<VertexType, TimeType>::is_out_incident(
      const VertexType& vert) const {
    return std::binary_search(_tails.begin(), _tails.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_hyperedge<VertexType, TimeType>::is_in_incident(
      const VertexType& vert) const {
    return std::binary_search(_heads.begin(), _heads.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_hyperedge<VertexType, TimeType>::is_incident(
      const VertexType& vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::mutator_verts() const {
    return _tails;
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::mutated_verts() const {
    return _heads;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::incident_verts() const {
    std::vector<VertexType> res;
    res.reserve(_heads.size() + _tails.size());
    ranges::set_union(_tails, _heads, std::back_inserter(res));
    return res;
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::tails() const {
    return _tails;
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  directed_temporal_hyperedge<VertexType, TimeType>::heads() const {
    return _heads;
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType, typename TimeType>
  auto directed_temporal_hyperedge<VertexType, TimeType>::operator<=>(
      const directed_temporal_hyperedge<VertexType, TimeType>& o) const {
    return utils::compare(std::tie(_time, _tails, _heads),
                          std::tie(o._time, o._tails, o._heads));
  }
#endif


  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_temporal_hyperedge<VertexType, TimeType>& b) {
    return a < b;
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_temporal_hyperedge<VertexType, TimeType>& b) {
    if (a._time >= b._time) {
      return false;
    } else {
      std::vector<VertexType> common;
      ranges::set_intersection(a._heads, b._tails,
          std::back_inserter(common));
      return common.size() > 0;
    }
  }

  // properties of directed delayed temporal edges:

  template <network_vertex VertexType, typename TimeType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
    directed_delayed_temporal_hyperedge(
      std::initializer_list<VertexType> tails,
      std::initializer_list<VertexType> heads,
      TimeType cause_time, TimeType effect_time) :
    directed_delayed_temporal_hyperedge(
        std::vector<VertexType>(tails),
        std::vector<VertexType>(heads),
        cause_time, effect_time) {}

  template <network_vertex VertexType, typename TimeType>
  template <ranges::input_range R1, ranges::input_range R2>
  requires
    std::convertible_to<ranges::range_value_t<R1>, VertexType> &&
    std::convertible_to<ranges::range_value_t<R2>, VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
    directed_delayed_temporal_hyperedge(
      const R1& tails, const R2& heads,
      TimeType cause_time, TimeType effect_time) :
      _cause_time(cause_time), _effect_time(effect_time) {
    if (_effect_time < _cause_time)
      throw std::invalid_argument("directed_delayed_temporal_hyperedge cannot"
          " have a cause_time larger than effect_time");

    if constexpr (ranges::sized_range<R1>)
      _heads.reserve(ranges::size(heads));
    ranges::copy(heads, std::back_inserter(_heads));

    if constexpr (ranges::sized_range<R2>)
      _tails.reserve(ranges::size(tails));
    ranges::copy(tails, std::back_inserter(_tails));

    ranges::sort(_heads);
    auto [hfirst, hlast] = ranges::unique(_heads);
    _heads.erase(hfirst, hlast);
    _heads.shrink_to_fit();

    ranges::sort(_tails);
    auto [tfirst, tlast] = ranges::unique(_tails);
    _tails.erase(tfirst, tlast);
    _tails.shrink_to_fit();
  }

  template <network_vertex VertexType, typename TimeType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
    directed_delayed_temporal_hyperedge(
      const directed_hyperedge<VertexType>& projection,
      TimeType cause_time, TimeType effect_time) :
      _cause_time(cause_time), _effect_time(effect_time),
      _tails(projection.tails()), _heads(projection.heads()) {
    if (_effect_time < _cause_time)
      throw std::invalid_argument("directed_delayed_temporal_hyperedge cannot"
          " have a cause_time larger than effect_time");
  }


  template <network_vertex VertexType, typename TimeType>
  directed_hyperedge<VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      static_projection() const {
    return directed_hyperedge<VertexType>(_tails, _heads);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      effect_time() const {
    return _effect_time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      cause_time() const {
    return _cause_time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      is_out_incident(const VertexType& vert) const {
    return std::binary_search(_tails.begin(), _tails.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      is_in_incident(const VertexType& vert) const {
    return std::binary_search(_heads.begin(), _heads.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      is_incident(const VertexType& vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      mutator_verts() const {
    return _tails;
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      mutated_verts() const {
    return _heads;
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      tails() const {
    return _tails;
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      heads() const {
    return _heads;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_hyperedge<VertexType, TimeType>::
      incident_verts() const {
    std::vector<VertexType> res;
    res.reserve(_heads.size() + _tails.size());
    ranges::set_union(_tails, _heads, std::back_inserter(res));
    return res;
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType, typename TimeType>
  auto directed_delayed_temporal_hyperedge<VertexType, TimeType>::operator<=>(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& o)
      const {
    return utils::compare(
      std::tie(_cause_time, _effect_time, _tails, _heads),
      std::tie(o._cause_time, o._effect_time, o._tails, o._heads));
  }
#endif

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b) {
    return std::make_tuple(a._effect_time, a._cause_time, a._heads, a._tails) <
      std::make_tuple(b._effect_time, b._cause_time, b._heads, b._tails);
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& a,
      const directed_delayed_temporal_hyperedge<VertexType, TimeType>& b) {
    if (a._effect_time >= b._cause_time) {
      return false;
    } else {
      std::vector<VertexType> common;
      ranges::set_intersection(a._heads, b._tails,
          std::back_inserter(common));
      return common.size() > 0;
    }
  }

  // properties of undirected temporal edges:

  template <network_vertex VertexType, typename TimeType>
  undirected_temporal_hyperedge<VertexType, TimeType>::
    undirected_temporal_hyperedge(
      std::initializer_list<VertexType> verts, TimeType time) :
    undirected_temporal_hyperedge(std::vector<VertexType>(verts), time) {}

  template <network_vertex VertexType, typename TimeType>
  template <ranges::input_range R>
  requires std::convertible_to<ranges::range_value_t<R>, VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::
    undirected_temporal_hyperedge(
      const R& verts, const TimeType time) : _time(time) {
    if constexpr (ranges::sized_range<R>)
      _verts.reserve(ranges::size(verts));
    ranges::copy(verts, std::back_inserter(_verts));
    ranges::sort(_verts);
    auto [first, last] = ranges::unique(_verts);
    _verts.erase(first, last);
    _verts.shrink_to_fit();
  }


  template <network_vertex VertexType, typename TimeType>
  undirected_temporal_hyperedge<VertexType, TimeType>::
  undirected_temporal_hyperedge(
      const undirected_hyperedge<VertexType>& projection, TimeType time) :
    _time(time), _verts(projection.incident_verts()) {}

  template <network_vertex VertexType, typename TimeType>
  undirected_hyperedge<VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::
      static_projection() const {
    return undirected_hyperedge<VertexType>(_verts);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType undirected_temporal_hyperedge<VertexType, TimeType>::
      effect_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType undirected_temporal_hyperedge<VertexType, TimeType>::
      cause_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_hyperedge<VertexType, TimeType>::
      is_incident(const VertexType& vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_hyperedge<VertexType, TimeType>::
      is_in_incident(const VertexType& vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_hyperedge<VertexType, TimeType>::
      is_out_incident(const VertexType& vert) const {
    return std::binary_search(_verts.begin(), _verts.end(), vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::mutator_verts() const {
    return _verts;
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::mutated_verts() const {
    return _verts;
  }

  template <network_vertex VertexType, typename TimeType>
  std::span<const VertexType>
  undirected_temporal_hyperedge<VertexType, TimeType>::incident_verts() const {
    return _verts;
  }

#if (_LIBCPP_VERSION)
  template <network_vertex VertexType, typename TimeType>
  auto undirected_temporal_hyperedge<VertexType, TimeType>::operator<=>(
      const undirected_temporal_hyperedge<VertexType, TimeType>& o) const {
    return utils::compare(std::tie(_time, _verts), std::tie(o._time, o._verts));
  }
#endif

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const undirected_temporal_hyperedge<VertexType, TimeType>& a,
      const undirected_temporal_hyperedge<VertexType, TimeType>& b) {
    return a < b;
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const undirected_temporal_hyperedge<VertexType, TimeType>& a,
      const undirected_temporal_hyperedge<VertexType, TimeType>& b) {
    if (a._time >= b._time) {
      return false;
    } else {
      std::vector<VertexType> common;
      ranges::set_intersection(a._verts, b._verts,
          std::back_inserter(common));
      return common.size() > 0;
    }
  }
}  // namespace reticula


#endif  // INCLUDE_RETICULA_TEMPORAL_HYPEREDGES_HPP_
