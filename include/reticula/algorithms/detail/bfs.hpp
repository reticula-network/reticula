#pragma once

#include <cstddef>
#include <queue>

#include <reticula/components.hpp>
#include <reticula/concepts.hpp>

namespace reticula::detail {
enum struct breadth_first_search_direction { forward, backward, undirected };

/**
  Generic breadth-first search algorithm for networks.
  @tparam Dir The search direction (forward, backward, or undirected).
  @param net The network to search.
  @param vert The starting vertex for the search.
  @param discovered A function that is called when a new vertex is discovered.
                    It should return true to continue the search, or false to
                    stop it.
  @param size_hint An optional hint for the size of the discovered component.
  @return A component containing all vertices discovered during the search.
 */
template <
  breadth_first_search_direction Dir, network_like NetT, typename DiscoveryF>
auto breadth_first_search(
  const NetT& net, VertexType vert, DiscoveryF discovered,
  std::size_t size_hint = 0) -> component {

  component discovered_comp(size_hint);
  discovered_comp.insert(vert);
  std::queue<VertexType> queue;
  queue.push(vert);

  auto visit = [&queue, &discovered_comp, &discovered](
                 VertexType from, const NetT::EdgeType& e,
                 VertexType to) -> bool {
    if (!discovered_comp.contains(to)) {
      discovered_comp.insert(to);
      if (!discovered(from, e, to))
        return false;
      queue.push(to);
    }
    return true;
  };

  auto edges_out = [&](VertexType v) {
    if constexpr (Dir == breadth_first_search_direction::undirected)
      return net.incident_edges(v);
    else if constexpr (Dir == breadth_first_search_direction::backward)
      return net.in_edges(v);
    else // forward
      return net.out_edges(v);
  };

  auto edge_verts_out = [&](const NetT::EdgeType& e) {
    if constexpr (Dir == breadth_first_search_direction::undirected)
      return e.incident_verts();
    else if constexpr (Dir == breadth_first_search_direction::backward)
      return e.mutator_verts();
    else // forward
      return e.mutated_verts();
  };

  while (!queue.empty()) {
    auto v = queue.front();
    queue.pop();

    for (const auto& e : edges_out(v))
      for (auto w : edge_verts_out(e))
        if (w != v && !visit(v, e, w))
          return discovered_comp;
  }

  return discovered_comp;
}
} // namespace reticula::detail
