#ifndef INCLUDE_DAG_ALGORITHMS_HPP_
#define INCLUDE_DAG_ALGORITHMS_HPP_

#include <vector>
#include <ranges>
#include <unordered_set>
#include <optional>

#include "network_concepts.hpp"
#include "networks.hpp"
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
    Returns true if the directed graph contains no cycles. This is detemined by
    trying to find a topologial ordering or the edges. If you intend to get a
    topological ordering of the nodes anyway, use `dag::try_topological_order`.

    @param dir Directed network in question
  */
  template <static_directed_edge EdgeT>
  bool is_acyclic(
      const network<EdgeT>& dir);

  /**
    Tries to sorts the vertices of `dir` in a way that for every directed edge
    in that graph, the head node appears after the tail node in the result.

    Returns std::nullopt in if the graph has cycles.

    @param dir Directed network in question
  */
  template <static_directed_edge EdgeT>
  std::optional<std::vector<typename EdgeT::VertexType>>
  try_topological_order(
      const network<EdgeT>& dir);

  /**
    Sorts the vertices of `dir` in a way that for every directed edge in that
    graph, the head node appears after the tail node in the result.

    Throws utils::not_acyclic_error if the graph has cycles.

    @param dir Directed network in question
  */
  template <static_directed_edge EdgeT>
  std::vector<typename EdgeT::VertexType>
  topological_order(
      const network<EdgeT>& dir);


  // out-components:


  /**
    Returns component of the graph `dir` that can be reached from `root` by
    traversing through a sequence of adjacent vertices.

    @param dir Directed network in question
    @param root The destination vert
  */
  template <static_directed_edge EdgeT>
  component<typename EdgeT::VertexType>
  out_component(
      const network<EdgeT>& dir,
      const typename EdgeT::VertexType& root,
      std::size_t size_hint = 0);

  /**
    Returns component of the graph `dir` that can be reached from each node by
    traversing through a sequence of adjacent vertices.

    @param dir Directed network in question
  */
  template <static_directed_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component<typename EdgeT::VertexType>>>
  out_components(
      const network<EdgeT>& dir);

  /**
    Returns the size of the component of the graph `dir` that can be reached
    from each node by traversing through a sequence of adjacent vertices.

    @param dir Directed network in question
  */
  template <static_directed_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size<typename EdgeT::VertexType>>>
  out_component_sizes(
      const network<EdgeT>& dir);

  /**
    Returns an estimate of the size of the component of the graph `dir` that
    can be reached from each node by traversing through a sequence of adjacent
    vertices.

    @param dir Directed network in question
  */
  template <static_directed_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size_estimate<typename EdgeT::VertexType>>>
  out_component_size_estimates(
      const network<EdgeT>& dir);


  // in-components:


  /**
    Returns component of the graph `dir` that can reach `root` by traversing
    through a sequence of adjacent vertices.

    @param dir Directed network in question
    @param root The destination vert
  */
  template <static_directed_edge EdgeT>
  component<typename EdgeT::VertexType>
  in_component(
      const network<EdgeT>& dir,
      const typename EdgeT::VertexType& root,
      std::size_t size_hint = 0);


  /**
    Returns component of the graph `dir` that can reach each of the nodes by
    traversing through a sequence of adjacent vertices.

    @param dir Directed network in question
  */
  template <static_directed_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component<typename EdgeT::VertexType>>>
  in_components(
      const network<EdgeT>& dir);

  /**
    Returns the size of the component of the graph `dir` that can reach
    each of the nodes by traversing through a sequence of adjacent vertices.

    @param dir Directed network in question
  */
  template <static_directed_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size<typename EdgeT::VertexType>>>
  in_component_sizes(
      const network<EdgeT>& dir);

  /**
    Returns an estimate of the size of the component of the graph `dir` that
    can reach each of the nodes by traversing through a sequence of adjacent
    vertices.

    @param dir Directed network in question
  */
  template <static_directed_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size_estimate<typename EdgeT::VertexType>>>
  in_component_size_estimates(
      const network<EdgeT>& dir);

  /**
    Returns the weakly connected components of `dir` containing vertex `vert`.

    @param dir Directed network in question
    @param `vert` A vertex that belongs to the returned component.
  */
  template <static_directed_edge EdgeT>
  component<typename EdgeT::VertexType>
  weakly_connected_component(
      const network<EdgeT>& dir,
      const typename EdgeT::VertexType& vert,
      std::size_t size_hint = 0);

  /**
    Returns list of all weakly connected components of `dir`.

    @param dir Directed network in question
    @param singletons If true, also returns components with only one members.
  */
  template <static_directed_edge EdgeT>
  std::vector<component<typename EdgeT::VertexType>>
  weakly_connected_components(
      const network<EdgeT>& dir,
      bool singletons = true);

  /**
    Returns true if all vertices in `dir` belong to the same weakly connected
    component.

    @param dir A directed network
  */
  template <static_directed_edge EdgeT>
  bool is_weakly_connected(const network<EdgeT>& dir);

  /**
    Returns the connected component of `net` that includes node `vert`.

    @param net An undirected Network
    @param vert A vertex that will belong to the final component
  */
  template <static_undirected_edge EdgeT>
  component<typename EdgeT::VertexType>
  connected_component(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert,
      std::size_t size_hint = 0);

  /**
    Returns list of all connected components of `net`.

    @param net An undirected Network
    @param singletons If true, also returns components with only one members.
  */
  template <static_undirected_edge EdgeT>
  std::vector<component<typename EdgeT::VertexType>>
  connected_components(
      const network<EdgeT>& net,
      bool singletons = true);

  /**
    Returns true if all all vertices in `net` belong to a single connected
    component.

    @param net An undirected Network
  */
  template <static_undirected_edge EdgeT>
  bool is_connected(const network<EdgeT>& net);


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

  /**
    Finds the range of times present in the events. If temporal edges have
    different cause and effect times (i.e. they have delays) this would be
    equivalent to minimum cause time and maximum effect time.
  */
  template <temporal_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  time_window(const network<EdgeT>& temp);

  /**
    Finds the range of cause times present in the events. If temporal edges have
    don't haver different cause and effect times (i.e. they are not delayed)
    this would be equivalent to time_window.
  */
  template <temporal_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  cause_time_window(const network<EdgeT>& temp);

  /**
    Finds the range of effect times present in the events. If temporal edges
    have don't haver different cause and effect times (i.e. they are not
    delayed) this would be equivalent to time_window.
  */
  template <temporal_edge EdgeT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  effect_time_window(const network<EdgeT>& temp);
}  // namespace dag

#include "../../src/algorithms.tpp"

#endif  // INCLUDE_DAG_ALGORITHMS_HPP_
