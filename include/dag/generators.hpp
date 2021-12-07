#ifndef INCLUDE_DAG_GENERATORS_HPP_
#define INCLUDE_DAG_GENERATORS_HPP_

#include "network_concepts.hpp"

namespace dag {
  template <network_vertex VertT>
  undirected_network<VertT> square_grid_graph(
      VertT side, size_t dims, bool periodic = false);

  template <network_vertex VertT>
  undirected_network<VertT> path_graph(VertT size, bool periodic = false);
}  // namespace dag

#include "../../src/generators.tpp"

#endif  // INCLUDE_DAG_GENERATORS_HPP_

