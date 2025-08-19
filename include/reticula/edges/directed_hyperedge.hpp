#pragma once

#include <array>
#include <format>
#include <functional>
#include <span>

#include <reticula/concepts.hpp>
#include <reticula/utils.hpp>

#include <hll/hyperloglog.hpp>

namespace reticula {
/**
  A directed hyperedges (or hyperlink) indicate unsymertic relation or
  orientation in the relationship between the two set of nodes. Tail and head
  set of vertices allude to a representation of the hyperedge as an arrow from
  one set of vertices to another.
*/
class directed_hyperedge {
public:
  directed_hyperedge() = default;

  /**
    Create a directed hyperedge.

    @param tails Tail end of the hyperedge, often the initiator or cause of the
    action or the relation.
    @param heads Head end of the hyperedge, often the receiving end of an
    effect.
   */
  directed_hyperedge(
    std::initializer_list<VertexType> tails,
    std::initializer_list<VertexType> heads);

  /**
    Create a directed hyperedge.

    @param tails The set of vertices at the tail end of the hyperedge, often
    the initiator or cause of the action or the relation.
    @param heads The set of vertices at the head end of the hyperedge, often
    the receiving end of an effect.
   */
  template <std::ranges::input_range R1, std::ranges::input_range R2>
    requires std::convertible_to<std::ranges::range_value_t<R1>, VertexType> &&
             std::convertible_to<std::ranges::range_value_t<R2>, VertexType>
  directed_hyperedge(R1&& tails, R2&& heads);

  /**
    A directed hyperedge is out_incident to vertex `v` iff `v` is in the tail
    set of vertices of the hyperedge.

    @param vert Vertex to check the out_incident relationship with.
   */
  [[nodiscard]]
  auto is_out_incident(VertexType vert) const -> bool;

  /**
    A directed hyperedge is in_incident to vertex `v` iff `v` is in the head set
    of vertices of the hyperedge.

    @param vert Vertex to check the in_incident relationship with.
   */
  [[nodiscard]]
  auto is_in_incident(VertexType vert) const -> bool;

  /**
    A directed hyperedge is incident to vertex `v` iff `v` is in the head or
    tail set of vertices of the hyperedge.

    @param vert Vertex to check the incident relationship with.
   */
  [[nodiscard]]
  auto is_incident(VertexType vert) const -> bool;

  /**
    List of all vertices that can initiate (cause) the effects of the
    relationship. For directed hyperedges this is equal to the set of tail
    vertices.
   */
  [[nodiscard]]
  auto mutator_verts() const -> std::span<const VertexType>;

  /**
    List of all vertices that receive (affected by) the effects of the
    relationship. For directed hyperedges this is equal to the set of head
    vertices.
   */
  [[nodiscard]]
  auto mutated_verts() const -> std::span<const VertexType>;

  /**
    List of all vertices that initiate (cause) or receive (affected by) the
    effects of the relationship. For directed hyperedges this is equal to the
    union of results of `mutator_verts()` and `mutated_verts()`.
   */
  [[nodiscard]]
  auto incident_verts() const -> std::vector<VertexType>;

  /**
    The same as `mutated_verts()`.
   */
  [[nodiscard]]
  auto tails() const -> std::span<const VertexType>;

  /**
    The same as `mutator_verts()`.
   */
  [[nodiscard]]
  auto heads() const -> std::span<const VertexType>;

  auto operator<=>(const directed_hyperedge& other) const = default;

  /**
    Defines a strong lexicographic ordering along with `operator==` where head
    vertices compared before tail vertices.
   */
  friend auto
  effect_cmp(const directed_hyperedge& a, const directed_hyperedge& b) -> bool;

  /**
    Two directed hyperedges are adjacent if at least one head of the first one
    is in the tail set of the second one. Lack of an adjacency relation between
    hyperedges ususlly mean that an effect transmitted through one hyperedge
    logically cannot be transmitted through the other.
   */
  friend auto adjacent(const directed_hyperedge& a, const directed_hyperedge& b)
    -> bool;

private:
  std::vector<VertexType> tails_, heads_;

  friend struct edge_relabel<directed_hyperedge>;
  friend struct std::hash<directed_hyperedge>;
  friend struct hll::hash<directed_hyperedge>;

  friend struct std::formatter<directed_hyperedge, char>;
};

auto effect_cmp(const directed_hyperedge& a, const directed_hyperedge& b)
  -> bool;
auto adjacent(const directed_hyperedge& a, const directed_hyperedge& b) -> bool;

template <>
struct is_instantaneous<directed_hyperedge> {
  static constexpr bool value = true;
};

template <>
struct is_undirected<directed_hyperedge> {
  static constexpr bool value = false;
};

template <>
struct is_dyadic<directed_hyperedge> {
  static constexpr bool value = false;
};

template <std::ranges::input_range R1, std::ranges::input_range R2>
  requires std::convertible_to<std::ranges::range_value_t<R1>, VertexType> &&
           std::convertible_to<std::ranges::range_value_t<R2>, VertexType>
directed_hyperedge::directed_hyperedge(R1&& tails, R2&& heads) {
  if constexpr (std::ranges::sized_range<R1>)
    heads_.reserve(std::ranges::size(heads));
  std::ranges::copy(std::forward<R2>(heads), std::back_inserter(heads_));

  if constexpr (std::ranges::sized_range<R2>)
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
struct edge_relabel<directed_hyperedge> {
  template <std::invocable<VertexType> MapF>
    requires std::convertible_to<
      std::invoke_result_t<MapF, VertexType>, VertexType>
  auto operator()(const directed_hyperedge& e, MapF&& map_fun) const
    -> directed_hyperedge {
    return directed_hyperedge{
      e.tails_ | std::views::transform(map_fun),
      e.heads_ | std::views::transform(map_fun)};
  }
};
} // namespace reticula

namespace std {
template <>
struct hash<reticula::directed_hyperedge> {
  auto operator()(const reticula::directed_hyperedge& e) const -> std::size_t {
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
    return reticula::utils::combine_hash<std::size_t, reticula::hash>(
      heads_hash, tails_hash);
  }
};

template <>
struct formatter<reticula::directed_hyperedge, char> {
  constexpr auto parse(format_parse_context& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it == '}')
      return it;
    throw format_error("Invalid format for reticula::directed_hyperedge");
  }

  template <typename FormatContext>
  auto format(const reticula::directed_hyperedge& e, FormatContext& ctx) const {
    return std::format_to(
      ctx.out(), "({{{}}} -> {{{}}})", reticula::utils::join(e.tails(), ", "),
      reticula::utils::join(e.heads(), ", "));
  }
};
} // namespace std

namespace hll {
template <>
struct hash<reticula::directed_hyperedge> {
  auto
  operator()(const reticula::directed_hyperedge& e, std::uint64_t seed) const
    -> uint64_t {
    return hll::hash<std::size_t>{}(
      std::hash<reticula::directed_hyperedge>{}(e), seed);
  }
};
} // namespace hll
