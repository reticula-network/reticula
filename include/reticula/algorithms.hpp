#ifndef INCLUDE_RETICULA_ALGORITHMS_HPP_
#define INCLUDE_RETICULA_ALGORITHMS_HPP_

#include <vector>
#include <ranges>
#include <unordered_set>
#include <optional>
#include <ranges>

#include "network_concepts.hpp"
#include "networks.hpp"
#include "components.hpp"

namespace reticula {
  template <static_edge EdgeT, typename DiscoveryF>
  component<typename EdgeT::VertexType>
  breadth_first_search(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& vert,
      DiscoveryF discovered,
      std::size_t size_hint = 0);

  /**
    Vertex induced subgraph is a subset of the graph that includes only nodes
    that are present in the provided set, along with any edge with all incident
    vertices present in the set.

    @param net The network in question
    @param verts The set of vertices. You can pass a component or any other type
    that conforms to the input_range concept, for example a vector or a set of
    vertices.
  */
  template <network_edge EdgeT, std::ranges::input_range Range>
  requires std::same_as<
            std::ranges::range_value_t<Range>,
            typename EdgeT::VertexType>
  network<EdgeT>
  vertex_induced_subgraph(
      const network<EdgeT>& net,
      Range&& verts);

  template <network_edge EdgeT>
  network<EdgeT>
  vertex_induced_subgraph(
      const network<EdgeT>& net,
      const std::initializer_list<typename EdgeT::VertexType>& verts);

  /**
    Edge induced subgraph is a subset of the graph that includes only edges that
    are present in the provided set of edges, along with all their incident
    verices.

    @param net The network in question
    @param edges The set of edges. You can pass any type that conforms to the
    input_range concept, for example a vector or a set of edges.
  */
  template <network_edge EdgeT, std::ranges::input_range Range>
  requires std::same_as<std::ranges::range_value_t<Range>, EdgeT>
  network<EdgeT>
  edge_induced_subgraph(
      const network<EdgeT>& net,
      Range&& edges);

  template <network_edge EdgeT>
  network<EdgeT>
  edge_induced_subgraph(
      const network<EdgeT>& net,
      const std::initializer_list<EdgeT>& edges);

  /**
    Returns a copy of the graph `g` with edge in range `edges` added to the
    graph.
  */
  template <network_edge EdgeT, std::ranges::input_range EdgeRange>
  requires std::convertible_to<std::ranges::range_value_t<EdgeRange>, EdgeT>
  network<EdgeT>
  with_edges(const network<EdgeT>& g, EdgeRange&& edges);

  template <network_edge EdgeT>
  network<EdgeT>
  with_edges(
      const network<EdgeT>& g,
      const std::initializer_list<EdgeT>& edges);

  /**
    Returns a copy of the graph `g` with vertices in range `verts` added to the
    graph.
  */
  template <network_edge EdgeT, std::ranges::input_range VertRange>
  requires std::convertible_to<
      std::ranges::range_value_t<VertRange>, typename EdgeT::VertexType>
  network<EdgeT>
  with_vertices(const network<EdgeT>& g, VertRange&& verts);

  template <network_edge EdgeT>
  network<EdgeT>
  with_vertices(
      const network<EdgeT>& g,
      const std::initializer_list<typename EdgeT::VertexType>& verts);

  /**
    Returns a copy of the graph `g` with edge in range `edges` removed from the
    graph.
  */
  template <network_edge EdgeT, std::ranges::input_range EdgeRange>
  requires std::convertible_to<std::ranges::range_value_t<EdgeRange>, EdgeT>
  network<EdgeT>
  without_edges(const network<EdgeT>& g, EdgeRange&& edges);

  template <network_edge EdgeT>
  network<EdgeT>
  without_edges(
      const network<EdgeT>& g,
      const std::initializer_list<EdgeT>& edges);

  /**
    Returns a copy of the graph `g` with vertices in range `verts` removed from
    the graph.
  */
  template <network_edge EdgeT, std::ranges::input_range VertRange>
  requires std::convertible_to<
      std::ranges::range_value_t<VertRange>, typename EdgeT::VertexType>
  network<EdgeT>
  without_vertices(const network<EdgeT>& g, VertRange&& verts);

  template <network_edge EdgeT>
  network<EdgeT>
  without_vertices(
      const network<EdgeT>& g,
      const std::initializer_list<typename EdgeT::VertexType>& verts);

  /**
    Randomly occupy edges with each edge's probability of occupation determined
    by running the function ``occupation_prob`.
  */
  template <
    network_edge EdgeT,
    std::invocable<const EdgeT&> ProbFun,
    std::uniform_random_bit_generator Gen>
  requires std::convertible_to<
    std::invoke_result_t<ProbFun, const EdgeT&>, double>
  network<EdgeT>
  occupy_edges(
      const network<EdgeT>& g,
      ProbFun&& occupation_prob,
      Gen& gen);

  /**
    Randomly occupy edges with each edge's probability of occupation determined
    by the (ordered or unordered) map `prob_map`, with missing keys using the
    value `default_prob`.
  */
  template <
    network_edge EdgeT,
    mapping<EdgeT, double> ProbMapT,
    std::uniform_random_bit_generator Gen>
  network<EdgeT>
  occupy_edges(
      const network<EdgeT>& g,
      const ProbMapT& prob_map,
      Gen& gen,
      double default_prob = 0.0);

  /**
    Randomly occupy edges with the uniform probability `occupation_prob`.
  */
  template <network_edge EdgeT, std::uniform_random_bit_generator Gen>
  network<EdgeT>
  uniformly_occupy_edges(
      const network<EdgeT>& g,
      double occupation_prob,
      Gen& gen);

  /**
    Randomly occupy vertices with each vetex having a probability of occupation
    determined by running the function ``occupation_prob`.
  */
  template <
    network_edge EdgeT,
    std::invocable<const typename EdgeT::VertexType&> ProbFun,
    std::uniform_random_bit_generator Gen>
  requires std::convertible_to<
    std::invoke_result_t<ProbFun, const typename EdgeT::VertexType&>, double>
  network<EdgeT>
  occupy_vertices(
      const network<EdgeT>& g,
      ProbFun&& occupation_prob,
      Gen& gen);

  /**
    Randomly occupy vertices with each vertex having a probability of occupation
    determined by the (ordered or unordered) map `prob_map`, with missing keys
    using the value `default_prob`.
  */
  template <
    network_edge EdgeT,
    mapping<typename EdgeT::VertexType, double> ProbMapT,
    std::uniform_random_bit_generator Gen>
  network<EdgeT>
  occupy_vertices(
      const network<EdgeT>& g,
      const ProbMapT& prob_map,
      Gen& gen,
      double default_prob = 0.0);

  /**
    Randomly occupy edges with the uniform probability `occupation_prob`.
  */
  template <network_edge EdgeT, std::uniform_random_bit_generator Gen>
  network<EdgeT>
  uniformly_occupy_vertices(
      const network<EdgeT>& g,
      double occupation_prob,
      Gen& gen);


  /**
    Returns the graph union (not the disjoint union) of the two graphs. The
    union of two graphs consists of union of the two graph's vertices connected
    through the union of their edges.
  */
  template <network_edge EdgeT>
  network<EdgeT>
  graph_union(const network<EdgeT>& g1, const network<EdgeT>& g2);

  /**
    Returns true if the directed graph contains no cycles. This is detemined by
    trying to find a topologial ordering or the edges. If you intend to get a
    topological ordering of the nodes anyway, use
    `reticula::try_topological_order`.

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
      const network<EdgeT>& dir,
      std::size_t seed);


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
      const network<EdgeT>& dir,
      std::size_t seed);

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
    Returns the largest weakly connected components of `dir` by number of
    vertices. If multiple components of the maximum size exist, one of the is
    arbitrary selected. For empty network, an empty component is returned.

    @param dir Directed network in question
  */
  template <static_directed_edge EdgeT>
  component<typename EdgeT::VertexType>
  largest_weakly_connected_components(const network<EdgeT>& dir);


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
    Returns the largest connected components of `dir` by number of vertices. If
    multiple components of the maximum size exist, one of the is arbitrary
    selected. For empty network, an empty component is returned.

    @param net An undirected Network
  */
  template <static_undirected_edge EdgeT>
  component<typename EdgeT::VertexType>
  largest_connected_component(const network<EdgeT>& net);


  /**
    Returns true if all all vertices in `net` belong to a single connected
    component.

    @param net An undirected Network
  */
  template <static_undirected_edge EdgeT>
  bool is_connected(const network<EdgeT>& net);

  /**
    Returns true if node `destination` can be reached by following edges
    starting from node `source`.

    @param net The network
    @param source The starting point of the reachability query
    @param destination The end point of the reachability query
  */
  template <static_edge EdgeT>
  bool is_reachable(
      const network<EdgeT>& net,
      const typename EdgeT::VertexType& source,
      const typename EdgeT::VertexType& destination);

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
  template <std::ranges::input_range PairRange>
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
  template <static_edge EdgeT>
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
  template <static_edge EdgeT>
  std::unordered_map<
      typename EdgeT::VertexType, std::size_t,
      hash<typename EdgeT::VertexType>>
  shortest_path_lengths_to(
          const network<EdgeT>& net,
          const typename EdgeT::VertexType& vert);
}  // namespace reticula

#include "../../src/algorithms.tpp"

#endif  // INCLUDE_RETICULA_ALGORITHMS_HPP_
