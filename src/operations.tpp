#include <random>

#include "../include/reticula/utils.hpp"
#include "../include/reticula/networks.hpp"
#include "../include/reticula/components.hpp"

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

  template <integer_network_vertex OutVertT, network_vertex InVertT>
  undirected_network<OutVertT>
  relabel_nodes(const undirected_network<InVertT>& g) {
    std::unordered_map<InVertT, OutVertT, hash<InVertT>> new_labels;
    new_labels.reserve(g.vertices().size());

    OutVertT i = {};
    for (auto&& n: g.vertices())
      new_labels.emplace(n, i++);

    std::vector<undirected_edge<OutVertT>> edges;
    for (auto&& e: g.edges()) {
      auto verts = e.mutator_verts();
      if (verts.size() > 1)
        edges.emplace_back(new_labels.at(verts[0]), new_labels.at(verts[1]));
    }

    return undirected_network<OutVertT>(edges);
  }
}  // namespace reticula
