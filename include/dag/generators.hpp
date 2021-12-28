#ifndef INCLUDE_DAG_GENERATORS_HPP_
#define INCLUDE_DAG_GENERATORS_HPP_

#include <numeric>

#include "network_concepts.hpp"

namespace dag {
  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> square_grid_graph(
      VertT side, size_t dims, bool periodic = false);

  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> path_graph(VertT size, bool periodic = false);

  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> cycle_graph(VertT size);

  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> regular_ring_lattice(VertT size, VertT degree);

  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> complete_graph(VertT size);
}  // namespace dag

#include "../../src/generators.tpp"

#endif  // INCLUDE_DAG_GENERATORS_HPP_

