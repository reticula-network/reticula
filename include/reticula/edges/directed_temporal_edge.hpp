#pragma once

#include <array>
#include <format>
#include <functional>
#include <span>

#include <reticula/concepts.hpp>
#include <reticula/edges/directed_edge.hpp>
#include <reticula/utils.hpp>

#include <hll/hyperloglog.hpp>

namespace reticula {
/**
  A directed temporal edges (or event) indicate instantaneous asymmertic
  relation or orientation in the relationship between the two nodes, as well as
  a specific time when the event took effect. Tail and head vertices allude to a
  representation of the edge as an arrow from one vertex to another.
*/
class directed_temporal_edge {
public:
  using StaticProjectionType = directed_edge;

  directed_temporal_edge() = default;

  /**
    Create a directed temporal edge.

    @param tail tail end of the edge, often the initiator or cause of the action
    or the relation.
    @param head head end of the edge, often the receiving end of an effect.
    @param time the time of the event
   */
  directed_temporal_edge(VertexType tail, VertexType head, TimeType time);

  /**
    Create a directed temporal edge from a static edge and time.

    @param edge static directed edge
    @param cause_time the time of the event
   */
  directed_temporal_edge(const StaticProjectionType& edge, TimeType time);

  /**
    Static edge that encompasses all the non-temporal information about this
    edge.
   */
  [[nodiscard]]
  auto static_projection() const -> StaticProjectionType;

  /**
    The timestamp that the effect is received by head vertex. For directed
    temporal edges this is equal to the `cause_time` of the edge.
   */
  [[nodiscard]]
  auto effect_time() const -> TimeType;

  /**
    The timestamp that the effect is initiated by tail vertex.
   */
  [[nodiscard]]
  auto cause_time() const -> TimeType;

  /**
    A directed edge is out_incident to vertex `v` iff `v` is the tail vertex
    of the edge.

    @param vert Vertex to check the out_incident relationship with.
   */
  [[nodiscard]]
  auto is_out_incident(VertexType vert) const -> bool;

  /**
    A directed edge is in_incident to vertex `v` iff `v` is the head vertex
    of the edge.

    @param vert Vertex to check the in_incident relationship with.
   */
  [[nodiscard]]
  auto is_in_incident(VertexType vert) const -> bool;

  /**
    A directed edge is incident to vertex `v` iff `v` is the head or tail
    vertex of the edge.

    @param vert Vertex to check the incident relationship with.
   */
  [[nodiscard]]
  auto is_incident(VertexType vert) const -> bool;

  /**
    List of all vertices that can initiate (cause) the effects of the
    relationship. For directed edges this is equal to the tail vertex.
   */
  [[nodiscard]]
  auto mutator_verts() const -> std::span<const VertexType, 1>;

  /**
    List of all vertices that receive (affected by) the effects of the
    relationship. For directed edges this is equal to the head vertex.
   */
  [[nodiscard]]
  auto mutated_verts() const -> std::span<const VertexType, 1>;

  /**
    List of all vertices that initiate (cause) or receive (affected by) the
    effects of the relationship. For directed edges this is equal to the union
    of results of `mutator_verts()` and `mutated_verts()`.
   */
  [[nodiscard]]
  auto incident_verts() const -> std::span<const VertexType>;

  /**
    Returns the single vertex at the tail of the directed link arrow.
   */
  [[nodiscard]]
  auto tail() const -> VertexType;

  /**
    Returns the single vertex at the head of the directed link arrow.
   */
  [[nodiscard]]
  auto head() const -> VertexType;

  auto operator<=>(const directed_temporal_edge& other) const = default;

  /**
    Defines a strong lexicographic ordering along with `operator==` where head
    vertices compared before tail vertices.
   */
  friend auto
  effect_cmp(const directed_temporal_edge& a, const directed_temporal_edge& b)
    -> bool;

  /**
    Two directed edges are adjacent if head of the first one is the tail of
    the second one. Lack of an adjacency relation between edges ususlly mean
    that an effect transmitted through one edge logically cannot be
    transmitted through the other.
   */
  friend auto
  adjacent(const directed_temporal_edge& a, const directed_temporal_edge& b)
    -> bool;

private:
  TimeType time_;
  std::array<VertexType, 2> verts_; // [tail, head]

  friend struct edge_relabel<directed_temporal_edge>;
  friend struct std::hash<directed_temporal_edge>;
  friend struct hll::hash<directed_temporal_edge>;

  friend struct std::formatter<directed_temporal_edge, char>;
};

auto effect_cmp(
  const directed_temporal_edge& a, const directed_temporal_edge& b) -> bool;
auto adjacent(const directed_temporal_edge& a, const directed_temporal_edge& b)
  -> bool;

template <>
struct is_instantaneous<directed_temporal_edge> {
  static constexpr bool value = true;
};

template <>
struct is_undirected<directed_temporal_edge> {
  static constexpr bool value = false;
};

template <>
struct is_dyadic<directed_temporal_edge> {
  static constexpr bool value = true;
};

template <>
struct edge_relabel<directed_temporal_edge> {
  template <std::invocable<VertexType> MapF>
    requires std::convertible_to<
      std::invoke_result_t<MapF, VertexType>, VertexType>
  auto operator()(const directed_temporal_edge& e, MapF&& map_fun) const
    -> directed_temporal_edge {
    return directed_temporal_edge{
      map_fun(e.verts_[0]), map_fun(e.verts_[1]), e.time_};
  }
};
} // namespace reticula

namespace std {
template <>
struct hash<reticula::directed_temporal_edge> {
  auto operator()(const reticula::directed_temporal_edge& e) const
    -> std::size_t {
    return reticula::utils::combine_hash<reticula::TimeType, reticula::hash>(
      reticula::utils::combine_hash<reticula::VertexType, reticula::hash>(
        reticula::hash<reticula::VertexType>{}(e.verts_[0]), e.verts_[1]),
      e.time_);
  }
};

template <>
struct formatter<reticula::directed_temporal_edge, char> {
  constexpr auto parse(format_parse_context& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it == '}')
      return it;
    throw format_error("Invalid format for reticula::directed_temporal_edge");
  }

  template <typename FormatContext>
  auto
  format(const reticula::directed_temporal_edge& e, FormatContext& ctx) const {
    return std::format_to(
      ctx.out(), "({} -> {} t={})", e.verts_[0], e.verts_[1], e.time_);
  }
};
} // namespace std

namespace hll {
template <>
struct hash<reticula::directed_temporal_edge> {
  auto operator()(const reticula::directed_temporal_edge& e, std::uint64_t seed)
    const -> uint64_t {
    return hll::hash<std::size_t>{}(
      std::hash<reticula::directed_temporal_edge>{}(e), seed);
  }
};
} // namespace hll
