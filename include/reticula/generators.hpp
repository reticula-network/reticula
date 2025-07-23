#ifndef INCLUDE_RETICULA_GENERATORS_HPP_
#define INCLUDE_RETICULA_GENERATORS_HPP_

#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  template <integer_network_vertex VertT>
  undirected_network<VertT> square_grid_graph(
      VertT side, size_t dims, bool periodic = false);

  template <integer_network_vertex VertT>
  undirected_network<VertT> path_graph(VertT size, bool periodic = false);

  template <integer_network_vertex VertT>
  undirected_network<VertT> cycle_graph(VertT size);

  template <integer_network_vertex VertT>
  undirected_network<VertT> regular_ring_lattice(VertT size, VertT degree);

  template <integer_network_vertex VertT>
  undirected_network<VertT> complete_graph(VertT size);

  template <integer_network_vertex VertT>
  directed_network<VertT> complete_directed_graph(VertT size);
}  // namespace reticula

// Implementation
#include "operations.hpp"

namespace reticula {
  template <integer_network_vertex VertT>
  undirected_network<VertT> square_grid_graph(
      VertT side, std::size_t dims, bool periodic) {
    auto g = path_graph<VertT>(side, periodic);
    for (std::size_t i = 1; i < dims; i++)
      g = relabel_nodes<VertT>(
          cartesian_product(g, path_graph<VertT>(side, periodic)));

    return g;
  }

  template <integer_network_vertex VertT>
  undirected_network<VertT> path_graph(VertT size, bool periodic) {
    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(size);

    for (VertT i = {}; i < size-1; i++)
      edges.emplace_back(i, i+1);

    if (periodic)
      edges.emplace_back(size - 1, 0);

    return undirected_network<VertT>(edges);
  }

  template <integer_network_vertex VertT>
  undirected_network<VertT> cycle_graph(VertT size) {
    return path_graph(size, true);
  }

  template <integer_network_vertex VertT>
  undirected_network<VertT> regular_ring_lattice(VertT size, VertT degree) {
    if (degree >= size)
      throw std::invalid_argument("degree should be smaller than size");
    if (degree % 2 != 0)
      throw std::invalid_argument("degree should be divisible by two");

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(size * degree / 2));

    for (VertT i = {}; i < size; i++)
      for (VertT j = i + 1; j <= i + degree/2; j++)
        edges.emplace_back(i, j % size);

    return undirected_network<VertT>(edges);
  }

  template <integer_network_vertex VertT>
  undirected_network<VertT> complete_graph(VertT size) {
    std::vector<undirected_edge<VertT>> edges;
    if (size > 0)
      edges.reserve(static_cast<std::size_t>((size*(size-1))/2));

    for (VertT i = {}; i < size; i++)
      for (VertT j = {}; j < i; j++)
        edges.emplace_back(i, j);

    return undirected_network<VertT>(edges);
  }

  template <integer_network_vertex VertT>
  directed_network<VertT> complete_directed_graph(VertT size) {
    std::vector<directed_edge<VertT>> edges;
    if (size > 0)
      edges.reserve(static_cast<std::size_t>(size*(size-1)));

    for (VertT i = {}; i < size; i++)
      for (VertT j = {}; j < size; j++)
        if (i != j) edges.emplace_back(i, j);

    return directed_network<VertT>(edges);
  }
}  // namespace reticula


#endif  // INCLUDE_RETICULA_GENERATORS_HPP_

