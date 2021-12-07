#include "../include/dag/algorithms.hpp"

namespace dag {
  template <network_vertex VertT>
  undirected_network<VertT> square_grid_graph(
      VertT side, size_t dims, bool periodic) {
    auto g = path_graph<VertT>(side, periodic);
    for (size_t i = 1; i < dims; i++)
      g = relabel_nodes<VertT>(
          cartesian_product(g, path_graph<VertT>(side, periodic)));

    return g;
  }

  template <network_vertex VertT>
  undirected_network<VertT> path_graph(VertT size, bool periodic) {
    static_assert(std::is_integral<VertT>::value,
        "vertices should be of integral type");
    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(size);

    for (VertT i = {}; i < size-1; i++)
      edges.emplace_back(i, i+1);

    if (periodic)
      edges.emplace_back(size - 1, 0);

    return undirected_network<VertT>(edges);
  }
}  // namespace dag
