#pragma once

#include <format>
#include <functional>
#include <span>
#include <vector>

#include <reticula/concepts.hpp>
#include <reticula/edges/directed_hyperedge.hpp>
#include <reticula/utils.hpp>

#include <hll/hyperloglog.hpp>

namespace reticula {
/**
  A directed temporal hyperedge (or temporal hyperlink) indicates asymmetric
  relation or orientation in the relationship between two sets of nodes at a
  specific time. Tail and head sets of vertices allude to a representation of
  the hyperedge as an arrow from one set of vertices to another.
*/
class directed_temporal_hyperedge {
public:
  using StaticProjectionType = directed_hyperedge;

  directed_temporal_hyperedge() = default;

  /**
    Create a directed temporal hyperedge.

    @param tails Tail end of the hyperedge, often the initiator or cause of the
    action or the relation.
    @param heads Head end of the hyperedge, often the receiving end of an
    effect.
    @param time The time of the event.
   */
  directed_temporal_hyperedge(
    std::initializer_list<VertexType> tails,
    std::initializer_list<VertexType> heads, TimeType time);

  /**
    Create a directed temporal hyperedge.

    @param tails The set of vertices at the tail end of the hyperedge, often
    the initiator or cause of the action or the relation.
    @param heads The set of vertices at the head end of the hyperedge, often
    the receiving end of an effect.
    @param time The time of the event.
   */
  template <std::ranges::input_range R1, std::ranges::input_range R2>
    requires std::convertible_to<std::ranges::range_value_t<R1>, VertexType> &&
             std::convertible_to<std::ranges::range_value_t<R2>, VertexType>
  directed_temporal_hyperedge(R1&& tails, R2&& heads, TimeType time);

  /**
    Create a directed temporal hyperedge from a static hyperedge and time.

    @param hyperedge static directed hyperedge
    @param time the time of the event
   */
  directed_temporal_hyperedge(
    const StaticProjectionType& hyperedge, TimeType time);

  /**
    Static edge that encompasses all the non-temporal information about this
    edge.
   */
  [[nodiscard]]
  auto static_projection() const -> StaticProjectionType;

  /**
    The timestamp that the effect is received by head vertices. For directed
    temporal hyperedges this is equal to the `cause_time` of the edge.
   */
  [[nodiscard]]
  auto effect_time() const -> TimeType;

  /**
    The timestamp that the effect is initiated by tail vertices.
   */
  [[nodiscard]]
  auto cause_time() const -> TimeType;

  /**
    A directed temporal hyperedge is out_incident to vertex `v` iff `v` is in
    the tail set of vertices of the hyperedge.

    @param vert Vertex to check the out_incident relationship with.
   */
  [[nodiscard]]
  auto is_out_incident(VertexType vert) const -> bool;

  /**
    A directed temporal hyperedge is in_incident to vertex `v` iff `v` is in
    the head set of vertices of the hyperedge.

    @param vert Vertex to check the in_incident relationship with.
   */
  [[nodiscard]]
  auto is_in_incident(VertexType vert) const -> bool;

  /**
    A directed temporal hyperedge is incident to vertex `v` iff `v` is in the
    head or tail set of vertices of the hyperedge.

    @param vert Vertex to check the incident relationship with.
   */
  [[nodiscard]]
  auto is_incident(VertexType vert) const -> bool;

  /**
    List of all vertices that can initiate (cause) the effects of the
    relationship. For directed temporal hyperedges this is equal to the set of
    tail vertices.
   */
  [[nodiscard]]
  auto mutator_verts() const -> std::span<const VertexType>;

  /**
    List of all vertices that receive (affected by) the effects of the
    relationship. For directed temporal hyperedges this is equal to the set of
    head vertices.
   */
  [[nodiscard]]
  auto mutated_verts() const -> std::span<const VertexType>;

  /**
    List of all vertices that initiate (cause) or receive (affected by) the
    effects of the relationship. For directed temporal hyperedges this is equal
    to the union of results of `mutator_verts()` and `mutated_verts()`.
   */
  [[nodiscard]]
  auto incident_verts() const -> std::vector<VertexType>;

  /**
    The same as `mutator_verts()`.
   */
  [[nodiscard]]
  auto tails() const -> std::span<const VertexType>;

  /**
    The same as `mutated_verts()`.
   */
  [[nodiscard]]
  auto heads() const -> std::span<const VertexType>;

  auto operator<=>(const directed_temporal_hyperedge& other) const = default;

  /**
    Defines a strong lexicographic ordering along with `operator==` where time
    is compared first, then head vertices, then tail vertices.
   */
  friend auto effect_cmp(
    const directed_temporal_hyperedge& a, const directed_temporal_hyperedge& b)
    -> bool;

  /**
    Two directed temporal hyperedges are adjacent if at least one head of the
    first one is in the tail set of the second one and the first one's effect
    time is before or equal to the second one's cause time. Lack of an
    adjacency relation between hyperedges usually means that an effect
    transmitted through one hyperedge logically cannot be transmitted through
    the other.
   */
  friend auto adjacent(
    const directed_temporal_hyperedge& a, const directed_temporal_hyperedge& b)
    -> bool;

private:
  TimeType time_;
  std::vector<VertexType> tails_, heads_;

  friend struct edge_relabel<directed_temporal_hyperedge>;
  friend struct std::hash<directed_temporal_hyperedge>;
  friend struct hll::hash<directed_temporal_hyperedge>;

  friend struct std::formatter<directed_temporal_hyperedge, char>;
};

auto effect_cmp(
  const directed_temporal_hyperedge& a, const directed_temporal_hyperedge& b)
  -> bool;
auto adjacent(
  const directed_temporal_hyperedge& a, const directed_temporal_hyperedge& b)
  -> bool;

template <>
struct is_instantaneous<directed_temporal_hyperedge> {
  static constexpr bool value = true;
};

template <>
struct is_undirected<directed_temporal_hyperedge> {
  static constexpr bool value = false;
};

template <>
struct is_dyadic<directed_temporal_hyperedge> {
  static constexpr bool value = false;
};

template <std::ranges::input_range R1, std::ranges::input_range R2>
  requires std::convertible_to<std::ranges::range_value_t<R1>, VertexType> &&
           std::convertible_to<std::ranges::range_value_t<R2>, VertexType>
directed_temporal_hyperedge::directed_temporal_hyperedge(
  R1&& tails, R2&& heads, TimeType time)
    : time_(time) {
  if constexpr (std::ranges::sized_range<R2>)
    heads_.reserve(std::ranges::size(heads));
  std::ranges::copy(std::forward<R2>(heads), std::back_inserter(heads_));

  if constexpr (std::ranges::sized_range<R1>)
    tails_.reserve(std::ranges::size(tails));
  std::ranges::copy(std::forward<R1>(tails), std::back_inserter(tails_));

  std::ranges::sort(heads_);
  auto [hfirst, hlast] = std::ranges::unique(heads_);
  heads_.erase(hfirst, hlast);
  heads_.shrink_to_fit();

  std::ranges::sort(tails_);
  auto [tfirst, tlast] = std::ranges::unique(tails_);
  tails_.erase(tfirst, tlast);
  tails_.shrink_to_fit();
}

template <>
struct edge_relabel<directed_temporal_hyperedge> {
  template <std::invocable<VertexType> MapF>
    requires std::convertible_to<
      std::invoke_result_t<MapF, VertexType>, VertexType>
  auto operator()(const directed_temporal_hyperedge& e, MapF&& map_fun) const
    -> directed_temporal_hyperedge {
    return directed_temporal_hyperedge{
      e.tails_ | std::views::transform(map_fun),
      e.heads_ | std::views::transform(map_fun), e.time_};
  }
};
} // namespace reticula

namespace std {
template <>
struct hash<reticula::directed_temporal_hyperedge> {
  auto operator()(const reticula::directed_temporal_hyperedge& e) const
    -> std::size_t {
    std::size_t heads_hash = std::accumulate(
      e.heads_.begin(), e.heads_.end(), 0ul,
      [](std::size_t h, const reticula::VertexType& v) {
        return reticula::utils::combine_hash<
          reticula::VertexType, reticula::hash>(h, v);
      });
    std::size_t tails_hash = std::accumulate(
      e.tails_.begin(), e.tails_.end(), 0ul,
      [](std::size_t h, const reticula::VertexType& v) {
        return reticula::utils::combine_hash<
          reticula::VertexType, reticula::hash>(h, v);
      });
    return reticula::utils::combine_hash<reticula::TimeType, reticula::hash>(
      reticula::utils::combine_hash<std::size_t, reticula::hash>(
        heads_hash, tails_hash),
      e.time_);
  }
};

template <>
struct formatter<reticula::directed_temporal_hyperedge, char> {
  constexpr auto parse(format_parse_context& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it == '}')
      return it;
    throw format_error(
      "Invalid format for reticula::directed_temporal_hyperedge");
  }

  template <typename FormatContext>
  auto format(
    const reticula::directed_temporal_hyperedge& e, FormatContext& ctx) const {
    return std::format_to(
      ctx.out(), "({{{}}} -> {{{}}} t={})",
      reticula::utils::join(e.tails(), ", "),
      reticula::utils::join(e.heads(), ", "), e.time_);
  }
};
} // namespace std

namespace hll {
template <>
struct hash<reticula::directed_temporal_hyperedge> {
  auto operator()(
    const reticula::directed_temporal_hyperedge& e, std::uint64_t seed) const
    -> uint64_t {
    return hll::hash<std::size_t>{}(
      std::hash<reticula::directed_temporal_hyperedge>{}(e), seed);
  }
};
} // namespace hll
