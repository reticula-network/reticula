#ifndef INCLUDE_DAG_ALGORITHMS_HPP_
#define INCLUDE_DAG_ALGORITHMS_HPP_

#include <vector>
#include <unordered_set>

#include "network_concepts.hpp"
#include "components.hpp"
#include "adjacency_prob.hpp"

namespace dag {
  template <static_edge EdgeT, typename DiscoveryF>
  component<typename EdgeT::VertexType>
  breadth_first_search(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert,
      DiscoveryF discovered,
      std::size_t size_hint = 0);

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  directed_network<EdgeT>
  event_graph(
      const network<EdgeT>& temp,
      const AdjacencyProbT& prob,
      std::size_t seed);

  /**
    Sorts the vertices of `dir` in a way that for every directed edge in that
    graph, the head node appears after the tail node in the result.

    @param dir Directed network in question
  */
  template <network_vertex VertT>
  std::vector<VertT>
  topological_order(
      const directed_network<VertT>& dir);


  // out-components:


  /**
    Returns component of the graph `dir` that can be reached from `root` by
    traversing through a sequence of adjacent vertices.

    @param dir Directed network in question
    @param root The destination vert
  */
  template <network_vertex VertT>
  component<VertT>
  out_component(
      const directed_network<VertT>& dir,
      const VertT& root,
      std::size_t size_hint = 0);

  /**
    Returns component of the graph `dir` that can be reached from each node by
    traversing through a sequence of adjacent vertices.

    @param dir Directed network in question
  */
  template <network_vertex VertT>
  std::vector<std::pair<VertT, component<VertT>>>
  out_components(
      const directed_network<VertT>& dir);

  /**
    Returns the size of the component of the graph `dir` that can be reached
    from each node by traversing through a sequence of adjacent vertices.

    @param dir Directed network in question
  */
  template <network_vertex VertT>
  std::vector<std::pair<VertT, component_size<VertT>>>
  out_component_sizes(
      const directed_network<VertT>& dir);

  /**
    Returns an estimate of the size of the component of the graph `dir` that
    can be reached from each node by traversing through a sequence of adjacent
    vertices.

    @param dir Directed network in question
  */
  template <network_vertex VertT>
  std::vector<std::pair<VertT, component_size_estimate<VertT>>>
  out_component_size_estimates(
      const directed_network<VertT>& dir);


  // in-components:


  /**
    Returns component of the graph `dir` that can reach `root` by traversing
    through a sequence of adjacent vertices.

    @param dir Directed network in question
    @param root The destination vert
  */
  template <network_vertex VertT>
  component<VertT>
  in_component(
      const directed_network<VertT>& dir,
      const VertT& root,
      std::size_t size_hint = 0);


  /**
    Returns component of the graph `dir` that can reach each of the nodes by
    traversing through a sequence of adjacent vertices.

    @param dir Directed network in question
  */
  template <network_vertex VertT>
  std::vector<std::pair<VertT, component<VertT>>>
  in_components(
      const directed_network<VertT>& dir);

  /**
    Returns the size of the component of the graph `dir` that can reach
    each of the nodes by traversing through a sequence of adjacent vertices.

    @param dir Directed network in question
  */
  template <network_vertex VertT>
  std::vector<std::pair<VertT, component_size<VertT>>>
  in_component_sizes(
      const directed_network<VertT>& dir);

  /**
    Returns an estimate of the size of the component of the graph `dir` that
    can reach each of the nodes by traversing through a sequence of adjacent
    vertices.

    @param dir Directed network in question
  */
  template <network_vertex VertT>
  std::vector<std::pair<VertT, component_size_estimate<VertT>>>
  in_component_size_estimates(
      const directed_network<VertT>& dir);


  /**
    Returns list of all weakly connected components of `dir`.

    @param dir Directed network in question
    @param singletons If true, also returns components with only one members.
  */
  template <network_vertex VertT>
  std::vector<component<VertT>>
  weakly_connected_components(
      const directed_network<VertT>& dir,
      bool singletons = true);


  /**
    Returns the connected component of `net` that includes node `vert`.

    @param net An undirected Network
    @param vert A vertex that will belong to the final component
  */
  template <network_vertex VertT>
  component<VertT>
  connected_component(
      const undirected_network<VertT>& net,
      const VertT& vert);

  /**
    Returns list of all connected components of `net`.

    @param net Directed network in question
    @param singletons If true, also returns components with only one members.
  */
  template <network_vertex VertT>
  std::vector<component<VertT>>
  connected_components(
      const undirected_network<VertT>& net,
      bool singletons = true);


  template <network_vertex VertT1, network_vertex VertT2>
  undirected_network<std::pair<VertT1, VertT2>>
  cartesian_product(
      const undirected_network<VertT1>& g1,
      const undirected_network<VertT2>& g2);

  template <integer_vertex OutVertT, network_vertex InVertT>
  undirected_network<OutVertT>
  relabel_nodes(const undirected_network<InVertT>& g);

  /**
    Checks if the sequence can be the degree sequence of a valid undirected
    graph, containing no multi-edges or loops, based on the Erdős--Gallai
    algorithm.

    Erdős, Paul, and Tibor Gallai. "Graphs with prescribed degrees of vertices."
    Mat. Lapok 11.264-274 (1960): 15.

    @param sequence The sequence in question, a forward range of VertT values.
  */
  template <std::ranges::forward_range Range>
  requires degree_range<Range>
  bool is_graphic(const Range& degree_sequence);

  /**
    Checks if the sequence can be the degree sequence of a valid directed graph,
    containing no multi-edges or loops, based on the algorithm by Kleitman and
    Wang.

    Kleitman, Daniel J., and Da-Lun Wang. "Algorithms for constructing graphs
    and digraphs with given valences and factors." Discrete Mathematics 6.1
    (1973): 79-88.

    @param sequence The sequence in question, a forward range of VertT values.
  */
  template <std::ranges::input_range PairRange>
  requires degree_pair_range<PairRange>
  bool is_digraphic(const PairRange& in_out_degree_sequence);
}  // namespace dag

#include "../../src/algorithms.tpp"

#endif  // INCLUDE_DAG_ALGORITHMS_HPP_
