#pragma once

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
auto path_graph(VertexType size, bool periodic = false) -> undirected_network;

auto cycle_graph(VertexType size) -> undirected_network;

auto star_graph(VertexType size) -> undirected_network;

auto wheel_graph(VertexType size) -> undirected_network;

auto complete_bipartite_graph(VertexType size_a, VertexType size_b)
  -> undirected_network;

auto complete_graph(VertexType size) -> undirected_network;

auto complete_bipartite_directed_graph(VertexType size_a, VertexType size_b)
  -> directed_network;

auto complete_directed_graph(VertexType size) -> directed_network;

auto regular_ring_lattice(VertexType size, VertexType degree)
  -> undirected_network;

auto square_grid_graph(
  std::vector<VertexType> dimensions, bool periodic = false)
  -> undirected_network;
} // namespace reticula
