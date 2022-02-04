#ifndef INCLUDE_DAG_GENERATORS_HPP_
#define INCLUDE_DAG_GENERATORS_HPP_

#include <numeric>

#include "network_concepts.hpp"

namespace dag {
  template <integer_vertex VertT>
  undirected_network<VertT> square_grid_graph(
      VertT side, size_t dims, bool periodic = false);

  template <integer_vertex VertT>
  undirected_network<VertT> path_graph(VertT size, bool periodic = false);

  template <integer_vertex VertT>
  undirected_network<VertT> cycle_graph(VertT size);

  template <integer_vertex VertT>
  undirected_network<VertT> regular_ring_lattice(VertT size, VertT degree);

  template <integer_vertex VertT>
  undirected_network<VertT> complete_graph(VertT size);

  template <integer_vertex VertT>
  directed_network<VertT> complete_directed_graph(VertT size);
}  // namespace dag

#include "../../src/generators.tpp"

#endif  // INCLUDE_DAG_GENERATORS_HPP_

