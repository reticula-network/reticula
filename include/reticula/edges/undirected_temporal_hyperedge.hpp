#pragma once

#include <format>
#include <functional>
#include <span>
#include <vector>

#include <reticula/concepts.hpp>
#include <reticula/edges/undirected_hyperedge.hpp>
#include <reticula/utils.hpp>

#include <hll/hyperloglog.hpp>

namespace reticula {
/**
  An undirected temporal hyperedge (or temporal link) indicates symmetric
  relation or orientation in the relationship between multiple nodes at a
  specific time.
*/
class undirected_temporal_hyperedge {
public:
  using StaticProjectionType = undirected_hyperedge;

  undirected_temporal_hyperedge() = default;

  /**
    Create an undirected temporal hyperedge. Order of the vertices are
    arbitrary. This variation is specifically created so that a brace-enclosed
    initializer list can be used to initialize this class.
   */
  undirected_temporal_hyperedge(
    std::initializer_list<VertexType> verts, TimeType time);

  /**
    Create an undirected temporal hyperedge. Order of the vertices is arbitrary.
   */
  template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, VertexType>
  explicit undirected_temporal_hyperedge(R&& verts, TimeType time);

  /**
    Create an undirected temporal hyperedge from a static hyperedge and time.

    @param hyperedge static undirected hyperedge
    @param cause_time the time of the event
   */
  undirected_temporal_hyperedge(
    const StaticProjectionType& hyperedge, TimeType time);

  /**
    Static edge that encompasses all the non-temporal information about this
    edge.
   */
  [[nodiscard]]
  auto static_projection() const -> StaticProjectionType;

  /**
    The timestamp that the effect is received by vertices. For undirected
    temporal hyperedges this is equal to the `cause_time` of the edge.
   */
  [[nodiscard]]
  auto effect_time() const -> TimeType;

  /**
    The timestamp that the effect is initiated by vertices.
   */
  [[nodiscard]]
  auto cause_time() const -> TimeType;

  /**
    An undirected temporal hyperedge is incident to vertex `v` iff `v` is one
    of its vertices.

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
    In an undirected temporal hyperedge all vertices might act as source or
    cause of an effect.
   */
  [[nodiscard]]
  auto mutator_verts() const -> std::span<const VertexType>;

  /**
    In an undirected temporal hyperedge all vertices might act as target of an
    effect.
   */
  [[nodiscard]]
  auto mutated_verts() const -> std::span<const VertexType>;

  /**
    In an undirected temporal hyperedge all vertices are considered incident.
   */
  [[nodiscard]]
  auto incident_verts() const -> std::span<const VertexType>;

  auto operator<=>(const undirected_temporal_hyperedge&) const = default;

  /**
    Defines a strong lexicographic ordering along with `operator==` where time
    is compared before vertices.
   */
  friend auto effect_cmp(
    const undirected_temporal_hyperedge& a,
    const undirected_temporal_hyperedge& b) -> bool;

  /**
    Two undirected temporal hyperedges are adjacent if they have at least one
    node in common and the first one's effect time is before or equal to the
    second one's cause time. Lack of an adjacency relation between hyperedges
    usually means that an effect transmitted through one hyperedge logically
    cannot be transmitted through the other.
   */
  friend auto adjacent(
    const undirected_temporal_hyperedge& a,
    const undirected_temporal_hyperedge& b) -> bool;

private:
  TimeType time_;
  std::vector<VertexType> verts_;

  friend struct edge_relabel<undirected_temporal_hyperedge>;
  friend struct std::hash<undirected_temporal_hyperedge>;
  friend struct hll::hash<undirected_temporal_hyperedge>;

  friend struct std::formatter<undirected_temporal_hyperedge, char>;
};

auto effect_cmp(
  const undirected_temporal_hyperedge& a,
  const undirected_temporal_hyperedge& b) -> bool;
auto adjacent(
  const undirected_temporal_hyperedge& a,
  const undirected_temporal_hyperedge& b) -> bool;

template <>
struct is_instantaneous<undirected_temporal_hyperedge> {
  static constexpr bool value = true;
};

template <>
struct is_undirected<undirected_temporal_hyperedge> {
  static constexpr bool value = true;
};

template <>
struct is_dyadic<undirected_temporal_hyperedge> {
  static constexpr bool value = false;
};

template <std::ranges::input_range R>
  requires std::convertible_to<std::ranges::range_value_t<R>, VertexType>
undirected_temporal_hyperedge::undirected_temporal_hyperedge(
  R&& verts, TimeType time)
    : time_(time) {
  if constexpr (std::ranges::sized_range<R>)
    verts_.reserve(std::ranges::size(verts));
  std::ranges::copy(std::forward<R>(verts), std::back_inserter(verts_));
  std::ranges::sort(verts_);
  auto [first, last] = std::ranges::unique(verts_);
  verts_.erase(first, last);
  verts_.shrink_to_fit();
}

template <>
struct edge_relabel<undirected_temporal_hyperedge> {
  template <std::invocable<VertexType> MapF>
    requires std::convertible_to<
      std::invoke_result_t<MapF, VertexType>, VertexType>
  auto operator()(const undirected_temporal_hyperedge& e, MapF&& map_fun) const
    -> undirected_temporal_hyperedge {
    return undirected_temporal_hyperedge{
      e.verts_ | std::views::transform(map_fun), e.time_};
  }
};
} // namespace reticula

namespace std {
template <>
struct hash<reticula::undirected_temporal_hyperedge> {
  auto operator()(const reticula::undirected_temporal_hyperedge& e) const
    -> std::size_t {
    std::size_t verts_hash = std::accumulate(
      e.verts_.begin(), e.verts_.end(), 0ul,
      [](std::size_t h, const reticula::VertexType& v) {
        return reticula::utils::combine_hash<
          reticula::VertexType, reticula::hash>(h, v);
      });
    return reticula::utils::combine_hash<reticula::TimeType, reticula::hash>(
      verts_hash, e.time_);
  }
};

template <>
struct formatter<reticula::undirected_temporal_hyperedge, char> {
  constexpr auto parse(format_parse_context& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it == '}')
      return it;
    throw format_error(
      "Invalid format for reticula::undirected_temporal_hyperedge");
  }

  template <typename FormatContext>
  auto format(
    const reticula::undirected_temporal_hyperedge& e,
    FormatContext& ctx) const {
    return std::format_to(
      ctx.out(), "({{{}}} t={})", reticula::utils::join(e.verts_, ", "),
      e.time_);
  }
};
} // namespace std

namespace hll {
template <>
struct hash<reticula::undirected_temporal_hyperedge> {
  auto operator()(
    const reticula::undirected_temporal_hyperedge& e, std::uint64_t seed) const
    -> uint64_t {
    return hll::hash<std::size_t>{}(
      std::hash<reticula::undirected_temporal_hyperedge>{}(e), seed);
  }
};
} // namespace hll
