#include <sstream>
#include <string>

#include <catch2/catch.hpp>
using Catch::Matchers::Equals;
using Catch::Matchers::UnorderedEquals;

#include <dag/static_edges.hpp>
#include <dag/temporal_edges.hpp>

#include <dag/networks.hpp>

TEST_CASE("undirected networks", "[dag::undirected_network]") {
  SECTION("when given one") {
    dag::undirected_network<int> graph(
        {{1, 2}, {1, 5}, {5, 2}, {4, 5}, {3, 2}, {4, 3}, {4, 6},
         {1, 2}, {2, 1}, {5, 2}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(3),
          UnorderedEquals(std::vector<int>({2, 4})));
      REQUIRE_THAT(graph.predecessors(3),
          UnorderedEquals(std::vector<int>({2, 4})));
      REQUIRE_THAT(graph.neighbours(3),
          UnorderedEquals(std::vector<int>({2, 4})));

      REQUIRE_THAT(graph.out_edges(3),
          UnorderedEquals(
            std::vector<dag::undirected_edge<int>>({{3, 4}, {3, 2}})));
      REQUIRE_THAT(graph.in_edges(3),
          UnorderedEquals(
            std::vector<dag::undirected_edge<int>>({{3, 4}, {3, 2}})));
      REQUIRE_THAT(graph.incident_edges(3),
          UnorderedEquals(
            std::vector<dag::undirected_edge<int>>({{3, 4}, {3, 2}})));

      REQUIRE(graph.out_degree(3) == 2);
      REQUIRE(graph.in_degree(3) == 2);
      REQUIRE(graph.degree(3) == 2);

      std::vector<dag::undirected_edge<int>> edges({
          {1, 2}, {1, 5}, {5, 2}, {4, 5}, {3, 2}, {4, 3}, {4, 6}});
      std::sort(edges.begin(), edges.end());

      REQUIRE_THAT(graph.edges(), Equals(edges));
      REQUIRE_THAT(graph.edges_cause(), Equals(edges));

      std::sort(edges.begin(), edges.end(),
          [](const dag::undirected_edge<int>& a,
            const dag::undirected_edge<int>& b) {
            return dag::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), Equals(edges));

      // test sorted output as well
      REQUIRE_THAT(graph.vertices(),
          Equals(std::vector<int>({0, 1, 2, 3, 4, 5, 6})));
    }
  }
}

TEST_CASE("undirected hypernetworks", "[dag::undirected_hypernetwork]") {
  SECTION("when given one") {
    using U = dag::undirected_hyperedge<int>;
    dag::undirected_hypernetwork<int> graph(
        {U{1, 2}, U{1, 5}, U{5, 2}, U{4, 5}, U{3, 2}, U{4, 3}, U{4, 6},
         U{1, 2}, U{2, 1}, U{5, 2}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(3),
          UnorderedEquals(std::vector<int>({2, 4})));
      REQUIRE_THAT(graph.predecessors(3),
          UnorderedEquals(std::vector<int>({2, 4})));
      REQUIRE_THAT(graph.neighbours(3),
          UnorderedEquals(std::vector<int>({2, 4})));

      REQUIRE_THAT(graph.out_edges(3),
          UnorderedEquals(
            std::vector<dag::undirected_hyperedge<int>>({U{3, 4}, U{3, 2}})));
      REQUIRE_THAT(graph.in_edges(3),
          UnorderedEquals(
            std::vector<dag::undirected_hyperedge<int>>({U{3, 4}, U{3, 2}})));
      REQUIRE_THAT(graph.incident_edges(3),
          UnorderedEquals(
            std::vector<dag::undirected_hyperedge<int>>({U{3, 4}, U{3, 2}})));

      REQUIRE(graph.out_degree(3) == 2);
      REQUIRE(graph.in_degree(3) == 2);
      REQUIRE(graph.degree(3) == 2);

      std::vector<dag::undirected_hyperedge<int>> edges({
          U{1, 2}, U{1, 5}, U{5, 2}, U{4, 5}, U{3, 2}, U{4, 3}, U{4, 6}});
      std::sort(edges.begin(), edges.end());

      REQUIRE_THAT(graph.edges(), Equals(edges));
      REQUIRE_THAT(graph.edges_cause(), Equals(edges));

      std::sort(edges.begin(), edges.end(),
          [](const dag::undirected_hyperedge<int>& a,
            const dag::undirected_hyperedge<int>& b) {
            return dag::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), Equals(edges));

      // test sorted output as well
      REQUIRE_THAT(graph.vertices(),
          Equals(std::vector<int>({0, 1, 2, 3, 4, 5, 6})));
    }
  }
}

TEST_CASE("directed networks", "[dag::directed_network]") {
  SECTION("when given one") {
    dag::directed_network<int> graph(
        {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2},
         {1, 2}, {2, 3}, {3, 5}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedEquals(std::vector<int>({3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedEquals(std::vector<int>({1, 4})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedEquals(std::vector<int>({1, 3, 4})));

      REQUIRE_THAT(graph.out_edges(2),
          UnorderedEquals(
            std::vector<dag::directed_edge<int>>({{2, 3}})));
      REQUIRE_THAT(graph.in_edges(2),
          UnorderedEquals(
            std::vector<dag::directed_edge<int>>({{4, 2}, {1, 2}})));
      REQUIRE_THAT(graph.incident_edges(2),
          UnorderedEquals(
            std::vector<dag::directed_edge<int>>({{4, 2}, {1, 2}, {2, 3}})));

      REQUIRE(graph.out_degree(2) == 1);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 3);

      std::vector<dag::directed_edge<int>> edges(
          {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}});
      std::sort(edges.begin(), edges.end());

      REQUIRE_THAT(graph.edges(), Equals(edges));
      REQUIRE_THAT(graph.edges_cause(), Equals(edges));

      std::sort(edges.begin(), edges.end(),
          [](const dag::directed_edge<int>& a,
            const dag::directed_edge<int>& b) {
            return dag::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), Equals(edges));


      REQUIRE_THAT(graph.vertices(),
          Equals(std::vector<int>({0, 1, 2, 3, 4, 5, 6})));
    }
  }
}

TEST_CASE("directed hypernetworks", "[dag::directed_network]") {
  SECTION("when given one") {
    dag::directed_hypernetwork<int> graph(
        {{{1}, {2}}, {{2}, {3}}, {{3}, {5}}, {{5}, {6}}, {{5}, {4}},
          {{4}, {2}}, {{1}, {2}}, {{2}, {3}}, {{3}, {5}}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedEquals(std::vector<int>({3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedEquals(std::vector<int>({1, 4})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedEquals(std::vector<int>({1, 3, 4})));

      REQUIRE_THAT(graph.out_edges(2),
          UnorderedEquals(
            std::vector<dag::directed_hyperedge<int>>(
              {{{2}, {3}}})));
      REQUIRE_THAT(graph.in_edges(2),
          UnorderedEquals(
            std::vector<dag::directed_hyperedge<int>>(
              {{{4}, {2}}, {{1}, {2}}})));
      REQUIRE_THAT(graph.incident_edges(2),
          UnorderedEquals(
            std::vector<dag::directed_hyperedge<int>>(
              {{{4}, {2}}, {{1}, {2}}, {{2}, {3}}})));

      REQUIRE(graph.out_degree(2) == 1);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 3);

      std::vector<dag::directed_hyperedge<int>> edges(
          {{{1}, {2}}, {{2}, {3}}, {{3}, {5}}, {{5}, {6}}, {{5}, {4}},
          {{4}, {2}}});
      std::sort(edges.begin(), edges.end());

      REQUIRE_THAT(graph.edges(), Equals(edges));
      REQUIRE_THAT(graph.edges_cause(), Equals(edges));

      std::sort(edges.begin(), edges.end(),
          [](const dag::directed_hyperedge<int>& a,
            const dag::directed_hyperedge<int>& b) {
            return dag::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), Equals(edges));


      REQUIRE_THAT(graph.vertices(),
          Equals(std::vector<int>({0, 1, 2, 3, 4, 5, 6})));
    }
  }
}

TEST_CASE("undirected temporal networks",
    "[dag::undirected_temporal_network]") {
  SECTION("when given one") {
    dag::undirected_temporal_network<int, int> graph(
        {{2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1},
         {2, 1, 2}, {2, 1, 2}, {1, 2, 5}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          Equals(std::vector<dag::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}})));
      REQUIRE_THAT(graph.in_edges(2),
          Equals(std::vector<dag::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}})));
      REQUIRE_THAT(graph.incident_edges(2),
          Equals(std::vector<dag::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}})));

      REQUIRE(graph.out_degree(2) == 4);
      REQUIRE(graph.in_degree(2) == 4);
      REQUIRE(graph.degree(2) == 4);

      REQUIRE_THAT(graph.edges(),
          Equals(
            std::vector<dag::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));

      REQUIRE_THAT(graph.edges_cause(),
          Equals(
            std::vector<dag::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));

      REQUIRE_THAT(graph.edges_effect(),
          Equals(
            std::vector<dag::undirected_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}})));

      REQUIRE_THAT(graph.vertices(),
          UnorderedEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }
  }
}

TEST_CASE("undirected temporal hypernetworks",
    "[dag::undirected_temporal_hypernetwork]") {
  SECTION("when given one") {
    dag::undirected_temporal_hypernetwork<int, int> graph(
        {{{2, 3}, 6}, {{2, 3}, 6}, {{3, 4}, 8}, {{1, 2}, 1},
         {{2, 1}, 2}, {{2, 1}, 2}, {{1, 2}, 5}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          Equals(std::vector<dag::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6}})));
      REQUIRE_THAT(graph.in_edges(2),
          Equals(std::vector<dag::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6}})));
      REQUIRE_THAT(graph.incident_edges(2),
          Equals(std::vector<dag::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6}})));

      REQUIRE(graph.out_degree(2) == 4);
      REQUIRE(graph.in_degree(2) == 4);
      REQUIRE(graph.degree(2) == 4);

      REQUIRE_THAT(graph.edges(),
          Equals(
            std::vector<dag::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6},
              {{3, 4}, 8}})));

      REQUIRE_THAT(graph.edges_cause(),
          Equals(
            std::vector<dag::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6},
              {{3, 4}, 8}})));

      REQUIRE_THAT(graph.edges_effect(),
          Equals(
            std::vector<dag::undirected_temporal_hyperedge<int, int>>(
              {{{1, 2}, 1}, {{2, 1}, 2}, {{1, 2}, 5}, {{2, 3}, 6},
              {{3, 4}, 8}})));

      REQUIRE_THAT(graph.vertices(),
          UnorderedEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }
  }
}

TEST_CASE("directed temporal networks",
    "[dag::directed_temporal_network]") {
  SECTION("when given one") {
    dag::directed_temporal_network<int, int> graph(
        {{2, 3, 6}, {2, 3, 6}, {3, 4, 8}, {1, 2, 1},
         {2, 1, 2}, {2, 1, 2}, {1, 2, 5}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedEquals(std::vector<int>({1})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          Equals(std::vector<dag::directed_temporal_edge<int, int>>(
              {{2, 1, 2}, {2, 3, 6}})));
      REQUIRE_THAT(graph.in_edges(2),
          Equals(std::vector<dag::directed_temporal_edge<int, int>>(
              {{1, 2, 1}, {1, 2, 5}})));
      REQUIRE_THAT(graph.incident_edges(2),
          Equals(std::vector<dag::directed_temporal_edge<int, int>>(
              {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}})));

      REQUIRE(graph.out_degree(2) == 2);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 4);

      std::vector<dag::directed_temporal_edge<int, int>> edges(
          {{1, 2, 1}, {2, 1, 2}, {1, 2, 5}, {2, 3, 6}, {3, 4, 8}});
      std::sort(edges.begin(), edges.end());

      REQUIRE_THAT(graph.edges(), Equals(edges));
      REQUIRE_THAT(graph.edges_cause(), Equals(edges));

      std::sort(edges.begin(), edges.end(),
          [](const dag::directed_temporal_edge<int, int>& a,
            const dag::directed_temporal_edge<int, int>& b) {
            return dag::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), Equals(edges));

      REQUIRE_THAT(graph.vertices(),
          UnorderedEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }
  }
}

TEST_CASE("directed temporal hypernetworks",
    "[dag::directed_temporal_hypernetwork]") {
  SECTION("when given one") {
    dag::directed_temporal_hypernetwork<int, int> graph(
        {{{2}, {3}, 6}, {{2}, {3}, 6}, {{3}, {4}, 8}, {{1}, {2}, 1},
         {{2}, {1}, 2}, {{2}, {1}, 2}, {{1}, {2}, 5}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedEquals(std::vector<int>({1})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          Equals(std::vector<dag::directed_temporal_hyperedge<int, int>>(
              {{{2}, {1}, 2}, {{2}, {3}, 6}})));
      REQUIRE_THAT(graph.in_edges(2),
          Equals(std::vector<dag::directed_temporal_hyperedge<int, int>>(
              {{{1}, {2}, 1}, {{1}, {2}, 5}})));
      REQUIRE_THAT(graph.incident_edges(2),
          Equals(std::vector<dag::directed_temporal_hyperedge<int, int>>(
              {{{1}, {2}, 1}, {{2}, {1}, 2}, {{1}, {2}, 5}, {{2}, {3}, 6}})));

      REQUIRE(graph.out_degree(2) == 2);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 4);

      std::vector<dag::directed_temporal_hyperedge<int, int>> edges(
          {{{1}, {2}, 1}, {{2}, {1}, 2}, {{1}, {2}, 5}, {{2}, {3}, 6},
          {{3}, {4}, 8}});
      std::sort(edges.begin(), edges.end());

      REQUIRE_THAT(graph.edges(), Equals(edges));
      REQUIRE_THAT(graph.edges_cause(), Equals(edges));

      std::sort(edges.begin(), edges.end(),
          [](const dag::directed_temporal_hyperedge<int, int>& a,
            const dag::directed_temporal_hyperedge<int, int>& b) {
            return dag::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), Equals(edges));

      REQUIRE_THAT(graph.vertices(),
          UnorderedEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }
  }
}


TEST_CASE("directed delayed temporal networks",
    "[dag::directed_delayed_temporal_network]") {
  SECTION("when given one") {
    dag::directed_delayed_temporal_network<int, int> graph(
        {{2, 3, 6, 7}, {3, 4, 8, 9}, {1, 2, 1, 9}, {2, 1, 2, 3}, {1, 2, 5, 8}},
        {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedEquals(std::vector<int>({1})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          Equals(std::vector<dag::directed_delayed_temporal_edge<int, int>>(
              {{2, 1, 2, 3}, {2, 3, 6, 7}})));
      REQUIRE_THAT(graph.in_edges(2),
          Equals(std::vector<dag::directed_delayed_temporal_edge<int, int>>(
              {{1, 2, 5, 8}, {1, 2, 1, 9}})));
      REQUIRE_THAT(graph.incident_edges(2),
          Equals(std::vector<dag::directed_delayed_temporal_edge<int, int>>(
              {{1, 2, 1, 9}, {2, 1, 2, 3}, {1, 2, 5, 8}, {2, 3, 6, 7}})));

      REQUIRE(graph.out_degree(2) == 2);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 4);

      std::vector<dag::directed_delayed_temporal_edge<int, int>> edges(
          {{1, 2, 1, 9}, {2, 1, 2, 3}, {1, 2, 5, 8}, {2, 3, 6, 7},
               {3, 4, 8, 9}});
      std::sort(edges.begin(), edges.end());

      REQUIRE_THAT(graph.edges(), Equals(edges));
      REQUIRE_THAT(graph.edges_cause(), Equals(edges));

      std::sort(edges.begin(), edges.end(),
          [](const dag::directed_delayed_temporal_edge<int, int>& a,
            const dag::directed_delayed_temporal_edge<int, int>& b) {
            return dag::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), Equals(edges));

      REQUIRE_THAT(graph.vertices(),
          UnorderedEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }
  }
}

TEST_CASE("directed delayed temporal hypernetworks",
    "[dag::directed_delayed_temporal_hypernetwork]") {
  SECTION("when given one") {
    dag::directed_delayed_temporal_hypernetwork<int, int> graph(
        {{{2}, {3}, 6, 7}, {{3}, {4}, 8, 9}, {{1}, {2}, 1, 9}, {{2}, {1}, 2, 3},
        {{1}, {2}, 5, 8}}, {0});

    SECTION("basic properties are correct") {
      REQUIRE_THAT(graph.successors(2),
          UnorderedEquals(std::vector<int>({1, 3})));
      REQUIRE_THAT(graph.predecessors(2),
          UnorderedEquals(std::vector<int>({1})));
      REQUIRE_THAT(graph.neighbours(2),
          UnorderedEquals(std::vector<int>({1, 3})));

      REQUIRE_THAT(graph.out_edges(2),
          Equals(std::vector<
            dag::directed_delayed_temporal_hyperedge<int, int>>(
              {{{2}, {1}, 2, 3}, {{2}, {3}, 6, 7}})));
      REQUIRE_THAT(graph.in_edges(2),
          Equals(std::vector<
            dag::directed_delayed_temporal_hyperedge<int, int>>(
              {{{1}, {2}, 5, 8}, {{1}, {2}, 1, 9}})));
      REQUIRE_THAT(graph.incident_edges(2),
          Equals(std::vector<
            dag::directed_delayed_temporal_hyperedge<int, int>>(
              {{{1}, {2}, 1, 9}, {{2}, {1}, 2, 3},
              {{1}, {2}, 5, 8}, {{2}, {3}, 6, 7}})));

      REQUIRE(graph.out_degree(2) == 2);
      REQUIRE(graph.in_degree(2) == 2);
      REQUIRE(graph.degree(2) == 4);

      std::vector<dag::directed_delayed_temporal_hyperedge<int, int>> edges(
          {{{1}, {2}, 1, 9}, {{2}, {1}, 2, 3}, {{1}, {2}, 5, 8},
          {{2}, {3}, 6, 7}, {{3}, {4}, 8, 9}});
      std::sort(edges.begin(), edges.end());

      REQUIRE_THAT(graph.edges(), Equals(edges));
      REQUIRE_THAT(graph.edges_cause(), Equals(edges));

      std::sort(edges.begin(), edges.end(),
          [](const dag::directed_delayed_temporal_hyperedge<int, int>& a,
            const dag::directed_delayed_temporal_hyperedge<int, int>& b) {
            return dag::effect_lt(a, b);
          });

      REQUIRE_THAT(graph.edges_effect(), Equals(edges));

      REQUIRE_THAT(graph.vertices(),
          UnorderedEquals(
            std::vector<int>({0, 1, 2, 3, 4})));
    }
  }
}

TEST_CASE("higher-order networks", "[dag::network]") {
  using HyperEventGraph =
    dag::network<
      dag::directed_edge<
        dag::directed_temporal_hyperedge<int, float>>>;
  REQUIRE_NOTHROW(HyperEventGraph({}, {}));
}
