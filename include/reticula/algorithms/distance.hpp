#pragma once

#include <cstddef>
#include <optional>
#include <unordered_map>

#include <reticula/concepts.hpp>
#include <reticula/utils.hpp>

namespace reticula {
template <network_like NetT>
[[nodiscard]] auto shortest_path_lengths_from(
  const NetT& net, VertexType source, std::size_t size_hint = 0)
  -> std::unordered_map<VertexType, std::size_t>;

template <network_like NetT>
[[nodiscard]] auto shortest_path_lengths_to(
  const NetT& net, VertexType destination, std::size_t size_hint = 0)
  -> std::unordered_map<VertexType, std::size_t>;

// Diameter

template <network_like NetT>
[[nodiscard]] auto try_diameter(const NetT& net) -> std::optional<std::size_t>;

template <network_like NetT>
[[nodiscard]] auto diameter(const NetT& net) -> std::size_t;

// Eccentricity

template <network_like NetT>
[[nodiscard]] auto try_out_eccentricity(const NetT& net, VertexType v)
  -> std::optional<std::size_t>;

template <network_like NetT>
[[nodiscard]] auto out_eccentricity(const NetT& net, VertexType v)
  -> std::size_t;

template <network_like NetT>
[[nodiscard]] auto try_in_eccentricity(const NetT& net, VertexType v)
  -> std::optional<std::size_t>;

template <network_like NetT>
[[nodiscard]] auto in_eccentricity(const NetT& net, VertexType v)
  -> std::size_t;

template <network_like NetT>
  requires is_undirected_v<NetT>
[[nodiscard]] auto try_eccentricity(const NetT& net, VertexType v)
  -> std::optional<std::size_t>;

template <network_like NetT>
  requires is_undirected_v<NetT>
[[nodiscard]] auto eccentricity(const NetT& net, VertexType v) -> std::size_t;

// Radius

template <network_like NetT>
[[nodiscard]] auto try_out_radius(const NetT& net)
  -> std::optional<std::size_t>;

template <network_like NetT>
[[nodiscard]] auto out_radius(const NetT& net) -> std::size_t;

template <network_like NetT>
[[nodiscard]] auto try_in_radius(const NetT& net) -> std::optional<std::size_t>;

template <network_like NetT>
[[nodiscard]] auto in_radius(const NetT& net) -> std::size_t;

template <network_like NetT>
  requires is_undirected_v<NetT>
[[nodiscard]] auto try_radius(const NetT& net) -> std::optional<std::size_t>;

template <network_like NetT>
  requires is_undirected_v<NetT>
[[nodiscard]] auto radius(const NetT& net) -> std::size_t;
} // namespace reticula

#include <algorithm>
#include <functional>
#include <ranges>
#include <vector>

#include "reticula/algorithms/detail/bfs.hpp"

namespace reticula {
template <network_like NetT>
auto shortest_path_lengths_from(
  const NetT& net, VertexType source, std::size_t size_hint)
  -> std::unordered_map<VertexType, std::size_t> {
  std::unordered_map<VertexType, std::size_t> lengths{{source, 0}};
  lengths.reserve(size_hint);
  detail::breadth_first_search<detail::breadth_first_search_direction::forward>(
    net, source,
    [&lengths](VertexType u, const NetT::EdgeType&, VertexType v) {
      lengths.try_emplace(v, lengths.at(u) + 1);
      return true;
    },
    size_hint);
  return lengths;
}

template <network_like NetT>
auto shortest_path_lengths_to(
  const NetT& net, VertexType destination, std::size_t size_hint)
  -> std::unordered_map<VertexType, std::size_t> {
  std::unordered_map<VertexType, std::size_t> lengths{{destination, 0}};
  lengths.reserve(size_hint);
  detail::breadth_first_search<
    detail::breadth_first_search_direction::backward>(
    net, destination,
    [&lengths](VertexType u, const NetT::EdgeType&, VertexType v) {
      lengths.try_emplace(v, lengths.at(u) + 1);
      return true;
    },
    size_hint);
  return lengths;
}

template <network_like NetT>
auto try_diameter(const NetT& net) -> std::optional<std::size_t> {
  if (std::ranges::empty(net.vertices()))
    return std::nullopt;
  auto verts = net.vertices();
  const auto s = verts.front();

  auto lengths_s = shortest_path_lengths_from(net, s, verts.size());
  if (lengths_s.size() < verts.size())
    return std::nullopt;
  if constexpr (is_directed_v<NetT>) {
    auto lengths_s_rev = shortest_path_lengths_to(net, s, verts.size());
    if (lengths_s_rev.size() < verts.size())
      return std::nullopt;
  }

  auto max_length_s = std::ranges::max_element(
    lengths_s, {}, [](const auto& pair) { return pair.second; });
  const auto a = max_length_s->first;

  auto lengths_a = shortest_path_lengths_from(net, a, verts.size());
  if (lengths_a.size() < verts.size())
    return std::nullopt;

  auto max_length_a = std::ranges::max_element(
    lengths_a, {}, [](const auto& pair) { return pair.second; });
  const auto b = max_length_a->first;
  auto lower_bound = max_length_a->second;

  auto layer_forwards = shortest_path_lengths_from(net, b, verts.size());
  std::unordered_map<VertexType, std::size_t> layer_backwards;
  if constexpr (is_directed_v<NetT>)
    layer_backwards = shortest_path_lengths_to(net, b, verts.size());

  auto maximum_value = [](const auto& d) -> std::size_t {
    return std::ranges::max_element(
             d, {}, [](const auto& pair) { return pair.second; })
      ->second;
  };

  if constexpr (is_directed_v<NetT>)
    lower_bound = std::max(
      {lower_bound, maximum_value(layer_forwards),
       maximum_value(layer_backwards)});
  else
    lower_bound = std::max(lower_bound, maximum_value(layer_forwards));

  std::vector<VertexType> order_forwards(verts.begin(), verts.end());
  std::ranges::sort(
    order_forwards, std::ranges::greater{},
    [&layer_forwards](VertexType a) { return layer_forwards.at(a); });

  std::vector<VertexType> order_backwards;
  if constexpr (is_directed_v<NetT>) {
    order_backwards.reserve(verts.size());
    std::ranges::copy(order_forwards, std::back_inserter(order_backwards));
    std::ranges::sort(
      order_backwards, std::ranges::greater{},
      [&layer_backwards](VertexType a) { return layer_backwards.at(a); });
  }

  auto upper_bound_at = [&](std::size_t i) -> std::size_t {
    if constexpr (is_directed_v<NetT>)
      return std::max<std::size_t>(
        layer_forwards.at(order_forwards[i]),
        layer_backwards.at(order_backwards[i]));
    else
      return 2 * layer_forwards.at(order_forwards[i]);
  };

  std::size_t i = 0;
  std::size_t upper_bound = upper_bound_at(i);

  while (i < verts.size() && lower_bound < upper_bound) {
    const auto x = order_forwards[i];
    auto d_to_x = shortest_path_lengths_to(net, x, verts.size());
    lower_bound = std::max(lower_bound, maximum_value(d_to_x));

    if constexpr (is_directed_v<NetT>) {
      auto y = order_backwards[i];
      auto d_from_y = shortest_path_lengths_from(net, y, verts.size());

      lower_bound =
        std::max({lower_bound, maximum_value(d_to_x), maximum_value(d_from_y)});
    }

    ++i;

    if (i < verts.size())
      upper_bound = upper_bound_at(i);
  }

  return lower_bound;
}

template <network_like NetT>
auto diameter(const NetT& net) -> std::size_t {
  auto maybe_diameter = try_diameter(net);
  if (maybe_diameter)
    return *maybe_diameter;
  else
    throw distance_undefined_error(
      "the network is not (strongly) connected or is empty");
}

template <network_like NetT>
auto try_out_eccentricity(const NetT& net, VertexType v)
  -> std::optional<std::size_t> {
  auto verts = net.vertices();

  if (std::ranges::empty(net.vertices()))
    return std::nullopt;

  auto lengths = shortest_path_lengths_from(net, v, verts.size());
  if (lengths.size() < verts.size())
    return std::nullopt;

  return std::ranges::max_element(
           lengths, {}, [](const auto& pair) { return pair.second; })
    ->second;
}

template <network_like NetT>
auto out_eccentricity(const NetT& net, VertexType v) -> std::size_t {
  auto maybe_eccentricity = try_out_eccentricity(net, v);
  if (maybe_eccentricity)
    return *maybe_eccentricity;
  else
    throw distance_undefined_error(
      "all other vertices are not reachable from the vertex");
}

template <network_like NetT>
auto try_in_eccentricity(const NetT& net, VertexType v)
  -> std::optional<std::size_t> {
  auto verts = net.vertices();

  if (std::ranges::empty(net.vertices()))
    return std::nullopt;

  auto lengths = shortest_path_lengths_to(net, v, verts.size());
  if (lengths.size() < verts.size())
    return std::nullopt;

  return std::ranges::max_element(
           lengths, {}, [](const auto& pair) { return pair.second; })
    ->second;
}

template <network_like NetT>
auto in_eccentricity(const NetT& net, VertexType v) -> std::size_t {
  auto maybe_eccentricity = try_in_eccentricity(net, v);
  if (maybe_eccentricity)
    return *maybe_eccentricity;
  else
    throw distance_undefined_error(
      "the vertex is not reachable from all other vertices");
}

template <network_like NetT>
  requires is_undirected_v<NetT>
auto try_eccentricity(const NetT& net, VertexType v)
  -> std::optional<std::size_t> {
  return try_out_eccentricity(net, v);
}

template <network_like NetT>
  requires is_undirected_v<NetT>
auto eccentricity(const NetT& net, VertexType v) -> std::size_t {
  return out_eccentricity(net, v);
}

namespace detail {
template <breadth_first_search_direction Dir, network_like NetT>
auto directional_radius(const NetT& net) -> std::optional<std::size_t> {
  auto verts = net.vertices();
  if (std::ranges::empty(verts))
    return std::nullopt;

  if (verts.size() == 1)
    return 0uz;

  auto bounded_dist = [&net, &verts](VertexType source, std::size_t threshold)
    -> std::pair<std::size_t, std::size_t> {
    std::unordered_map<VertexType, std::size_t> dists{{source, 0}};
    dists.reserve(verts.size());
    std::size_t max_d = 0;
    detail::breadth_first_search<Dir>(
      net, source,
      [&](VertexType u, const NetT::EdgeType&, VertexType v) {
        auto dist_u = dists.at(u);
        if (dist_u >= threshold)
          return false;

        auto [it, inserted] = dists.try_emplace(v, dist_u + 1);
        if (inserted && dist_u + 1 > max_d)
          max_d = dist_u + 1;
        return true;
      },
      verts.size());
    return {max_d, dists.size()};
  };

  bool found = false;
  auto best = verts.size() - 1;
  for (const auto& v : verts) {
    auto [ecc, reached] = bounded_dist(v, best);
    if (reached == verts.size()) {
      if (best == 0)
        return 0uz;

      found = true;
      if (ecc < best)
        best = ecc;
    }
  }

  if (!found)
    return std::nullopt;

  return best;
}
} // namespace detail

template <network_like NetT>
auto try_out_radius(const NetT& net) -> std::optional<std::size_t> {
  return detail::directional_radius<
    detail::breadth_first_search_direction::forward>(net);
}

template <network_like NetT>
auto out_radius(const NetT& net) -> std::size_t {
  auto maybe_radius = try_out_radius(net);
  if (maybe_radius)
    return *maybe_radius;
  else
    throw distance_undefined_error(
      "no single vertex can reach all other vertices");
}

template <network_like NetT>
auto try_in_radius(const NetT& net) -> std::optional<std::size_t> {
  return detail::directional_radius<
    detail::breadth_first_search_direction::backward>(net);
}

template <network_like NetT>
auto in_radius(const NetT& net) -> std::size_t {
  auto maybe_radius = try_in_radius(net);
  if (maybe_radius)
    return *maybe_radius;
  else
    throw distance_undefined_error(
      "no single vertex can be reached from all other vertices");
}

template <network_like NetT>
  requires is_undirected_v<NetT>
auto try_radius(const NetT& net) -> std::optional<std::size_t> {
  return try_out_radius(net);
}

template <network_like NetT>
  requires is_undirected_v<NetT>
auto radius(const NetT& net) -> std::size_t {
  auto maybe_radius = try_radius(net);
  if (maybe_radius)
    return *maybe_radius;
  else
    throw distance_undefined_error(
      "no single vertex can reach all other vertices or be reached from them");
}
} // namespace reticula
