#pragma once

#include <array>
#include <format>
#include <functional>
#include <span>

#include <reticula/concepts.hpp>
#include <reticula/edges/undirected_edge.hpp>
#include <reticula/utils.hpp>

#include <hll/hyperloglog.hpp>

namespace reticula {
/**
  An undirected temporal edge (or link) indicate symmertic relation or
  orientation in the relationship between the two nodes.
*/
class undirected_temporal_edge {
public:
  using StaticProjectionType = undirected_edge;

  undirected_temporal_edge() = default;

  /**
    Create an undirected temporal edge. Order of the vertex parameters are
    arbitrary.
   */
  undirected_temporal_edge(VertexType v1, VertexType v2, TimeType time);

  /**
    Create an undirected temporal edge from a static edge and time.

    @param edge static undirected edge
    @param time the time of the event
   */
  undirected_temporal_edge(const StaticProjectionType& edge, TimeType time);

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
    An undirected temporal edge is incident to vertex `v` iff `v` is either of
    its vertices.

    @param vert Vertex to check the incident relationship with.
   */
  [[nodiscard]]
  auto is_incident(VertexType vert) const -> bool;

  /**
    Exactly the same as `is_incident`.

    @param vert Vertex to check the incident relationship with.
   */
  [[nodiscard]]
  auto is_in_incident(VertexType vert) const -> bool;

  /**
    Exactly the same as `is_incident`.

    @param vert Vertex to check the incident relationship with.
   */
  [[nodiscard]]
  auto is_out_incident(VertexType vert) const -> bool;

  /**
    In an undirected edge both vertices might act as source or cause of an
    effect.
   */
  [[nodiscard]]
  auto mutator_verts() const -> std::span<const VertexType>;

  /**
    In an undirected edge both vertices might act as target of an effect.
   */
  [[nodiscard]]
  auto mutated_verts() const -> std::span<const VertexType>;

  /**
    In an undirected edge both vertices might act are considered incident.
   */
  [[nodiscard]]
  auto incident_verts() const -> std::span<const VertexType>;

  auto operator<=>(const undirected_temporal_edge&) const = default;

  /**
    Two undirected temporal edges are adjacent if they have at least one node in
    common. Lack of an adjacency relation between edges ususlly mean that an
    effect transmitted through one edge logically cannot be transmitted through
    the other.
   */
  friend auto
  adjacent(const undirected_temporal_edge& a, const undirected_temporal_edge& b)
    -> bool;

private:
  TimeType time_;
  std::array<VertexType, 2> verts_;

  friend struct edge_relabel<undirected_temporal_edge>;
  friend struct std::hash<undirected_temporal_edge>;
  friend struct hll::hash<undirected_temporal_edge>;

  friend struct std::formatter<undirected_temporal_edge, char>;
};

auto effect_cmp(
  const undirected_temporal_edge& a, const undirected_temporal_edge& b) -> bool;
auto adjacent(
  const undirected_temporal_edge& a, const undirected_temporal_edge& b) -> bool;

template <>
struct is_instantaneous<undirected_temporal_edge> {
  static constexpr bool value = true;
};

template <>
struct is_undirected<undirected_temporal_edge> {
  static constexpr bool value = true;
};

template <>
struct is_dyadic<undirected_temporal_edge> {
  static constexpr bool value = true;
};

template <>
struct edge_relabel<undirected_temporal_edge> {
  template <std::invocable<VertexType> MapF>
    requires std::convertible_to<
      std::invoke_result_t<MapF, VertexType>, VertexType>
  auto operator()(const undirected_temporal_edge& e, MapF&& map_fun) const
    -> undirected_temporal_edge {
    return undirected_temporal_edge{
      map_fun(e.verts_[0]), map_fun(e.verts_[1]), e.time_};
  }
};
} // namespace reticula

namespace std {
template <>
struct hash<reticula::undirected_temporal_edge> {
  auto operator()(const reticula::undirected_temporal_edge& e) const
    -> std::size_t {
    return reticula::utils::combine_hash<reticula::TimeType, reticula::hash>(
      reticula::utils::combine_hash<reticula::VertexType, reticula::hash>(
        reticula::hash<reticula::VertexType>{}(e.verts_[0]), e.verts_[1]),
      e.time_);
  }
};

template <>
struct formatter<reticula::undirected_temporal_edge, char> {
  constexpr auto parse(format_parse_context& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it == '}')
      return it;
    throw format_error("Invalid format for reticula::undirected_temporal_edge");
  }

  template <typename FormatContext>
  auto format(
    const reticula::undirected_temporal_edge& e, FormatContext& ctx) const {
    return std::format_to(
      ctx.out(), "({} - {} t={})", e.verts_[0], e.verts_[1], e.time_);
  }
};
} // namespace std

namespace hll {
template <>
struct hash<reticula::undirected_temporal_edge> {
  auto operator()(
    const reticula::undirected_temporal_edge& e, std::uint64_t seed) const
    -> std::size_t {
    return hll::hash<std::size_t>{}(
      std::hash<reticula::undirected_temporal_edge>{}(e), seed);
  }
};
} // namespace hll
