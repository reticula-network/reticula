#ifndef INCLUDE_DAG_ALGORITHMS_HPP_
#define INCLUDE_DAG_ALGORITHMS_HPP_

#include <vector>
#include <unordered_set>

#include "network_concepts.hpp"
#include "adjacency_prob.hpp"

namespace dag {
  template <static_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  directed_network<EdgeT>
  event_graph(
      const network<EdgeT>& temp,
      const AdjacencyProbT& prob,
      size_t seed);

  template <network_vertex VertT>
  std::vector<VertT>
  topological_order(
      const directed_network<VertT>& dir);

  template <network_vertex VertT>
  std::vector<VertT> out_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint = 0);

  template <network_vertex VertT>
  std::vector<VertT> in_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint = 0);


  /**
    Returns list of all weakly connected components of `dir`.

    @param dir Directed network in question
    @param singleton If true, return components with only one members.
   */
  template <network_vertex VertT>
  std::vector<std::vector<VertT>>
  weakly_connected_components(
      const directed_network<VertT>& dir,
      bool singleton = true);


  template <network_vertex VertT1, network_vertex VertT2>
  undirected_network<std::pair<VertT1, VertT2>>
  cartesian_product(
      const undirected_network<VertT1>& g1,
      const undirected_network<VertT2>& g2);

  template <network_vertex OutVertT, network_vertex InVertT>
  requires std::numeric_limits<OutVertT>::is_integer
  undirected_network<OutVertT>
  relabel_nodes(const undirected_network<InVertT>& g);

  /**
    Checks if the sequence can be degree sequence of a valid undirected graph,
    containing no multi-edges or loops, based on the Erdős--Gallai algorithm.

    @param sequence The sequence in question, a forward range of VertT values.
  */
  template <network_vertex VertT, std::ranges::forward_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, VertT>
  bool is_graphic(const Range& sequence);
}  // namespace dag

#include "../../src/algorithms.tpp"

#endif  // INCLUDE_DAG_ALGORITHMS_HPP_
