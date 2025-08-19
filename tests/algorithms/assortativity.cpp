#include "reticula/concepts.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
using Catch::Approx;

#include <reticula/algorithms/assortativity.hpp>
#include <reticula/edges.hpp>
#include <reticula/networks.hpp>

TEST_CASE("degree assortativity", "[reticula::degree_assortativity]") {
  SECTION("undirected") {
    std::vector<reticula::undirected_edge> edges;
    for (reticula::VertexType i = 0; i < 10; ++i)
      for (reticula::VertexType j = i + 1; j < 10; ++j)
        edges.emplace_back(i, j);
    auto g = reticula::undirected_network(edges, std::views::iota(0uz, 10uz));

    REQUIRE(std::isnan(reticula::degree_assortativity(g)));

    reticula::undirected_network karate_club(
      {{1, 2},   {1, 3},   {2, 3},   {1, 4},   {2, 4},   {3, 4},   {1, 5},
       {1, 6},   {1, 7},   {5, 7},   {6, 7},   {1, 8},   {2, 8},   {3, 8},
       {4, 8},   {1, 9},   {3, 9},   {3, 10},  {1, 11},  {5, 11},  {6, 11},
       {1, 12},  {1, 13},  {4, 13},  {1, 14},  {2, 14},  {3, 14},  {4, 14},
       {6, 17},  {7, 17},  {1, 18},  {2, 18},  {1, 20},  {2, 20},  {1, 22},
       {2, 22},  {24, 26}, {25, 26}, {3, 28},  {24, 28}, {25, 28}, {3, 29},
       {24, 30}, {27, 30}, {2, 31},  {9, 31},  {1, 32},  {25, 32}, {26, 32},
       {29, 32}, {3, 33},  {9, 33},  {15, 33}, {16, 33}, {19, 33}, {21, 33},
       {23, 33}, {24, 33}, {30, 33}, {31, 33}, {32, 33}, {9, 34},  {10, 34},
       {14, 34}, {15, 34}, {16, 34}, {19, 34}, {20, 34}, {21, 34}, {23, 34},
       {24, 34}, {27, 34}, {28, 34}, {29, 34}, {30, 34}, {31, 34}, {32, 34},
       {33, 34}});

    REQUIRE(
      reticula::degree_assortativity(karate_club) ==
      Approx(-0.4756).margin(1e-4));

    reticula::undirected_network zebra(
      {{1, 2},   {1, 3},   {1, 4},   {1, 5},   {1, 6},   {1, 7},   {1, 8},
       {1, 9},   {1, 10},  {1, 11},  {1, 12},  {1, 13},  {1, 14},  {2, 3},
       {2, 4},   {2, 6},   {2, 7},   {2, 8},   {2, 9},   {2, 10},  {2, 11},
       {2, 12},  {2, 13},  {2, 14},  {2, 15},  {3, 4},   {3, 6},   {3, 7},
       {3, 8},   {3, 9},   {3, 10},  {3, 11},  {3, 12},  {3, 13},  {3, 14},
       {3, 15},  {4, 5},   {4, 6},   {4, 7},   {4, 8},   {4, 9},   {4, 10},
       {4, 11},  {4, 12},  {4, 14},  {4, 15},  {5, 8},   {6, 7},   {6, 8},
       {6, 9},   {6, 10},  {6, 11},  {6, 12},  {6, 13},  {6, 15},  {7, 9},
       {7, 10},  {7, 11},  {7, 12},  {7, 13},  {7, 14},  {7, 15},  {8, 9},
       {8, 10},  {8, 11},  {8, 12},  {8, 13},  {8, 14},  {8, 15},  {9, 10},
       {9, 11},  {9, 12},  {9, 13},  {9, 14},  {9, 15},  {10, 11}, {10, 12},
       {10, 13}, {10, 15}, {10, 16}, {11, 12}, {11, 13}, {11, 14}, {11, 15},
       {12, 13}, {12, 14}, {12, 15}, {13, 15}, {13, 17}, {13, 22}, {13, 23},
       {14, 15}, {14, 16}, {16, 17}, {16, 18}, {16, 19}, {16, 20}, {16, 21},
       {17, 18}, {17, 22}, {17, 23}, {19, 20}, {19, 21}, {20, 21}, {22, 23},
       {24, 25}, {24, 26}, {24, 27}, {25, 26}, {25, 27}, {26, 27}});
    REQUIRE(
      reticula::degree_assortativity(zebra) == Approx(0.7177).margin(1e-4));
  }

  SECTION("directed") {
    std::vector<reticula::directed_edge> edges;
    for (reticula::VertexType i = 0; i < 10; ++i)
      for (reticula::VertexType j = 0; j < 10; ++j)
        if (i != j)
          edges.emplace_back(i, j);
    auto g = reticula::directed_network(edges, std::views::iota(0uz, 10uz));

    REQUIRE(
      std::isnan(
        reticula::degree_assortativity(
          g, reticula::direction::in, reticula::direction::in)));
    REQUIRE(
      std::isnan(
        reticula::degree_assortativity(
          g, reticula::direction::in, reticula::direction::out)));
    REQUIRE(
      std::isnan(
        reticula::degree_assortativity(
          g, reticula::direction::out, reticula::direction::in)));
    REQUIRE(
      std::isnan(
        reticula::degree_assortativity(
          g, reticula::direction::out, reticula::direction::out)));

    reticula::directed_network rhesus_macaques(
      {{1, 2},   {1, 3},   {4, 2},   {5, 6},   {5, 7},   {5, 8},   {5, 3},
       {5, 9},   {5, 10},  {5, 11},  {5, 12},  {5, 13},  {5, 14},  {6, 1},
       {6, 5},   {6, 8},   {6, 3},   {6, 10},  {6, 12},  {6, 15},  {7, 5},
       {7, 8},   {7, 2},   {7, 9},   {7, 14},  {16, 2},  {16, 3},  {16, 9},
       {16, 11}, {16, 12}, {16, 15}, {8, 6},   {8, 7},   {2, 1},   {2, 4},
       {2, 16},  {2, 3},   {2, 9},   {2, 10},  {2, 11},  {2, 12},  {2, 15},
       {3, 1},   {3, 2},   {3, 9},   {3, 10},  {3, 11},  {3, 12},  {3, 15},
       {3, 13},  {9, 2},   {9, 3},   {9, 11},  {9, 15},  {9, 13},  {9, 14},
       {10, 1},  {10, 5},  {10, 6},  {10, 2},  {10, 3},  {10, 9},  {10, 11},
       {10, 12}, {10, 15}, {10, 13}, {11, 1},  {11, 5},  {11, 16}, {11, 2},
       {11, 3},  {11, 9},  {11, 10}, {11, 15}, {11, 13}, {12, 5},  {12, 6},
       {12, 8},  {12, 2},  {12, 3},  {12, 10}, {12, 11}, {12, 15}, {12, 13},
       {12, 14}, {15, 4},  {15, 16}, {15, 2},  {15, 3},  {15, 10}, {15, 11},
       {15, 12}, {15, 14}, {13, 1},  {13, 5},  {13, 3},  {13, 9},  {13, 10},
       {13, 11}, {13, 12}, {13, 15}, {14, 4},  {14, 7},  {14, 8},  {14, 2},
       {14, 3},  {14, 9},  {14, 10}, {14, 12}, {14, 15}, {14, 13}});
    REQUIRE(
      reticula::degree_assortativity(
        rhesus_macaques, reticula::direction::in, reticula::direction::in) ==
      Approx(-0.0046).margin(1e-4));
    REQUIRE(
      reticula::degree_assortativity(
        rhesus_macaques, reticula::direction::in, reticula::direction::out) ==
      Approx(0.0203).margin(1e-4));
    REQUIRE(
      reticula::degree_assortativity(
        rhesus_macaques, reticula::direction::out, reticula::direction::in) ==
      Approx(-0.0914).margin(1e-4));
    REQUIRE(
      reticula::degree_assortativity(
        rhesus_macaques, reticula::direction::out, reticula::direction::out) ==
      Approx(-0.0583).margin(1e-4));
  }
}

TEST_CASE("attribute assortativity", "[reticula::attribute_assortativity]") {
  SECTION("undirected") {
    reticula::undirected_network net({{0, 1}, {2, 3}});
    reticula::undirected_network net2({{0, 2}, {1, 3}});

    SECTION("with function") {
      REQUIRE(
        reticula::attribute_assortativity(net, [](reticula::VertexType v) {
          return v < 2 ? 1 : 2;
        }) == Approx(1.0));

      REQUIRE(
        reticula::attribute_assortativity(net2, [](reticula::VertexType v) {
          return v < 2 ? 1 : 2;
        }) == Approx(-1.0));

      REQUIRE(
        reticula::attribute_assortativity(
          net, [](reticula::VertexType v) { return v; }) == Approx(0.6));

      REQUIRE(
        reticula::attribute_assortativity(
          net2, [](reticula::VertexType v) { return v; }) == Approx(-0.6));
    }

    SECTION("with mapping") {
      std::unordered_map<reticula::VertexType, double> m{
        {1, 1}, {2, 2}, {3, 2}};
      REQUIRE(reticula::attribute_assortativity(net, m, 1.0) == Approx(1.0));

      REQUIRE(reticula::attribute_assortativity(net2, m, 1.0) == Approx(-1.0));

      std::unordered_map<reticula::VertexType, double> m2{
        {1, 1}, {2, 2}, {3, 3}};
      REQUIRE(reticula::attribute_assortativity(net, m2, 0.0) == Approx(0.6));

      REQUIRE(reticula::attribute_assortativity(net2, m2, 0.0) == Approx(-0.6));
    }
  }

  SECTION("directed") {
    reticula::directed_network net({{0, 1}, {2, 3}, {0, 3}});
    reticula::directed_network net2({{0, 2}, {3, 1}, {0, 3}});

    SECTION("with function") {
      REQUIRE(
        reticula::attribute_assortativity(
          net, [](reticula::VertexType v) { return v < 2 ? 1 : 2; },
          [](reticula::VertexType v) { return v < 2 ? 2 : 3; }) == Approx(0.5));

      REQUIRE(
        reticula::attribute_assortativity(
          net2, [](reticula::VertexType v) { return v < 2 ? 1 : 2; },
          [](reticula::VertexType v) { return v < 2 ? 2 : 3; }) ==
        Approx(-1.0));

      REQUIRE(
        reticula::attribute_assortativity(
          net, [](reticula::VertexType v) { return v; },
          [](reticula::VertexType v) { return v + 1; }) == Approx(0.5));
      REQUIRE(
        reticula::attribute_assortativity(
          net2, [](reticula::VertexType v) { return v; },
          [](reticula::VertexType v) { return v + 1; }) ==
        Approx(-std::sqrt(3) / 2));
    }

    SECTION("with mapping") {
      std::unordered_map<reticula::VertexType, double> m_in{
        {1, 1}, {2, 2}, {3, 2}};
      std::unordered_map<reticula::VertexType, double> m_out{
        {1, 2}, {2, 3}, {3, 3}};
      REQUIRE(
        reticula::attribute_assortativity(net, m_in, m_out, 1.0, 2.0) ==
        Approx(0.5));

      REQUIRE(
        reticula::attribute_assortativity(net2, m_in, m_out, 1.0, 2.0) ==
        Approx(-1.0));

      std::unordered_map<reticula::VertexType, double> m2_in{
        {1, 1}, {2, 2}, {3, 3}};
      std::unordered_map<reticula::VertexType, double> m2_out{
        {1, 2}, {2, 3}, {3, 4}};
      REQUIRE(
        reticula::attribute_assortativity(net, m2_in, m2_out, 0.0, 1.0) ==
        Approx(0.5));

      REQUIRE(
        reticula::attribute_assortativity(net2, m2_in, m2_out, 0.0, 1.0) ==
        Approx(-std::sqrt(3) / 2));
    }
  }
}
