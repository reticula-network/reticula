#pragma once

#include <optional>
#include <utility>

#include <reticula/components.hpp>
#include <reticula/concepts.hpp>

namespace reticula {
template <network_like NetT>
[[nodiscard]] auto try_two_colouring(const NetT& net)
  -> std::optional<std::pair<component, component>>;

template <network_like NetT>
[[nodiscard]] auto two_colouring(const NetT& net)
  -> std::pair<component, component>;

template <network_like NetT>
[[nodiscard]] auto is_bipartite(const NetT& net) -> bool;
} // namespace reticula

#include <unordered_map>

#include "reticula/algorithms/detail/bfs.hpp"
#include <reticula/utils.hpp>

namespace reticula {
template <network_like NetT>
auto try_two_colouring(const NetT& net)
  -> std::optional<std::pair<component, component>> {
  component first, second;
  // TODO: use dense_id and bitmap
  std::unordered_map<VertexType, bool> colours;
  colours.reserve(net.vertices().size());

  auto assign = [&](VertexType from, const NetT::EdgeType&, VertexType to) {
    colours[to] = !colours[from];
    if (colours[to])
      second.insert(to);
    else
      first.insert(to);
    return true;
  };

  for (auto v : net.vertices()) {
    if (!colours.contains(v)) {
      colours[v] = false;
      first.insert(v);
      detail::breadth_first_search<
        detail::breadth_first_search_direction::forward>(net, v, assign);
    }
  }

  for (const auto& e : net.edges()) {
    auto verts = e.incident_verts();
    if (verts.size() < 2)
      return std::nullopt;
    for (std::size_t i = 0; i < verts.size(); ++i)
      for (std::size_t j = i + 1; j < verts.size(); ++j)
        if (colours[verts[i]] == colours[verts[j]])
          return std::nullopt;
  }

  return std::pair{first, second};
}

template <network_like NetT>
auto two_colouring(const NetT& net) -> std::pair<component, component> {
  auto maybe_colours = try_two_colouring(net);
  if (maybe_colours)
    return *maybe_colours;
  else
    throw not_bipartite_error("argument net most be bipartite");
}

template <network_like NetT>
auto is_bipartite(const NetT& net) -> bool {
  return try_two_colouring(net).has_value();
}
} // namespace reticula
