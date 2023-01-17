#ifndef INCLUDE_RETICULA_OPERATIONS_HPP_
#define INCLUDE_RETICULA_OPERATIONS_HPP_

#include <random>
#include <utility>

#include "ranges.hpp"
#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  /**
    Vertex induced subgraph is a subset of the graph that includes only nodes
    that are present in the provided set, along with any edge with all incident
    vertices present in the set.

    @param net The network in question
    @param verts The set of vertices. You can pass a component or any other type
    that conforms to the input_range concept, for example a vector or a set of
    vertices.
  */
  template <network_edge EdgeT, ranges::input_range Range>
  requires std::same_as<
            ranges::range_value_t<Range>,
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
  template <network_edge EdgeT, ranges::input_range Range>
  requires std::same_as<ranges::range_value_t<Range>, EdgeT>
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
  template <network_edge EdgeT, ranges::input_range EdgeRange>
  requires std::convertible_to<ranges::range_value_t<EdgeRange>, EdgeT>
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
  template <network_edge EdgeT, ranges::input_range VertRange>
  requires std::convertible_to<
      ranges::range_value_t<VertRange>, typename EdgeT::VertexType>
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
  template <network_edge EdgeT, ranges::input_range EdgeRange>
  requires std::convertible_to<ranges::range_value_t<EdgeRange>, EdgeT>
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
  template <network_edge EdgeT, ranges::input_range VertRange>
  requires std::convertible_to<
      ranges::range_value_t<VertRange>, typename EdgeT::VertexType>
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
    Calculates the Cartesian product of two undirected networks.
  */
  template <network_vertex VertT1, network_vertex VertT2>
  undirected_network<std::pair<VertT1, VertT2>>
  cartesian_product(
      const undirected_network<VertT1>& g1,
      const undirected_network<VertT2>& g2);


  template <integer_network_vertex OutVertT, network_vertex InVertT>
  undirected_network<OutVertT>
  relabel_nodes(const undirected_network<InVertT>& g);
}  // namespace reticula

#include "../../src/operations.tpp"

#endif  // INCLUDE_RETICULA_OPERATIONS_HPP_
