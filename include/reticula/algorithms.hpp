#ifndef INCLUDE_RETICULA_ALGORITHMS_HPP_
#define INCLUDE_RETICULA_ALGORITHMS_HPP_

#include <vector>
#include <unordered_set>
#include <optional>

#include "ranges.hpp"
#include "network_concepts.hpp"
#include "networks.hpp"
#include "components.hpp"

namespace reticula {
  template <static_network_edge EdgeT, typename DiscoveryF>
  component<typename EdgeT::VertexType>
  breadth_first_search(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert,
      DiscoveryF discovered,
      std::size_t size_hint = 0);


  /**
    Returns true if the directed graph contains no cycles. This is detemined by
    trying to find a topologial ordering or the edges. If you intend to get a
    topological ordering of the nodes anyway, use
    `reticula::try_topological_order`.

    @param dir Directed network in question
  */
  template <directed_static_network_edge EdgeT>
  bool is_acyclic(
      const network<EdgeT>& dir);

  /**
    Tries to sorts the vertices of `dir` in a way that for every directed edge
    in that graph, the head node appears after the tail node in the result.

    Returns std::nullopt in if the graph has cycles.

    @param dir Directed network in question
  */
  template <directed_static_network_edge EdgeT>
  std::optional<std::vector<typename EdgeT::VertexType>>
  try_topological_order(
      const network<EdgeT>& dir);

  /**
    Sorts the vertices of `dir` in a way that for every directed edge in that
    graph, the head node appears after the tail node in the result.

    Throws utils::not_acyclic_error if the graph has cycles.

    @param dir Directed network in question
  */
  template <directed_static_network_edge EdgeT>
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
  template <directed_static_network_edge EdgeT>
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
  template <directed_static_network_edge EdgeT>
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
  template <directed_static_network_edge EdgeT>
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
  template <directed_static_network_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size_estimate<typename EdgeT::VertexType>>>
  out_component_size_estimates(
      const network<EdgeT>& dir,
      std::size_t seed);


  // in-components:


  /**
    Returns component of the graph `dir` that can reach `root` by traversing
    through a sequence of adjacent vertices.

    @param dir Directed network in question
    @param root The destination vert
  */
  template <directed_static_network_edge EdgeT>
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
  template <directed_static_network_edge EdgeT>
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
  template <directed_static_network_edge EdgeT>
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
  template <directed_static_network_edge EdgeT>
  std::vector<std::pair<
    typename EdgeT::VertexType,
    component_size_estimate<typename EdgeT::VertexType>>>
  in_component_size_estimates(
      const network<EdgeT>& dir,
      std::size_t seed);

  /**
    Returns the weakly connected components of `dir` containing vertex `vert`.

    @param dir Directed network in question
    @param `vert` A vertex that belongs to the returned component.
  */
  template <directed_static_network_edge EdgeT>
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
  template <directed_static_network_edge EdgeT>
  std::vector<component<typename EdgeT::VertexType>>
  weakly_connected_components(
      const network<EdgeT>& dir,
      bool singletons = true);

  /**
    Returns the largest weakly connected components of `dir` by number of
    vertices. If multiple components of the maximum size exist, one of the is
    arbitrary selected. For empty network, an empty component is returned.

    @param dir Directed network in question
  */
  template <directed_static_network_edge EdgeT>
  component<typename EdgeT::VertexType>
  largest_weakly_connected_components(const network<EdgeT>& dir);


  /**
    Returns true if all vertices in `dir` belong to the same weakly connected
    component.

    @param dir A directed network
  */
  template <directed_static_network_edge EdgeT>
  bool is_weakly_connected(const network<EdgeT>& dir);

  /**
    Returns the connected component of `net` that includes node `vert`.

    @param net An undirected Network
    @param vert A vertex that will belong to the final component
  */
  template <undirected_static_network_edge EdgeT>
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
  template <undirected_static_network_edge EdgeT>
  std::vector<component<typename EdgeT::VertexType>>
  connected_components(
      const network<EdgeT>& net,
      bool singletons = true);

  /**
    Returns the largest connected components of `dir` by number of vertices. If
    multiple components of the maximum size exist, one of the is arbitrary
    selected. For empty network, an empty component is returned.

    @param net An undirected Network
  */
  template <undirected_static_network_edge EdgeT>
  component<typename EdgeT::VertexType>
  largest_connected_component(const network<EdgeT>& net);


  /**
    Returns true if all all vertices in `net` belong to a single connected
    component.

    @param net An undirected Network
  */
  template <undirected_static_network_edge EdgeT>
  bool is_connected(const network<EdgeT>& net);

  /**
    Returns true if node `destination` can be reached by following edges
    starting from node `source`.

    @param net The network
    @param source The starting point of the reachability query
    @param destination The end point of the reachability query
  */
  template <static_network_edge EdgeT>
  bool is_reachable(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& source,
      const typename EdgeT::VertexType& destination);

  /**
    Checks if the sequence can be the degree sequence of a valid undirected
    graph, containing no multi-edges or loops, based on the Erdős--Gallai
    algorithm.

    Erdős, Paul, and Tibor Gallai. "Graphs with prescribed degrees of vertices."
    Mat. Lapok 11.264-274 (1960): 15.

    @param sequence The sequence in question, a forward range of VertT values.
  */
  template <ranges::forward_range Range>
  requires degree_range<Range>
  bool is_graphic(Range&& degree_sequence);

  /**
    Checks if the sequence can be the degree sequence of a valid directed graph,
    containing no multi-edges or loops, based on the algorithm by Kleitman and
    Wang.

    Kleitman, Daniel J., and Da-Lun Wang. "Algorithms for constructing graphs
    and digraphs with given valences and factors." Discrete Mathematics 6.1
    (1973): 79-88.

    @param sequence The sequence in question, a forward range of VertT values.
  */
  template <ranges::input_range PairRange>
  requires degree_pair_range<PairRange>
  bool is_digraphic(PairRange&& in_out_degree_sequence);


  /**
    Density of an undirected graph is the number of edges divided by the number
    of possible unordered pairs of distinct vertices. Note that in the presense
    of self-links, density might be higher than 1.
  */
  template <network_vertex VertT>
  double density(const undirected_network<VertT>& net);

  /**
    Density of a directed graph is the number of edges divided by the number
    of possible ordered pairs of distinct vertices. Note that in the presense of
    self-links, density might be higher than 1.
  */
  template <network_vertex VertT>
  double density(const directed_network<VertT>& net);

  /**
    Shortest-path lengths from vertex `vert` to every other vertex reachable
    from `vert`.
  */
  template <static_network_edge EdgeT>
  std::unordered_map<
      typename EdgeT::VertexType, std::size_t,
      hash<typename EdgeT::VertexType>>
  shortest_path_lengths_from(
          const network<EdgeT>& net,
          const typename EdgeT::VertexType& vert);

  /**
    Shortest-path lengths to vertex `vert` from every other vertex that can
    reach `vert`.
  */
  template <static_network_edge EdgeT>
  std::unordered_map<
      typename EdgeT::VertexType, std::size_t,
      hash<typename EdgeT::VertexType>>
  shortest_path_lengths_to(
          const network<EdgeT>& net,
          const typename EdgeT::VertexType& vert);


  /**
    Calculate in-degree of a vertex in a network
  */
  template <network_edge EdgeT>
  std::size_t in_degree(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert);

  /**
    Calculate out-degree of a vertex in a network
  */
  template <network_edge EdgeT>
  std::size_t out_degree(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert);

  /**
    Calculate incident degree of a vertex in a network
  */
  template <network_edge EdgeT>
  std::size_t incident_degree(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert);

  /**
    Calculate degree of a vertex in an undirected network
  */
  template <undirected_network_edge EdgeT>
  std::size_t degree(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert);

  /**
    Calculate in-degree of an edge
  */
  template <network_edge EdgeT>
  std::size_t edge_in_degree(
      const EdgeT& edge);

  /**
    Calculate out-degree of an edge
  */
  template <network_edge EdgeT>
  std::size_t edge_out_degree(
      const EdgeT& edge);

  /**
    Calculate incident degree (order) of an edge
  */
  template <network_edge EdgeT>
  std::size_t edge_incident_degree(
      const EdgeT& edge);

  /**
    Calculate degree (order) of an edge
  */
  template <undirected_network_edge EdgeT>
  std::size_t edge_degree(
      const EdgeT& edge);

  /**
    Returns the in-degree sequence of a network
  */
  template <network_edge EdgeT>
  std::vector<std::size_t>
  in_degree_sequence(const network<EdgeT>& net);

  /**
    Returns the out-degree sequence of a network
  */
  template <network_edge EdgeT>
  std::vector<std::size_t>
  out_degree_sequence(const network<EdgeT>& net);

  /**
    Returns the degree sequence of a network
  */
  template <network_edge EdgeT>
  std::vector<std::size_t>
  incident_degree_sequence(const network<EdgeT>& net);

  /**
    Returns the (in-, out-) degree-pair sequence of a network
  */
  template <network_edge EdgeT>
  std::vector<std::pair<std::size_t, std::size_t>>
  in_out_degree_pair_sequence(const network<EdgeT>& net);

  /**
    Returns the degree sequence of an undirected network
  */
  template <undirected_network_edge EdgeT>
  std::vector<std::size_t>
  degree_sequence(const network<EdgeT>& net);

  /**
    Returns the edge in-degree sequence of a network
  */
  template <network_edge EdgeT>
  std::vector<std::size_t>
  edge_in_degree_sequence(const network<EdgeT>& net);

  /**
    Returns the edge out-degree sequence of a network
  */
  template <network_edge EdgeT>
  std::vector<std::size_t>
  edge_out_degree_sequence(const network<EdgeT>& net);

  /**
    Returns the edge degree (order) sequence of a network
  */
  template <network_edge EdgeT>
  std::vector<std::size_t>
  edge_incident_degree_sequence(const network<EdgeT>& net);

  /**
    Returns the (in-, out-) edge-degree-pair sequence of a network
  */
  template <network_edge EdgeT>
  std::vector<std::pair<std::size_t, std::size_t>>
  edge_in_out_degree_pair_sequence(const network<EdgeT>& net);

  /**
    Returns the edge degree (order) sequence of an undirected network
  */
  template <undirected_network_edge EdgeT>
  std::vector<std::size_t>
  edge_degree_sequence(const network<EdgeT>& net);


  /**
    Calculate degree assortativity on undirected networks.
  */
  template <undirected_static_network_edge EdgeT>
  double degree_assortativity(const network<EdgeT>& net);

  /**
    Calculate attribute assortativity on undirected networks. Function
    attribute_fun returns a number given a vertex.
  */
  template <
    undirected_static_network_edge EdgeT,
    std::invocable<const typename EdgeT::VertexType&> AttrFun>
  requires std::convertible_to<
      std::invoke_result_t<AttrFun, const typename EdgeT::VertexType&>, double>
  double attribute_assortativity(
      const network<EdgeT>& net,
      AttrFun&& attribute_fun);

  /**
    Calculate attribute assortativity on undirected networks. Mapping
    attribute_mapping contains a number for each vertex.
  */
  template <
    undirected_static_network_edge EdgeT,
    mapping<typename EdgeT::VertexType, double> MapT>
  double attribute_assortativity(
      const network<EdgeT>& net,
      const MapT& attribute_map,
      double default_value);

  /**
    Calculate attribute assortativity on directed networks. Function
    mutator_attribute_fun returns a number given each tail vertex, whereas
    mutated_attribute_fun returns a number given each head vertex.
  */
  template <
    directed_static_network_edge EdgeT,
    std::invocable<const typename EdgeT::VertexType&> AttrFun1,
    std::invocable<const typename EdgeT::VertexType&> AttrFun2>
  requires
    std::convertible_to<
      std::invoke_result_t<
        AttrFun1, const typename EdgeT::VertexType&>, double> &&
    std::convertible_to<
      std::invoke_result_t<
        AttrFun2, const typename EdgeT::VertexType&>, double>
  double attribute_assortativity(
      const network<EdgeT>& net,
      AttrFun1&& mutator_attr_fun,
      AttrFun2&& mutated_attr_fun);

  /**
    Calculate attribute assortativity on undirected networks. Mapping
    mutator_attribute_mapping contains a number for each tail vertex, whereas
    mutated_attribute_mapping contains a number for each head vertex,
  */
  template <
    directed_static_network_edge EdgeT,
    mapping<typename EdgeT::VertexType, double> MapT1,
    mapping<typename EdgeT::VertexType, double> MapT2>
  double attribute_assortativity(
      const network<EdgeT>& net,
      const MapT1& mutator_attribute_map,
      const MapT2& mutated_attribute_map,
      double mutator_default_value,
      double mutated_default_value);

  /**
    Calculate in-degree in-degree assortativity on directed networks.
  */
  template <directed_static_network_edge EdgeT>
  double in_in_degree_assortativity(const network<EdgeT>& net);

  /**
    Calculate in-degree out-degree assortativity on directed networks.
  */
  template <directed_static_network_edge EdgeT>
  double in_out_degree_assortativity(const network<EdgeT>& net);

  /**
    Calculate out-degree in-degree assortativity on directed networks.
  */
  template <directed_static_network_edge EdgeT>
  double out_in_degree_assortativity(const network<EdgeT>& net);

  /**
    Calculate out-degree out-degree assortativity on directed networks.
  */
  template <directed_static_network_edge EdgeT>
  double out_out_degree_assortativity(const network<EdgeT>& net);
}  // namespace reticula


#include "../../src/algorithms.tpp"

#endif  // INCLUDE_RETICULA_ALGORITHMS_HPP_
