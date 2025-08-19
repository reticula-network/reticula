#pragma once

#include <format>
#include <functional>
#include <span>
#include <vector>

#include <reticula/concepts.hpp>
#include <reticula/utils.hpp>

#include <hll/hyperloglog.hpp>

namespace reticula {
/**
  An undirected hyperedge (or link) indicate symmertic relation or orientation
  in the relationship between the two nodes.
*/
class undirected_hyperedge {
public:
  undirected_hyperedge() = default;

  /**
    Create an undirected hyperedge. Order of the vertices are arbitrary. This
    variation is specifically created so that a brace-enclosed initializer
    list can be used to initialize this class.
   */
  undirected_hyperedge(std::initializer_list<VertexType> verts);

  /**
    Create an undirected hyperedge. Order of the vertices is arbitrary.
   */
  template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, VertexType>
  explicit undirected_hyperedge(R&& verts);

  /**
    An undirected hyperedge is incident to vertex `v` iff `v` is one of its
    vertices.

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
    In an undirected hyperedge all vertices might act as source or cause of an
    effect.
   */
  [[nodiscard]]
  auto mutator_verts() const -> std::span<const VertexType>;

  /**
    In an undirected hyperedge all vertices might act as target of an effect.
   */
  [[nodiscard]]
  auto mutated_verts() const -> std::span<const VertexType>;

  /**
    In an undirected hyperedge all vertices might act are considered incident.
   */
  [[nodiscard]]
  auto incident_verts() const -> std::span<const VertexType>;

  auto operator<=>(const undirected_hyperedge&) const = default;

  /**
    Exactly the same as `operator<`.
   */
  friend auto
  effect_cmp(const undirected_hyperedge& a, const undirected_hyperedge& b)
    -> bool;

  /**
    Two undirected hyperedges are adjacent if they have at least one node in
    common. Lack of an adjacency relation between hyperedges ususlly mean that
    an effect transmitted through one hyperedge logically cannot be transmitted
    through the other.
   */
  friend auto
  adjacent(const undirected_hyperedge& a, const undirected_hyperedge& b)
    -> bool;

private:
  std::vector<VertexType> verts_;

  friend struct edge_relabel<undirected_hyperedge>;
  friend struct std::hash<undirected_hyperedge>;
  friend struct hll::hash<undirected_hyperedge>;

  friend struct std::formatter<undirected_hyperedge, char>;
};

auto effect_cmp(const undirected_hyperedge& a, const undirected_hyperedge& b)
  -> bool;
auto adjacent(const undirected_hyperedge& a, const undirected_hyperedge& b)
  -> bool;

template <>
struct is_instantaneous<undirected_hyperedge> {
  static constexpr bool value = true;
};

template <>
struct is_undirected<undirected_hyperedge> {
  static constexpr bool value = true;
};

template <>
struct is_dyadic<undirected_hyperedge> {
  static constexpr bool value = false;
};

template <std::ranges::input_range R>
  requires std::convertible_to<std::ranges::range_value_t<R>, VertexType>
undirected_hyperedge::undirected_hyperedge(R&& verts) {
  if constexpr (std::ranges::sized_range<R>)
    verts_.reserve(std::ranges::size(verts));
  std::ranges::copy(std::forward<R>(verts), std::back_inserter(verts_));
  std::ranges::sort(verts_);
  auto [first, last] = std::ranges::unique(verts_);
  verts_.erase(first, last);
  verts_.shrink_to_fit();
}

template <>
struct edge_relabel<undirected_hyperedge> {
  template <std::invocable<VertexType> MapF>
    requires std::convertible_to<
      std::invoke_result_t<MapF, VertexType>, VertexType>
  auto operator()(const undirected_hyperedge& e, MapF&& map_fun) const
    -> undirected_hyperedge {
    return undirected_hyperedge{e.verts_ | std::views::transform(map_fun)};
  }
};
} // namespace reticula

namespace std {
template <>
struct hash<reticula::undirected_hyperedge> {
  auto operator()(const reticula::undirected_hyperedge& e) const
    -> std::size_t {
    return reticula::utils::combine_hash<reticula::VertexType, reticula::hash>(
      reticula::hash<reticula::VertexType>{}(e.verts_[0]), e.verts_[1]);
  }
};

template <>
struct formatter<reticula::undirected_hyperedge, char> {
  constexpr auto parse(format_parse_context& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it == '}')
      return it;
    throw format_error("Invalid format for reticula::undirected_hyperedge");
  }

  template <typename FormatContext>
  auto
  format(const reticula::undirected_hyperedge& e, FormatContext& ctx) const {
    return std::format_to(
      ctx.out(), "({{{}}})", reticula::utils::join(e.verts_, ", "));
  }
};
} // namespace std

namespace hll {
template <>
struct hash<reticula::undirected_hyperedge> {
  auto
  operator()(const reticula::undirected_hyperedge& e, std::uint64_t seed) const
    -> uint64_t {
    return hll::hash<std::size_t>{}(
      std::hash<reticula::undirected_hyperedge>{}(e), seed);
  }
};
} // namespace hll
