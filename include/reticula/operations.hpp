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


  /**
    Relabels all vertices in a network using a mapping function.

    @param g The input network.
    @param label_map A function that maps old vertices to new vertices.
    @return A new network with relabeled vertices.
   */
  template <network_vertex OutVertT, network_edge EdgeT,
    std::invocable<const typename EdgeT::VertexType&> MapF>
  requires std::convertible_to<
    std::invoke_result_t<MapF, const typename EdgeT::VertexType&>, OutVertT>
  network<rebind_edge_t<EdgeT, OutVertT>>
  relabel_nodes(const network<EdgeT>& g, MapF&& label_map);

  /**
    Relabels all vertices in a network using a mapping container.

    @param g The input network.
    @param label_map A mapping container (e.g., std::map, std::unordered_map)
    that maps old vertices to new vertices.
    @return A new network with relabeled vertices.
   */
  template <network_vertex OutVertT, network_edge EdgeT,
    mapping<typename EdgeT::VertexType, OutVertT> MapT>
  network<rebind_edge_t<EdgeT, OutVertT>>
  relabel_nodes(const network<EdgeT>& g, const MapT& label_map);

  /**
    Relabels all vertices in a network to consecutive integers starting from 0.

    @param g The input network.
    @return A new network with vertices relabeled as consecutive integers.
   */
  template <integer_network_vertex OutVertT, network_edge EdgeT>
  network<rebind_edge_t<EdgeT, OutVertT>>
  relabel_nodes(const network<EdgeT>& g);
}  // namespace reticula

// Implementation
#include <random>

#include "utils.hpp"
#include "networks.hpp"
#include "components.hpp"

namespace reticula {
  template <network_edge EdgeT, ranges::input_range Range>
  requires std::same_as<
            ranges::range_value_t<Range>,
            typename EdgeT::VertexType>
  network<EdgeT>
  vertex_induced_subgraph(
      const network<EdgeT>& net,
      Range&& verts) {
    component<typename EdgeT::VertexType> verts_comp(verts);

    std::vector<EdgeT> es;
    ranges::copy_if(net.edges(), std::back_inserter(es),
        [&verts_comp](const EdgeT& e) {
          for (auto& v: e.incident_verts())
            if (!verts_comp.contains(v))
              return false;
          return true;
        });

    std::vector<typename EdgeT::VertexType> vs;
    ranges::copy_if(net.vertices(), std::back_inserter(vs),
        [&verts_comp](const typename EdgeT::VertexType& v) {
          return verts_comp.contains(v);
        });

    return network<EdgeT>(es, vs);
  }

  template <network_edge EdgeT>
  network<EdgeT>
  vertex_induced_subgraph(
      const network<EdgeT>& net,
      const std::initializer_list<typename EdgeT::VertexType>& verts) {
    return vertex_induced_subgraph(net,
        std::vector<typename EdgeT::VertexType>(verts));
  }

  template <network_edge EdgeT, ranges::input_range Range>
  requires std::same_as<ranges::range_value_t<Range>, EdgeT>
  network<EdgeT>
  edge_induced_subgraph(
      const network<EdgeT>& net,
      Range&& edges) {
    component<EdgeT> edge_comp(edges);

    std::vector<EdgeT> es;
    ranges::copy_if(net.edges(), std::back_inserter(es),
        [&edge_comp](const EdgeT& e) {
          return edge_comp.contains(e);
        });

    return network<EdgeT>(es);
  }

  template <network_edge EdgeT>
  network<EdgeT>
  edge_induced_subgraph(
      const network<EdgeT>& net,
      const std::initializer_list<EdgeT>& edges) {
    return edge_induced_subgraph(net, std::vector<EdgeT>(edges));
  }

  template <network_edge EdgeT>
  network<EdgeT>
  graph_union(const network<EdgeT>& g1, const network<EdgeT>& g2) {
    if (g1.vertices().size() > g2.vertices().size())
      return g1.union_with(g2);
    else
      return g2.union_with(g1);
  }

  template <network_edge EdgeT, ranges::input_range EdgeRange>
  requires std::convertible_to<ranges::range_value_t<EdgeRange>, EdgeT>
  network<EdgeT>
  with_edges(const network<EdgeT>& g, EdgeRange&& edges) {
    return graph_union(g, network<EdgeT>(edges));
  }

  template <network_edge EdgeT>
  network<EdgeT>
  with_edges(
      const network<EdgeT>& g,
      const std::initializer_list<EdgeT>& edges) {
    return with_edges(g, std::vector<EdgeT>(edges));
  }

  template <network_edge EdgeT, ranges::input_range VertRange>
  requires std::convertible_to<
      ranges::range_value_t<VertRange>, typename EdgeT::VertexType>
  network<EdgeT>
  with_vertices(const network<EdgeT>& g, VertRange&& verts) {
    return graph_union(g, network<EdgeT>(std::vector<EdgeT>{}, verts));
  }

  template <network_edge EdgeT>
  network<EdgeT>
  with_vertices(
      const network<EdgeT>& g,
      const std::initializer_list<typename EdgeT::VertexType>& verts) {
    return with_vertices(g, std::vector<typename EdgeT::VertexType>(verts));
  }


  template <network_edge EdgeT, ranges::input_range EdgeRange>
  requires std::convertible_to<ranges::range_value_t<EdgeRange>, EdgeT>
  network<EdgeT>
  without_edges(const network<EdgeT>& g, EdgeRange&& edges) {
    std::vector<EdgeT> edges_v;
    if constexpr (ranges::sized_range<EdgeRange>)
      edges_v.reserve(ranges::size(edges));
    ranges::copy(edges, std::back_inserter(edges_v));
    ranges::sort(edges_v);

    std::vector<EdgeT> remaining;
    if (edges_v.size() < g.edges().size())
      // probably everything in edges_v is already in g.edges()
      // (i.e. its a subset of g.edges())
      remaining.reserve(g.edges().size() - edges_v.size());

    ranges::set_difference(
        g.edges(), edges_v, std::back_inserter(remaining));
    return network<EdgeT>(remaining, g.vertices());
  }

  template <network_edge EdgeT>
  network<EdgeT>
  without_edges(
      const network<EdgeT>& g,
      const std::initializer_list<EdgeT>& edges) {
    return without_edges(g, std::vector<EdgeT>(edges));
  }


  template <network_edge EdgeT, ranges::input_range VertRange>
  requires std::convertible_to<
      ranges::range_value_t<VertRange>, typename EdgeT::VertexType>
  network<EdgeT>
  without_vertices(const network<EdgeT>& g, VertRange&& verts) {
    component<typename EdgeT::VertexType> verts_comp(verts);
    auto edges_view = g.edges() | views::filter(
          [&verts_comp](const EdgeT& e) {
            for (auto& v: e.incident_verts())
              if (verts_comp.contains(v))
                return false;
            return true;
          });
    auto graph_verts = g.vertices();
    auto verts_view = graph_verts | views::filter(
          [&verts_comp](const typename EdgeT::VertexType& v) {
            return !verts_comp.contains(v);
          });
    return network<EdgeT>(edges_view, verts_view);
  }

  template <network_edge EdgeT>
  network<EdgeT>
  without_vertices(
      const network<EdgeT>& g,
      const std::initializer_list<typename EdgeT::VertexType>& verts) {
    return without_vertices(g, component<typename EdgeT::VertexType>(verts));
  }



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
      Gen& gen) {
    auto edges = g.edges() | views::filter(
          [&occupation_prob, &gen](const EdgeT& e) {
            return std::bernoulli_distribution{1.0-occupation_prob(e)}(gen);
        });
    return without_edges(g, edges);
  }

  template <
    network_edge EdgeT,
    mapping<EdgeT, double> ProbMapT,
    std::uniform_random_bit_generator Gen>
  network<EdgeT>
  occupy_edges(
      const network<EdgeT>& g,
      const ProbMapT& prob_map,
      Gen& gen,
      double default_prob) {
    return occupy_edges(g,
        [&prob_map, default_prob](const EdgeT& e) {
          auto it = prob_map.find(e);
          if (it == prob_map.end())
            return default_prob;
          else
            return it->second;
        }, gen);
  }

  template <network_edge EdgeT, std::uniform_random_bit_generator Gen>
  network<EdgeT>
  uniformly_occupy_edges(
      const network<EdgeT>& g,
      double occupation_prob,
      Gen& gen) {
    return occupy_edges(g,
        [occupation_prob](const EdgeT&) {
            return occupation_prob;
        }, gen);
  }


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
      Gen& gen) {
    auto graph_verts = g.vertices();
    auto verts = graph_verts | views::filter(
          [&occupation_prob, &gen](const typename EdgeT::VertexType& v) {
            return std::bernoulli_distribution{1.0-occupation_prob(v)}(gen);
        });
    return without_vertices(g, verts);
  }


  template <
    network_edge EdgeT,
    mapping<typename EdgeT::VertexType, double> ProbMapT,
    std::uniform_random_bit_generator Gen>
  network<EdgeT>
  occupy_vertices(
      const network<EdgeT>& g,
      const ProbMapT& prob_map,
      Gen& gen,
      double default_prob) {
    return occupy_vertices(g,
        [&prob_map, default_prob](const typename EdgeT::VertexType& v) {
          auto it = prob_map.find(v);
          if (it == prob_map.end())
            return default_prob;
          else
            return it->second;
        }, gen);
  }

  template <network_edge EdgeT, std::uniform_random_bit_generator Gen>
  network<EdgeT>
  uniformly_occupy_vertices(
      const network<EdgeT>& g,
      double default_prob,
      Gen& gen) {
    return occupy_vertices(g,
        [default_prob](const typename EdgeT::VertexType&) {
            return default_prob;
        }, gen);
  }

  template <network_vertex VertT1, network_vertex VertT2>
  undirected_network<std::pair<VertT1, VertT2>>
  cartesian_product(
      const undirected_network<VertT1>& g1,
      const undirected_network<VertT2>& g2) {
    std::vector<undirected_edge<std::pair<VertT1, VertT2>>> edges;
    edges.reserve(
        g1.vertices().size()*g2.edges().size() +
        g1.edges().size()*g2.vertices().size());

    for (auto&& e: g1.edges()) {
      auto verts = e.mutator_verts();
      if (verts.size() > 1)
        for (auto&& n: g2.vertices())
          edges.emplace_back(std::make_pair(verts[0], n),
              std::make_pair(verts[1], n));
    }

    for (auto&& n: g1.vertices()) {
      for (auto&& e: g2.edges()) {
        auto verts = e.mutator_verts();
        if (verts.size() > 1)
          edges.emplace_back(
              std::make_pair(n, verts[0]),
              std::make_pair(n, verts[1]));
      }
    }

    return undirected_network<std::pair<VertT1, VertT2>>(edges);
  }

  template <network_vertex OutVertT, network_edge EdgeT,
    std::invocable<const typename EdgeT::VertexType&> MapF>
  requires std::convertible_to<
    std::invoke_result_t<MapF, const typename EdgeT::VertexType&>, OutVertT>
  network<rebind_edge_t<EdgeT, OutVertT>>
  relabel_nodes(const network<EdgeT>& g, MapF&& label_map) {
    using OutEdgeT = rebind_edge_t<EdgeT, OutVertT>;
    
    std::vector<OutEdgeT> new_edges;
    new_edges.reserve(g.edges().size());
    
    for (const auto& edge : g.edges()) {
      auto incident = edge.incident_verts();
      
      std::vector<OutVertT> new_incident;
      new_incident.reserve(incident.size());
      for (const auto& v : incident) {
        new_incident.push_back(label_map(v));
      }
      
      if constexpr (temporal_network_edge<EdgeT>) {
        // All temporal edges now use the same 5-argument constructor
        auto mutators = edge.mutator_verts();
        auto mutated = edge.mutated_verts();
        
        std::vector<OutVertT> new_mutators;
        new_mutators.reserve(mutators.size());
        for (const auto& v : mutators) {
          new_mutators.push_back(label_map(v));
        }
        
        std::vector<OutVertT> new_mutated;
        new_mutated.reserve(mutated.size());
        for (const auto& v : mutated) {
          new_mutated.push_back(label_map(v));
        }
        
        new_edges.emplace_back(uniform_const, new_mutators, new_mutated,
                              edge.cause_time(), edge.effect_time());
      } else {
        // Static edge
        if constexpr (is_dyadic_v<EdgeT>) {
          // For dyadic edges, use the first two vertices
          if (new_incident.size() >= 2) {
            new_edges.emplace_back(new_incident[0], new_incident[1]);
          } else if (new_incident.size() == 1) {
            new_edges.emplace_back(new_incident[0], new_incident[0]);
          }
        } else {
          // For hyperedges, use the uniform constructor
          auto mutators = edge.mutator_verts();
          auto mutated = edge.mutated_verts();
          
          std::vector<OutVertT> new_mutators;
          new_mutators.reserve(mutators.size());
          for (const auto& v : mutators) {
            new_mutators.push_back(label_map(v));
          }
          
          std::vector<OutVertT> new_mutated;
          new_mutated.reserve(mutated.size());
          for (const auto& v : mutated) {
            new_mutated.push_back(label_map(v));
          }
          
          new_edges.emplace_back(uniform_const, new_mutators, new_mutated);
        }
      }
    }
    
    std::vector<OutVertT> new_vertices;
    new_vertices.reserve(g.vertices().size());
    for (const auto& v : g.vertices()) {
      new_vertices.push_back(label_map(v));
    }
    
    return network<OutEdgeT>(new_edges, new_vertices);
  }


  template <network_vertex OutVertT, network_edge EdgeT,
    mapping<typename EdgeT::VertexType, OutVertT> MapT>
  network<rebind_edge_t<EdgeT, OutVertT>>
  relabel_nodes(const network<EdgeT>& g, const MapT& label_map) {
    return relabel_nodes<OutVertT>(g, [&label_map](const typename EdgeT::VertexType& v) {
      auto it = label_map.find(v);
      if (it == label_map.end())
        throw std::out_of_range("Vertex not found in the label map");
      else
        return it->second;
      });
  }

  template <integer_network_vertex OutVertT, network_edge EdgeT>
  network<rebind_edge_t<EdgeT, OutVertT>>
  relabel_nodes(const network<EdgeT>& g) {
    using InVertT = typename EdgeT::VertexType;
    std::unordered_map<InVertT, OutVertT, hash<InVertT>> new_labels;
    new_labels.reserve(g.vertices().size());

    OutVertT i{};
    for (const auto& v: g.vertices())
      new_labels.emplace(v, i++);

    return relabel_nodes<OutVertT>(g, new_labels);
  }
}  // namespace reticula


#endif  // INCLUDE_RETICULA_OPERATIONS_HPP_
