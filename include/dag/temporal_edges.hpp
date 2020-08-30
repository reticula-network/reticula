#ifndef INCLUDE_DAG_TEMPORAL_EDGES_HPP_
#define INCLUDE_DAG_TEMPORAL_EDGES_HPP_

#include <tuple>
#include <vector>
#include <algorithm>
#include <hyperloglog.hpp>

#include "type_traits.hpp"
#include "static_edges.hpp"

namespace dag {
  template <class VertT, class TimeT>
  class undirected_temporal_edge;

  template <class VertT, class TimeT>
  struct is_instantaneous<undirected_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <class VertT, class TimeT>
  struct is_undirected<undirected_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <class VertT, class TimeT>
  class directed_temporal_edge;

  template <class VertT, class TimeT>
  struct is_instantaneous<directed_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = true;
  };

  template <class VertT, class TimeT>
  struct is_undirected<directed_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <class VertT, class TimeT>
  class directed_delayed_temporal_edge;

  template <class VertT, class TimeT>
  struct is_instantaneous<directed_delayed_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <class VertT, class TimeT>
  struct is_undirected<directed_delayed_temporal_edge<VertT, TimeT>> {
    static constexpr bool value = false;
  };

  template <class VertT, class TimeT>
  bool effect_lt(
      const undirected_temporal_edge<VertT, TimeT>& a,
      const undirected_temporal_edge<VertT, TimeT>& b) {
      return (a < b);
  }

  template <class VertT, class TimeT>
  bool effect_lt(
      const directed_temporal_edge<VertT, TimeT>& a,
      const directed_temporal_edge<VertT, TimeT>& b) {
      return (a.effect_comp_tuple() < b.effect_comp_tuple());
  }

  template <class VertT, class TimeT>
  bool effect_lt(
      const directed_delayed_temporal_edge<VertT, TimeT>& a,
      const directed_delayed_temporal_edge<VertT, TimeT>& b) {
      return (a.effect_comp_tuple() < b.effect_comp_tuple());
  }

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
  template <typename VertT, typename TimeT>
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

    directed_temporal_edge() = default;
    /**
      Create a directed temporal edge.

      @param v1 Tail end of the edge, often the initiator or cause of the action
      or the relation.
      @param v2 Head end of the edge, often the receiving end of an effect.
      @param time Timestamp at which the event "happened".
     */
    directed_temporal_edge(
        const VertexType v1, const VertexType v2, const TimeType time)
        : v1(v1), v2(v2), time(time) {}

    /**
      Static edge that encompasses all the non-temporal information about this
      edge.
     */
    [[nodiscard]]
    directed_edge<VertT>
    static_projection() { return directed_edge<VertT>(v1, v2); }

    /**
      The timestamp that the effect is received by head vertex. For directed
      temporal edges this is equal to the `cause_time` of the edge.
     */
    [[nodiscard]]
    TimeType effect_time() const { return time; }

    /**
      The timestamp that the effect is initiated by tail vertex.
     */
    [[nodiscard]]
    TimeType cause_time() const { return time; }

    /**
      A directed temporal edge is out_incident to vertex `v` iff `v` is the
      tail vertex of the edge.

      @param vert Vertex to check the out_incident relationship with.
     */
    [[nodiscard]]
    bool is_out_incident(const VertexType vert) const { return (v1 == vert); }

    /**
      A directed temporal edge is in_incident to vertex `v` iff `v` is the head
      vertex of the edge.

      @param vert Vertex to check the in_incident relationship with.
     */
    [[nodiscard]]
    bool is_in_incident(const VertexType vert) const  { return (v2 == vert); }

    /**
      A directed temporal edge is incident to vertex `v` iff `v` is the head or
      tail vertex of the edge.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    bool is_incident(const VertexType vert) const {
      return (v1 == vert || v2 == vert);
    }

    /**
      List of all vertices that initiate (cause) the effects of the
      relationship. For directed temporal edges this is equal to the tail
      vertex.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const { return {v1}; }

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed temporal edges this is equal to the head
      vertex.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const { return {v2}; }

    /**
      Two directed temporal edges are adjacent if head of the first one is the
      tail of the second one and the cause time of the second one is after the
      first. Lack of an adjacency relation between edges ususlly mean that an
      effect transmitted through one edge logically cannot be transmitted
      through the other.
     */
    [[nodiscard]]
    friend bool adjacent(
        const directed_temporal_edge<VertexType, TimeType>& a,
        const directed_temporal_edge<VertexType, TimeType>& b) {
      return ((b.time > a.time) && (a.v2 == b.v1));
    }

    /**
      Simply defined as negation of equal operator `operator==`.
     */
    [[nodiscard]]
    friend bool operator!=(
        const directed_temporal_edge<VertexType, TimeType>& a,
        const directed_temporal_edge<VertexType, TimeType>& b) {
      return !(a == b);
    }

    /**
      Two directed temporal edges are equal if their casue time and head and
      tail vertices are correspondingly equal to each others.
     */
    [[nodiscard]]
    friend bool operator==(
        const directed_temporal_edge<VertexType, TimeType>& a,
        const directed_temporal_edge<VertexType, TimeType>& b) {
      return (a.cause_comp_tuple() == b.cause_comp_tuple());
    }

    /**
      Defines a strong lexicographic ordering along with `operator==` where
      cause times are compare then tail vertices and then head vertices.
     */
    [[nodiscard]]
    friend bool operator<(
        const directed_temporal_edge<VertexType, TimeType>& a,
        const directed_temporal_edge<VertexType, TimeType>& b) {
      return (a.cause_comp_tuple() < b.cause_comp_tuple());
    }

    /**
      Defines a strong lexicographic ordering along with `operator==` where
      cause times are compared then head vertices and then tail vertices.
     */
    [[nodiscard]]
    friend bool ::dag::effect_lt<>(
        const directed_temporal_edge<VertexType, TimeType>& a,
        const directed_temporal_edge<VertexType, TimeType>& b);

    /**
      Inserts undirected edge formatted as `v1 v2 time` where `v1` and `v2` are
      tail and head vertex respectively and `time` is the cause time.
     */
    friend std::ostream& operator<<(std::ostream& os,
        const directed_temporal_edge<VertexType, TimeType>& e) {
      return os << e.v1 << " " << e.v2 << " " << e.time;
    }

    /**
      Extracts directed edge from an input stream formatted as `v1 v2 time`
      where `v1` and `v2` are tail and head vertex respectively and time is the
      cause time.
     */
    friend std::istream& operator>>(std::istream& is,
        directed_temporal_edge<VertexType, TimeType>& e) {
      return is >> e.v1 >> e.v2 >> e.time;
    }

  private:
    VertexType v1, v2;
    TimeType time;

    std::tuple<TimeType, VertexType, VertexType>
    cause_comp_tuple() const {
      return std::make_tuple(time, v1, v2);
    }

    std::tuple<TimeType, VertexType, VertexType>
    effect_comp_tuple() const {
      return std::make_tuple(time, v2, v1);
    }

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
  template <typename VertT, typename TimeT>
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

      @param v1 Tail end of the edge, often the initiator or cause of the action
      or the relation.
      @param v2 Head end of the edge, often the receiving end of an effect.
      @param time Timestamp at which the event "happened".
      @param delay Timestamp at which the event was "received".
     */
    directed_delayed_temporal_edge(
        const VertexType v1, const VertexType v2,
        const TimeType time, const TimeType delay)
      : v1(v1), v2(v2), time(time), delay(delay) {}

    /**
      Static edge that encompasses all the non-temporal information about this
      edge.
     */
    [[nodiscard]]
    directed_edge<VertT>
    static_projection() { return directed_edge<VertT>(v1, v2); }

    /**
      The timestamp that the effect is received by head vertex. For directed
      delayed temporal edges this is equal to the `cause_time` plus delay of the
      edge.
     */
    [[nodiscard]]
    TimeType effect_time() const { return time+delay; }

    /**
      The timestamp that the effect is initiated by tail vertex.
     */
    [[nodiscard]]
    TimeType cause_time() const { return time; }

    /**
      A directed delayed temporal edge is out_incident to vertex `v` iff `v` is
      the tail vertex of the edge.

      @param vert Vertex to check the out_incident relationship with.
     */
    [[nodiscard]]
    bool is_out_incident(const VertexType vert) const { return (v1 == vert); }

    /**
      A directed delayed temporal edge is in_incident to vertex `v` iff `v` is
      the head vertex of the edge.

      @param vert Vertex to check the in_incident relationship with.
     */
    [[nodiscard]]
    bool is_in_incident(const VertexType vert) const  { return (v2 == vert); }

    /**
      A directed delayed temporal edge is incident to vertex `v` iff `v` is the
      head or tail vertex of the edge.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    bool is_incident(const VertexType vert) const {
      return (v1 == vert || v2 == vert);
    }

    /**
      List of all vertices that initiate (cause) the effects of the
      relationship. For directed delayed temporal edges this is equal to the
      tail vertex.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const { return {v1}; }

    /**
      List of all vertices that receive (affected by) the effects of the
      relationship. For directed delayed temporal edges this is equal to the
      head vertex.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const { return {v2}; }

    /**
      Two directed delayed temporal edges are adjacent if head of the first one
      is the tail of the second one and the cause time of the second one is
      after the effect time of the first. Lack of an adjacency relation between
      edges ususlly mean that an effect transmitted through one edge logically
      cannot be transmitted through the other.
     */
    [[nodiscard]]
    friend bool adjacent(
        const directed_delayed_temporal_edge<VertexType, TimeType>& a,
        const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
      return ((b.time > a.effect_time()) && (a.v2 == b.v1));
    }

    /**
      Simply defined as negation of equal operator `operator==`.
     */
    [[nodiscard]]
    friend bool operator!=(
        const directed_delayed_temporal_edge<VertexType, TimeType>& a,
        const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
      return !(a == b);
    }

    /**
      Two directed temporal edges are equal if their cause times, effect times
      and head and tail vertices are correspondingly equal to each others.
     */
    [[nodiscard]]
    friend bool operator==(
        const directed_delayed_temporal_edge<VertexType, TimeType>& a,
        const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
      return (a.cause_comp_tuple() == b.cause_comp_tuple());
    }

    /**
      Defines a strong lexicographic ordering along with `operator==` where
      cause times are compare then effect times then tail vertices and finally
      head vertices.
     */
    [[nodiscard]]
    friend bool operator<(
        const directed_delayed_temporal_edge<VertexType, TimeType>& a,
        const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
      return (a.cause_comp_tuple() < b.cause_comp_tuple());
    }

    [[nodiscard]]
    friend bool ::dag::effect_lt<>(
        const directed_delayed_temporal_edge<VertexType, TimeType>& a,
        const directed_delayed_temporal_edge<VertexType, TimeType>& b);

    friend std::ostream& operator<<(std::ostream& os,
        const directed_delayed_temporal_edge<VertexType, TimeType>& e) {
      return os << e.v1 << " " << e.v2 << " " << e.time << " " << e.delay;
    }

    friend std::istream& operator>>(std::istream& is,
        directed_delayed_temporal_edge<VertexType, TimeType>& e) {
      return is >> e.v1 >> e.v2 >> e.time >> e.delay;
    }

  private:
    VertexType v1, v2;
    TimeType time, delay;

    std::tuple<TimeType, TimeType, VertexType, VertexType>
    cause_comp_tuple() const {
      return std::make_tuple(time, time+delay, v1, v2);
    }

    std::tuple<TimeType, TimeType, VertexType, VertexType>
    effect_comp_tuple() const {
      return std::make_tuple(time+delay, time, v2, v1);
    }

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
  template <typename VertT, typename TimeT>
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

    undirected_temporal_edge() = default;
    undirected_temporal_edge(
        const VertexType v1, const VertexType v2, const TimeType time)
        : v1(v1), v2(v2), time(time) {}

    /**
      Static edge that encompasses all the non-temporal information about this
      edge.
     */
    [[nodiscard]]
    undirected_edge<VertT>
    static_projection() { return undirected_edge<VertT>(v1, v2); }

    /**
      The timestamp that the effect is received by head vertex. For undirected
      temporal edges this is equal to the `cause_time` of the edge.
     */
    [[nodiscard]]
    TimeType effect_time() const { return time; }

    /**
      The timestamp that the effect is initiated by tail vertex.
     */
    [[nodiscard]]
    TimeType cause_time() const { return time; }

    /**
      An undirected temporal edge is incident to vertex `v` iff `v` is either of
      its vertices.

      @param vert Vertex to check the incident relationship with.
     */
    [[nodiscard]]
    bool is_incident(const VertexType vert) const {
      return (v1 == vert || v2 == vert);
    }

    /**
      In an undirected temporal edge both edges might act as source or cause of
      an effect.
     */
    [[nodiscard]]
    std::vector<VertexType> mutator_verts() const { return {v1, v2}; }

    /**
      In an undirected edge both edges might act as target of an effect.
     */
    [[nodiscard]]
    std::vector<VertexType> mutated_verts() const { return {v1, v2}; }

    /**
      Two undirected temporal edges are adjacent if they share at least on node
      and the cause time of the second one is after the first. Lack of an
      adjacency relation between edges ususlly mean that an effect transmitted
      through one edge logically cannot be transmitted through the other.
     */
    [[nodiscard]]
    friend bool adjacent(
        const undirected_temporal_edge<VertexType, TimeType>& a,
        const undirected_temporal_edge<VertexType, TimeType>& b) {
      return ((b.time > a.time) &&
                (a.v1 == b.v1 ||
                 a.v1 == b.v2 ||
                 a.v2 == b.v1 ||
                 a.v2 == b.v2));
    }

    /**
      Simply defined as negation of equal operator `operator==`.
     */
    [[nodiscard]]
    friend bool operator!=(
      const undirected_temporal_edge<VertexType, TimeType>& a,
      const undirected_temporal_edge<VertexType, TimeType>& b) {
      return !(a == b);
    }

    /**
      Two undirected temporal edges are equal if the (unordered) set of their
      vertices and their cause times are equal.
     */
    [[nodiscard]]
    friend bool operator==(
        const undirected_temporal_edge<VertexType, TimeType>& a,
        const undirected_temporal_edge<VertexType, TimeType>& b) {
      return (a.comp_tuple() == b.comp_tuple());
    }

    /**
      Defines a weak ordering along with `operator==` that would rank events
      based on cause times first.
     */
    [[nodiscard]]
    friend bool operator<(
        const undirected_temporal_edge<VertexType, TimeType>& a,
        const undirected_temporal_edge<VertexType, TimeType>& b) {
      return (a.comp_tuple() < b.comp_tuple());
    }

    /**
      Exactly the same as `operator<`.
     */
    [[nodiscard]]
    friend bool ::dag::effect_lt<>(
        const undirected_temporal_edge<VertexType, TimeType>& a,
        const undirected_temporal_edge<VertexType, TimeType>& b);

    /**
      Inserts undirected temporal edge formatted as `v1 v2 time` where `v1` and
      `v2` are vertex in the same order as initialiser and `time` is the cause
      time.
     */
    friend std::ostream& operator<<(std::ostream& os,
        const undirected_temporal_edge<VertexType, TimeType>& e) {
      return os <<  e.v1 << " " << e.v2 << " " << e.time;
    }

    /**
      Extracts undirected temporal edge from an input stream formatted as
      `v1 v2 time`.
     */
    friend std::istream& operator>>(std::istream& is,
        undirected_temporal_edge<VertexType, TimeType>& e) {
      return is >> e.v1 >> e.v2 >> e.time;
    }

  private:
    VertexType v1, v2;
    TimeType time;

    std::tuple<TimeType, VertexType, VertexType> comp_tuple() const {
      return std::make_tuple(time, std::min(v1, v2), std::max(v1, v2));
    }

    friend struct std::hash<undirected_temporal_edge<VertexType, TimeType>>;
    friend struct hll::hash<undirected_temporal_edge<VertexType, TimeType>>;
  };
}  // namespace dag

#include "../../src/temporal_edges.tpp"

#endif  // INCLUDE_DAG_TEMPORAL_EDGES_HPP_
