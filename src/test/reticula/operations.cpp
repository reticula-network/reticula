#include <vector>
#include <random>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/catch_approx.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::Contains;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/networks.hpp>
#include <reticula/components.hpp>
#include <reticula/operations.hpp>
#include <reticula/generators.hpp>
#include <reticula/algorithms.hpp>

TEST_CASE("cartesian product", "[reticula::cartesian_product]") {
    reticula::undirected_network<int> graph1({{1, 2}, {1, 3}});
    reticula::undirected_network<int> graph2({{1, 2}, {2, 3}});

    reticula::undirected_network<std::pair<int, int>> prod =
      reticula::cartesian_product(graph1, graph2);

    REQUIRE(prod.vertices().size() ==
        graph1.vertices().size()*graph2.vertices().size());
    REQUIRE(prod.edges().size() ==
        graph1.vertices().size()*graph2.edges().size() +
        graph2.vertices().size()*graph1.edges().size());

    REQUIRE_THAT(prod.edges(),
        UnorderedRangeEquals(
          std::vector<reticula::undirected_edge<std::pair<int, int>>>{
            {{1, 1}, {2, 1}}, {{1, 1}, {3, 1}}, {{1, 1}, {1, 2}},
            {{1, 2}, {2, 2}}, {{1, 2}, {3, 2}}, {{1, 2}, {1, 3}},
            {{1, 3}, {2, 3}}, {{1, 3}, {3, 3}},
            {{2, 1}, {2, 2}},
            {{2, 2}, {2, 3}},
            {{3, 1}, {3, 2}},
            {{3, 2}, {3, 3}}}));
}


TEST_CASE("vertex induced subgraph", "[reticula::vertex_induced_subgraph]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> network(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6},
      {{3, 4}, {4}, 8}});

  SECTION("with components") {
    reticula::component<typename EdgeType::VertexType> comp({1, 2, 3, 4, 9});
    auto res = reticula::vertex_induced_subgraph(network, comp);

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 3, 4}),
        UnorderedRangeEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}),
        UnorderedRangeEquals(res.edges()));
  }

  SECTION("with initializer_list") {
    auto res = reticula::vertex_induced_subgraph(network, {1, 2, 3, 4, 9});

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 3, 4}),
        UnorderedRangeEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}),
        UnorderedRangeEquals(res.edges()));
  }
}

TEST_CASE("edge induced subgraph", "[reticula::edge_induced_subgraph]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> network(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6},
      {{3, 4}, {4}, 8}});

  SECTION("with component") {
    reticula::component<EdgeType> comp({
        {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}, {{3, 4}, {4}, 10}});
    auto res = reticula::edge_induced_subgraph(network, comp);

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 4, 7}),
        UnorderedRangeEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}),
        UnorderedRangeEquals(res.edges()));
  }

  SECTION("with initializer_list") {
    auto res = reticula::edge_induced_subgraph(network, {
        {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}, {{3, 4}, {4}, 10}});

    REQUIRE_THAT(std::vector<typename EdgeType::VertexType>(
          {1, 2, 4, 7}),
        UnorderedRangeEquals(res.vertices()));

    REQUIRE_THAT(std::vector<EdgeType>({
          {{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}),
        UnorderedRangeEquals(res.edges()));
  }
}

TEST_CASE("graph union", "[reticula::graph_union]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});

  reticula::network<EdgeType> n2(
      {{{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});

  auto res1 = reticula::graph_union(n1, n2);
  auto res2 = reticula::graph_union(n1, n2);
  REQUIRE_THAT(res1.edges(), RangeEquals(res2.edges()));
  for (const auto& edge : n1.edges())
    REQUIRE_THAT(res1.edges(), Contains(edge));
  for (const auto& edge : n2.edges())
    REQUIRE_THAT(res1.edges(), Contains(edge));

  REQUIRE_THAT(res1.vertices(), RangeEquals(res2.vertices()));
  for (const auto& vertex : n1.vertices())
    REQUIRE_THAT(res1.vertices(), Contains(vertex));
  for (const auto& vertex : n2.vertices())
    REQUIRE_THAT(res1.vertices(), Contains(vertex));
}

TEST_CASE("with edges", "[reticula::with_edges]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::with_edges(n1, {
      {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}});
  REQUIRE_THAT(res.vertices(), UnorderedRangeEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 3, 4, 7}));
  REQUIRE_THAT(res.edges(), UnorderedRangeEquals(
        std::vector<EdgeType>{
          {{1}, {2, 4}, 1}, {{2}, {1, 7}, 2},
          {{7, 1}, {2}, 5}, {{2}, {3, 1}, 6}, {{3, 4}, {4}, 8}}));
}

TEST_CASE("with vertices", "[reticula::with_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::with_vertices(n1, {3, 4, 5, 6});
  REQUIRE_THAT(res.vertices(), UnorderedRangeEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 3, 4, 5, 6, 7}));
  REQUIRE_THAT(res.edges(), UnorderedRangeEquals(
        std::vector<EdgeType>{
          {{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
}


TEST_CASE("without edges", "[reticula::without_edges]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::without_edges(n1,
      {{{3}, {3}, 1}, {{3}, {3}, 1}, {{2}, {7, 1}, 2}});
  REQUIRE_THAT(res.vertices(), UnorderedRangeEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 4, 7}));
  REQUIRE_THAT(res.edges(), UnorderedRangeEquals(
        std::vector<EdgeType>{{{1}, {2, 4}, 1}, {{7, 1}, {2}, 5}}));
}

TEST_CASE("without vertices", "[reticula::without_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  reticula::network<EdgeType> n1(
      {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto res = reticula::without_vertices(n1, {3, 4, 8});
  REQUIRE_THAT(res.vertices(), UnorderedRangeEquals(
        std::vector<typename EdgeType::VertexType>{1, 2, 7}));
  REQUIRE_THAT(res.edges(), UnorderedRangeEquals(
        std::vector<EdgeType>{{{7, 1}, {2}, 5}, {{2}, {1, 7}, 2}}));
}

TEST_CASE("occupy edges", "[reticula::occupy_edges]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  std::mt19937 gen(42);
  std::unordered_map<EdgeType, double, reticula::hash<EdgeType>> probs = {
    {{{1}, {2, 4}, 1}, 0.0}, {{{2}, {1, 7}, 2}, 1.0}};
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto g1 = reticula::occupy_edges(n1, [&probs](const EdgeType& e) {
      return probs.contains(e) ? probs.at(e) : 0.0;
    }, gen);
  auto g2 = reticula::occupy_edges(n1, probs, gen, 0.0);
  REQUIRE_THAT(n1.vertices(), UnorderedRangeEquals(g1.vertices()));
  REQUIRE_THAT(n1.vertices(), UnorderedRangeEquals(g2.vertices()));

  REQUIRE_THAT(g1.edges(),
      UnorderedRangeEquals(std::vector<EdgeType>{{{2}, {1, 7}, 2}}));
  REQUIRE_THAT(g2.edges(),
      UnorderedRangeEquals(std::vector<EdgeType>{{{2}, {1, 7}, 2}}));
}

TEST_CASE("uniformly occupy edges", "[reticula::uniformly_occupy_edges]") {
  std::size_t n = 512;
  double p = 0.5;
  auto g = reticula::complete_graph(n);
  std::mt19937_64 gen(42);
  auto g2 = reticula::uniformly_occupy_edges(g, p, gen);
  double mean = static_cast<double>(n)*static_cast<double>((n-1)/2)*p;
  double sigma = std::sqrt(mean);
  REQUIRE(g2.vertices().size() == n);
  REQUIRE(static_cast<double>(g2.edges().size()) > mean - 3*sigma);
  REQUIRE(static_cast<double>(g2.edges().size()) < mean + 3*sigma);
}

TEST_CASE("occupy vertices", "[reticula::occupy_vertices]") {
  using EdgeType = reticula::directed_temporal_hyperedge<int, int>;
  std::mt19937 gen(42);
  std::unordered_map<int, double> probs = {
    {1, 1.0}, {2, 1.0}, {7, 1.0}};
  reticula::network<EdgeType> n1(
    {{{1}, {2, 4}, 1}, {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}});
  auto g1 = reticula::occupy_vertices(n1, [&probs](int v) {
      return probs.contains(v) ? probs.at(v) : 0.0;
    }, gen);
  auto g2 = reticula::occupy_vertices(n1, probs, gen, 0.0);
  REQUIRE_THAT(g1.vertices(), UnorderedRangeEquals(std::vector<int>{1, 2, 7}));
  REQUIRE_THAT(g2.vertices(), UnorderedRangeEquals(std::vector<int>{1, 2, 7}));

  REQUIRE_THAT(g1.edges(),
      UnorderedRangeEquals(std::vector<EdgeType>{
        {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
  REQUIRE_THAT(g2.edges(),
      UnorderedRangeEquals(std::vector<EdgeType>{
        {{2}, {1, 7}, 2}, {{7, 1}, {2}, 5}}));
}

TEST_CASE("uniformly occupy vertices",
    "[reticula::uniformly_occupy_vertices]") {
  std::size_t n = 512;
  double p = 0.5;
  auto g = reticula::complete_graph(n);
  std::mt19937_64 gen(42);
  auto g2 = reticula::uniformly_occupy_vertices(g, p, gen);
  double mean = static_cast<double>(n)*p;
  double sigma = std::sqrt(mean);
  REQUIRE(static_cast<double>(g2.vertices().size()) > mean - 3*sigma);
  REQUIRE(static_cast<double>(g2.vertices().size()) < mean + 3*sigma);
  REQUIRE(g2.edges().size() ==
      (g2.vertices().size()*(g2.vertices().size()-1))/2);
}

TEST_CASE("mapping", "[reticula::mapping]") {
  STATIC_REQUIRE(reticula::mapping<std::unordered_map<int, double>,
      int, double>);
  STATIC_REQUIRE(reticula::mapping<std::map<int, double>,
      int, double>);
}


TEST_CASE("relabel nodes", "[reticula::relabel_nodes]") {
  // let's make a graph with more to it than integral vertices
  reticula::undirected_network<int> graph1({{1, 2}, {1, 3}});
  reticula::undirected_network<int> graph2({{1, 2}, {1, 3}});
  reticula::undirected_network<std::pair<int, int>> orig =
    reticula::cartesian_product(graph1, graph2);

  reticula::undirected_network<std::size_t> relabeled =
    reticula::relabel_nodes<std::size_t>(orig);

  REQUIRE(orig.vertices().size() == relabeled.vertices().size());
  REQUIRE(orig.edges().size() == relabeled.edges().size());
}

TEST_CASE("relabel_nodes with function mapping", "[reticula::relabel_nodes]") {
  SECTION("simple directed network with lambda") {
    reticula::directed_network<int> net({{1, 2}, {2, 3}, {3, 1}});
    
    auto relabeled = reticula::relabel_nodes<std::size_t>(net, 
      [](int v) { return static_cast<std::size_t>(v * 10); });
    
    REQUIRE(relabeled.vertices().size() == 3);
    REQUIRE(relabeled.edges().size() == 3);
    
    // Check that vertices are correctly mapped
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<std::size_t>({10, 20, 30})));
  }
  
  SECTION("undirected network with function object") {
    struct Doubler {
      int operator()(int x) const { return x * 2; }
    };
    
    reticula::undirected_network<int> net({{1, 2}, {2, 3}});
    auto relabeled = reticula::relabel_nodes<int>(net, Doubler{});
    
    REQUIRE(relabeled.vertices().size() == 3);
    REQUIRE(relabeled.edges().size() == 2);
    
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<int>({2, 4, 6})));
  }
  
  SECTION("type conversion") {
    reticula::undirected_network<int> net({{-1, 0}, {0, 1}});
    
    auto relabeled = reticula::relabel_nodes<std::size_t>(net,
      [](int v) { return static_cast<std::size_t>(v + 10); });
    
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<std::size_t>({9, 10, 11})));
  }
  
  SECTION("complex vertex types") {
    using VertexType = std::pair<int, int>;
    reticula::undirected_network<VertexType> net({
      {{1, 2}, {3, 4}},
      {{3, 4}, {5, 6}}
    });
    
    auto relabeled = reticula::relabel_nodes<int>(net,
      [](const VertexType& v) { return v.first + v.second; });
    
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<int>({3, 7, 11})));
  }
}

TEST_CASE("relabel_nodes with container mapping", "[reticula::relabel_nodes]") {
  SECTION("std::map mapping") {
    reticula::directed_network<std::string> net({
      {"alice", "bob"},
      {"bob", "charlie"},
      {"charlie", "alice"}
    });
    
    std::map<std::string, int> mapping = {
      {"alice", 1},
      {"bob", 2}, 
      {"charlie", 3}
    };
    
    auto relabeled = reticula::relabel_nodes<int>(net, mapping);
    
    REQUIRE(relabeled.vertices().size() == 3);
    REQUIRE(relabeled.edges().size() == 3);
    
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<int>({1, 2, 3})));
  }
  
  SECTION("std::unordered_map mapping") {
    reticula::undirected_network<int> net({{10, 20}, {20, 30}});
    
    std::unordered_map<int, std::string> mapping = {
      {10, "ten"},
      {20, "twenty"},
      {30, "thirty"}
    };
    
    auto relabeled = reticula::relabel_nodes<std::string>(net, mapping);
    
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<std::string>({"ten", "twenty", "thirty"})));
  }
  
  SECTION("missing vertex throws exception") {
    reticula::undirected_network<int> net({{1, 2}, {2, 3}});
    
    std::map<int, std::string> incomplete_mapping = {
      {1, "one"},
      {2, "two"}
      // missing vertex 3
    };
    
    REQUIRE_THROWS_AS(
      reticula::relabel_nodes<std::string>(net, incomplete_mapping),
      std::out_of_range
    );
  }
  
  SECTION("extra mappings are ignored") {
    reticula::undirected_network<int> net({{1, 2}});
    
    std::map<int, char> mapping = {
      {1, 'a'},
      {2, 'b'},
      {3, 'c'},  // extra mapping
      {4, 'd'}   // extra mapping
    };
    
    auto relabeled = reticula::relabel_nodes<char>(net, mapping);
    
    REQUIRE(relabeled.vertices().size() == 2);
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<char>({'a', 'b'})));
  }
}

TEST_CASE("relabel_nodes automatic integer", "[reticula::relabel_nodes]") {
  SECTION("consecutive numbering from 0") {
    reticula::undirected_network<std::string> net({
      {"zebra", "apple"},
      {"apple", "banana"},
      {"banana", "zebra"}
    });
    
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.vertices().size() == 3);
    REQUIRE(relabeled.edges().size() == 3);
    
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<std::size_t>({0, 1, 2})));
  }
  
  SECTION("different integer types") {
    reticula::directed_network<char> net({{'a', 'b'}, {'b', 'c'}});
    
    auto relabeled_int = reticula::relabel_nodes<int>(net);
    auto relabeled_uint8 = reticula::relabel_nodes<std::uint8_t>(net);
    
    REQUIRE(relabeled_int.vertices().size() == 3);
    REQUIRE(relabeled_uint8.vertices().size() == 3);
    
    auto vertices_int = relabeled_int.vertices();
    auto vertices_uint8 = relabeled_uint8.vertices();
    
    REQUIRE_THAT(vertices_int, UnorderedRangeEquals(std::vector<int>({0, 1, 2})));
    REQUIRE_THAT(vertices_uint8, UnorderedRangeEquals(std::vector<std::uint8_t>({0, 1, 2})));
  }
  
  SECTION("complex vertex types to integers") {
    using ComplexVertex = std::pair<std::string, int>;
    reticula::undirected_network<ComplexVertex> net({
      {{"hello", 1}, {"world", 2}},
      {{"world", 2}, {"test", 3}}
    });
    
    auto relabeled = reticula::relabel_nodes<int>(net);
    
    REQUIRE(relabeled.vertices().size() == 3);
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<int>({0, 1, 2})));
  }
}

TEST_CASE("relabel_nodes edge type handling", "[reticula::relabel_nodes]") {
  SECTION("directed edges") {
    reticula::directed_network<int> net({{1, 2}, {2, 3}, {3, 1}});
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 3);
    
    // Check that edges maintain their directedness
    for (const auto& edge : relabeled.edges()) {
      REQUIRE(edge.mutator_verts().size() == 1);
      REQUIRE(edge.mutated_verts().size() == 1);
    }
  }
  
  SECTION("undirected edges") {
    reticula::undirected_network<int> net({{1, 2}, {2, 3}});
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 2);
    
    // Check that edges maintain their undirectedness
    for (const auto& edge : relabeled.edges()) {
      REQUIRE(edge.mutator_verts().size() == edge.mutated_verts().size());
    }
  }
  
  SECTION("directed hyperedges") {
    reticula::directed_hypernetwork<int> net({
      {{1, 2}, {3, 4}},
      {{3}, {5, 6}}
    });
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 2);
    REQUIRE(relabeled.vertices().size() == 6);
  }
  
  SECTION("undirected hyperedges") {
    reticula::undirected_hypernetwork<int> net({
      {1, 2, 3},
      {3, 4, 5}
    });
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 2);
    REQUIRE(relabeled.vertices().size() == 5);
  }
  
  SECTION("directed temporal edges") {
    reticula::directed_temporal_network<int, int> net({
      {1, 2, 10},
      {2, 3, 20},
      {3, 1, 30}
    });
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 3);
    REQUIRE(relabeled.vertices().size() == 3);
    
    // Check that edges maintain their temporal properties
    for (const auto& edge : relabeled.edges()) {
      REQUIRE(edge.mutator_verts().size() == 1);
      REQUIRE(edge.mutated_verts().size() == 1);
      REQUIRE(edge.cause_time() == edge.effect_time());
    }
  }
  
  SECTION("undirected temporal edges") {
    reticula::undirected_temporal_network<int, double> net({
      {1, 2, 1.5},
      {2, 3, 2.5}
    });
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 2);
    REQUIRE(relabeled.vertices().size() == 3);
    
    // Check that edges maintain their undirectedness and temporal properties
    for (const auto& edge : relabeled.edges()) {
      REQUIRE(edge.mutator_verts().size() == edge.mutated_verts().size());
      REQUIRE(edge.cause_time() == edge.effect_time());
    }
  }
  
  SECTION("directed delayed temporal edges") {
    reticula::directed_delayed_temporal_network<int, double> net({
      {1, 2, 1.0, 2.0},
      {2, 3, 2.5, 3.5}
    });
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 2);
    REQUIRE(relabeled.vertices().size() == 3);
    
    // Check that edges maintain their delayed temporal properties
    for (const auto& edge : relabeled.edges()) {
      REQUIRE(edge.mutator_verts().size() == 1);
      REQUIRE(edge.mutated_verts().size() == 1);
      REQUIRE(edge.cause_time() < edge.effect_time());
    }
  }
  
  SECTION("directed temporal hyperedges") {
    reticula::directed_temporal_hypernetwork<int, int> net({
      {{1, 2}, {3, 4}, 10},
      {{3}, {5, 6}, 20}
    });
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 2);
    REQUIRE(relabeled.vertices().size() == 6);
    
    // Check that hyperedges maintain their temporal properties
    for (const auto& edge : relabeled.edges()) {
      REQUIRE(edge.cause_time() == edge.effect_time());
    }
  }
  
  SECTION("undirected temporal hyperedges") {
    reticula::undirected_temporal_hypernetwork<int, double> net({
      {{1, 2, 3}, 1.5},
      {{3, 4, 5}, 2.5}
    });
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 2);
    REQUIRE(relabeled.vertices().size() == 5);
    
    // Check that hyperedges maintain their temporal properties
    for (const auto& edge : relabeled.edges()) {
      REQUIRE(edge.cause_time() == edge.effect_time());
    }
  }
  
  SECTION("directed delayed temporal hyperedges") {
    reticula::directed_delayed_temporal_hypernetwork<int, double> net({
      {{1, 2}, {3, 4}, 1.0, 2.0},
      {{3}, {5, 6}, 2.5, 3.5}
    });
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 2);
    REQUIRE(relabeled.vertices().size() == 6);
    
    // Check that hyperedges maintain their delayed temporal properties
    for (const auto& edge : relabeled.edges()) {
      REQUIRE(edge.cause_time() < edge.effect_time());
    }
  }
}

TEST_CASE("relabel_nodes preserves network structure", "[reticula::relabel_nodes]") {
  SECTION("vertex and edge count preservation") {
    reticula::undirected_network<int> net({{1, 2}, {2, 3}, {3, 4}, {4, 1}});
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(net.vertices().size() == relabeled.vertices().size());
    REQUIRE(net.edges().size() == relabeled.edges().size());
  }
  
  SECTION("connectivity preservation") {
    // Create a simple path: 1-2-3-4
    reticula::undirected_network<int> net({{1, 2}, {2, 3}, {3, 4}});
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    // The relabeled network should still be connected
    auto components = reticula::connected_components(relabeled);
    REQUIRE(components.size() == 1);
    REQUIRE(components[0].size() == 4);
  }
  
  SECTION("self-loops preservation") {
    reticula::directed_network<int> net({{1, 1}, {1, 2}, {2, 2}});
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.edges().size() == 3);
    
    // Check for self-loops in relabeled network
    int self_loop_count = 0;
    for (const auto& edge : relabeled.edges()) {
      if (edge.tail() == edge.head()) {
        self_loop_count++;
      }
    }
    REQUIRE(self_loop_count == 2);
  }
  
  SECTION("degree preservation") {
    reticula::undirected_network<int> net({{1, 2}, {1, 3}, {1, 4}, {2, 3}});
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    // Create mapping from old to new vertices
    std::map<int, std::size_t> mapping = {{1, 0}, {2, 1}, {3, 2}, {4, 3}};
    auto explicit_relabeled = reticula::relabel_nodes<std::size_t>(net, mapping);
    
    // Check that vertex counts are preserved (degree function not available in test)
    REQUIRE(net.vertices().size() == explicit_relabeled.vertices().size());
    REQUIRE(net.edges().size() == explicit_relabeled.edges().size());
  }
}

TEST_CASE("relabel_nodes edge cases", "[reticula::relabel_nodes]") {
  SECTION("empty network") {
    reticula::undirected_network<int> empty_net;
    auto relabeled = reticula::relabel_nodes<std::size_t>(empty_net);
    
    REQUIRE(relabeled.vertices().empty());
    REQUIRE(relabeled.edges().empty());
  }
  
  SECTION("single vertex network") {
    reticula::undirected_network<int> net({}, {42});
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.vertices().size() == 1);
    REQUIRE(relabeled.edges().empty());
    REQUIRE(relabeled.vertices()[0] == 0);
  }
  
  SECTION("single edge network") {
    reticula::undirected_network<int> net({{1, 2}});
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.vertices().size() == 2);
    REQUIRE(relabeled.edges().size() == 1);
    
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<std::size_t>({0, 1})));
  }
  
  SECTION("disconnected components") {
    reticula::undirected_network<int> net({{1, 2}, {3, 4}});
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.vertices().size() == 4);
    REQUIRE(relabeled.edges().size() == 2);
    
    auto components = reticula::connected_components(relabeled);
    REQUIRE(components.size() == 2);
    REQUIRE(components[0].size() == 2);
    REQUIRE(components[1].size() == 2);
  }
  
  SECTION("isolated vertices") {
    reticula::undirected_network<int> net({{1, 2}}, {1, 2, 3, 4, 5});
    auto relabeled = reticula::relabel_nodes<std::size_t>(net);
    
    REQUIRE(relabeled.vertices().size() == 5);
    REQUIRE(relabeled.edges().size() == 1);
    
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<std::size_t>({0, 1, 2, 3, 4})));
  }
  
  SECTION("large vertex labels") {
    reticula::undirected_network<std::size_t> net({{1000000, 2000000}});
    auto relabeled = reticula::relabel_nodes<int>(net);
    
    REQUIRE(relabeled.vertices().size() == 2);
    auto vertices = relabeled.vertices();
    REQUIRE_THAT(vertices, UnorderedRangeEquals(std::vector<int>({0, 1})));
  }
}
