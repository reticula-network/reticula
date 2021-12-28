#include "../include/dag/algorithms.hpp"

namespace dag {
  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> square_grid_graph(
      VertT side, std::size_t dims, bool periodic) {
    auto g = path_graph<VertT>(side, periodic);
    for (std::size_t i = 1; i < dims; i++)
      g = relabel_nodes<VertT>(
          cartesian_product(g, path_graph<VertT>(side, periodic)));

    return g;
  }

  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> path_graph(VertT size, bool periodic) {
    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(size);

    for (VertT i = {}; i < size-1; i++)
      edges.emplace_back(i, i+1);

    if (periodic)
      edges.emplace_back(size - 1, 0);

    return undirected_network<VertT>(edges);
  }

  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> cycle_graph(VertT size) {
    return path_graph(size, true);
  }

  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> complete_graph(VertT size) {
    std::vector<undirected_edge<VertT>> edges;
    edges.reserve((size*(size-1))/2);

    for (VertT i = {}; i < size; i++)
      for (VertT j = {}; j < i; j++)
        edges.emplace_back(i, j);

    return undirected_network<VertT>(edges);
  }

  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> regular_ring_lattice(VertT size, VertT degree) {
    if (degree >= size)
      throw std::invalid_argument("degree should be smaller than size");
    if (degree % 2 != 0)
      throw std::invalid_argument("degree should be divisible by two");

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(size*degree/2);

    for (VertT i = {}; i < size; i++)
      for (VertT j = i + 1; j <= i + degree/2; j++)
        edges.emplace_back(i, j % size);

    return undirected_network<VertT>(edges);
  }
}  // namespace dag
