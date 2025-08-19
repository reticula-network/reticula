#include "reticula/concepts.hpp"
#include <catch2/catch_test_macros.hpp>

#include <reticula/algorithms/distance.hpp>
#include <reticula/networks.hpp>
#include <vector>

TEST_CASE(
  "shortest path lengths from", "[reticula::shortest_path_lengths_from]") {
  reticula::directed_network net(
    {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}}, {12});

  REQUIRE(
    reticula::shortest_path_lengths_from(net, 1) ==
    std::unordered_map<reticula::VertexType, std::size_t>{
      {1, 0}, {2, 1}, {3, 2}, {5, 3}, {6, 4}, {4, 4}});

  REQUIRE(
    reticula::shortest_path_lengths_from(net, 12) ==
    std::unordered_map<reticula::VertexType, std::size_t>{{12, 0}});
}

TEST_CASE("shortest path lengths to", "[reticula::shortest_path_lengths_to]") {
  reticula::directed_network net(
    {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}, {4, 2}}, {12});

  REQUIRE(
    reticula::shortest_path_lengths_to(net, 4) ==
    std::unordered_map<reticula::VertexType, std::size_t>{
      {1, 4}, {2, 3}, {3, 2}, {5, 1}, {4, 0}});

  REQUIRE(
    reticula::shortest_path_lengths_to(net, 12) ==
    std::unordered_map<reticula::VertexType, std::size_t>{{12, 0}});
}

TEST_CASE("diameter", "[reticula::try_diameter][reticula::diameter]") {
  SECTION("works for null graphs and graphs with one vertex") {
    REQUIRE(
      reticula::try_diameter(reticula::directed_network()) == std::nullopt);
    REQUIRE(
      reticula::try_diameter(reticula::directed_network({}, {0})).value() == 0);

    REQUIRE(
      reticula::try_diameter(reticula::undirected_network()) == std::nullopt);
    REQUIRE(
      reticula::try_diameter(reticula::undirected_network({}, {0})).value() ==
      0);

    REQUIRE_THROWS_AS(
      reticula::diameter(reticula::directed_network()),
      reticula::distance_undefined_error);
    REQUIRE(reticula::diameter(reticula::directed_network({}, {0})) == 0);

    REQUIRE_THROWS_AS(
      reticula::diameter(reticula::undirected_network()),
      reticula::distance_undefined_error);
    REQUIRE(reticula::diameter(reticula::undirected_network({}, {0})) == 0);
  }

  SECTION("returns nullopt for disconnected graphs") {
    reticula::directed_network dir_disc(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}}, {12});
    REQUIRE(reticula::try_diameter(dir_disc) == std::nullopt);
    REQUIRE_THROWS_AS(
      reticula::diameter(dir_disc), reticula::distance_undefined_error);

    reticula::directed_network dir_weak(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 1}, {6, 12}});
    REQUIRE(reticula::try_diameter(dir_disc) == std::nullopt);
    REQUIRE_THROWS_AS(
      reticula::diameter(dir_weak), reticula::distance_undefined_error);

    reticula::undirected_network und_disc(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}}, {12});
    REQUIRE(reticula::try_diameter(und_disc) == std::nullopt);
    REQUIRE_THROWS_AS(
      reticula::diameter(und_disc), reticula::distance_undefined_error);
  }

  SECTION("works for directed networks") {
    reticula::directed_network net(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 1}});
    REQUIRE(reticula::try_diameter(net).value() == 5);
    REQUIRE(reticula::diameter(net) == 5);
  }

  SECTION("works for undirected networks") {
    reticula::undirected_network net1(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 1}});
    REQUIRE(reticula::try_diameter(net1).value() == 3);
    REQUIRE(reticula::diameter(net1) == 3);

    reticula::undirected_network net2({{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}});
    REQUIRE(reticula::try_diameter(net2).value() == 5);
    REQUIRE(reticula::diameter(net2) == 5);
  }
}

TEST_CASE(
  "eccentricity", "[reticula::try_out_eccentricity][reticula::out_eccentricity]"
                  "[reticula::try_in_eccentricity][reticula::in_eccentricity]"
                  "[reticula::try_eccentricity][reticula::eccentricity]") {

  SECTION("works for graphs with one vertex") {
    reticula::directed_network d1({}, {0});
    REQUIRE(reticula::try_out_eccentricity(d1, 0).value() == 0);
    REQUIRE(reticula::try_in_eccentricity(d1, 0).value() == 0);
    REQUIRE(reticula::out_eccentricity(d1, 0) == 0);
    REQUIRE(reticula::in_eccentricity(d1, 0) == 0);

    reticula::undirected_network u1({}, {0});
    REQUIRE(reticula::try_eccentricity(u1, 0).value() == 0);
    REQUIRE(reticula::eccentricity(u1, 0) == 0);
  }

  SECTION("returns nullopt / throws when undefined (directed & undirected)") {
    reticula::directed_network dir_disc(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}}, {12});
    for (auto v : dir_disc.vertices()) {
      REQUIRE_FALSE(reticula::try_out_eccentricity(dir_disc, v).has_value());
      REQUIRE_FALSE(reticula::try_in_eccentricity(dir_disc, v).has_value());
      REQUIRE_THROWS_AS(
        reticula::out_eccentricity(dir_disc, v),
        reticula::distance_undefined_error);
      REQUIRE_THROWS_AS(
        reticula::in_eccentricity(dir_disc, v),
        reticula::distance_undefined_error);
    }

    reticula::undirected_network und_disc({{1, 2}, {2, 3}, {10, 11}}, {99});
    for (auto v : und_disc.vertices()) {
      REQUIRE_FALSE(reticula::try_eccentricity(und_disc, v).has_value());
      REQUIRE_THROWS_AS(
        reticula::eccentricity(und_disc, v),
        reticula::distance_undefined_error);
    }
  }

  SECTION("works for directed reversed path 6->5->4->3->2->1") {
    reticula::directed_network path_rev(
      {{6, 5}, {5, 4}, {4, 3}, {3, 2}, {2, 1}});

    REQUIRE(reticula::try_out_eccentricity(path_rev, 6).value() == 5);
    REQUIRE(reticula::out_eccentricity(path_rev, 6) == 5);
    for (auto v : std::vector<reticula::VertexType>{5, 4, 3, 2, 1}) {
      REQUIRE_FALSE(reticula::try_out_eccentricity(path_rev, v).has_value());
      REQUIRE_THROWS_AS(
        reticula::out_eccentricity(path_rev, v),
        reticula::distance_undefined_error);
    }

    REQUIRE(reticula::try_in_eccentricity(path_rev, 1).value() == 5);
    REQUIRE(reticula::in_eccentricity(path_rev, 1) == 5);
    for (auto v : std::vector<reticula::VertexType>{2, 3, 4, 5}) {
      REQUIRE_FALSE(reticula::try_in_eccentricity(path_rev, v).has_value());
      REQUIRE_THROWS_AS(
        reticula::in_eccentricity(path_rev, v),
        reticula::distance_undefined_error);
    }
  }

  SECTION("works for directed SCCs with a one-way bridge (4<->3 -> 2<->1)") {
    reticula::directed_network bridge({{4, 3}, {3, 4}, {2, 1}, {1, 2}, {4, 2}});

    REQUIRE(reticula::try_out_eccentricity(bridge, 4).value() == 2);
    REQUIRE(reticula::try_out_eccentricity(bridge, 3).value() == 3);
    REQUIRE(reticula::out_eccentricity(bridge, 4) == 2);
    REQUIRE(reticula::out_eccentricity(bridge, 3) == 3);
    for (auto v : std::vector<reticula::VertexType>{1, 2}) {
      REQUIRE_FALSE(reticula::try_out_eccentricity(bridge, v).has_value());
      REQUIRE_THROWS_AS(
        reticula::out_eccentricity(bridge, v),
        reticula::distance_undefined_error);
    }

    REQUIRE(reticula::try_in_eccentricity(bridge, 2).value() == 2);
    REQUIRE(reticula::try_in_eccentricity(bridge, 1).value() == 3);
    REQUIRE(reticula::in_eccentricity(bridge, 2) == 2);
    REQUIRE(reticula::in_eccentricity(bridge, 1) == 3);
    for (auto v : std::vector<reticula::VertexType>{3, 4}) {
      REQUIRE_FALSE(reticula::try_in_eccentricity(bridge, v).has_value());
      REQUIRE_THROWS_AS(
        reticula::in_eccentricity(bridge, v),
        reticula::distance_undefined_error);
    }
  }

  SECTION("works for directed 6-cycle") {
    reticula::directed_network cyc6(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 1}});
    for (auto v : cyc6.vertices()) {
      REQUIRE(reticula::try_out_eccentricity(cyc6, v).value() == 5);
      REQUIRE(reticula::try_in_eccentricity(cyc6, v).value() == 5);
      REQUIRE(reticula::out_eccentricity(cyc6, v) == 5);
      REQUIRE(reticula::in_eccentricity(cyc6, v) == 5);
    }
  }

  SECTION("works for undirected networks") {
    reticula::undirected_network und_cyc(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 1}});
    for (auto v : und_cyc.vertices()) {
      REQUIRE(reticula::try_eccentricity(und_cyc, v).value() == 3);
      REQUIRE(reticula::eccentricity(und_cyc, v) == 3);
    }

    reticula::undirected_network und_path(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}});
    REQUIRE(reticula::try_eccentricity(und_path, 1).value() == 5);
    REQUIRE(reticula::try_eccentricity(und_path, 6).value() == 5);
    REQUIRE(reticula::try_eccentricity(und_path, 2).value() == 4);
    REQUIRE(reticula::try_eccentricity(und_path, 5).value() == 4);
    REQUIRE(reticula::try_eccentricity(und_path, 3).value() == 3);
    REQUIRE(reticula::try_eccentricity(und_path, 4).value() == 3);

    reticula::undirected_network und_star({{0, 1}, {0, 2}, {0, 3}, {0, 4}});
    REQUIRE(reticula::try_eccentricity(und_star, 0).value() == 1);
    for (auto v : std::vector<reticula::VertexType>{1, 2, 3, 4}) {
      REQUIRE(reticula::try_eccentricity(und_star, v).value() == 2);
    }
  }
}

TEST_CASE(
  "radius", "[reticula::try_out_radius][reticula::out_radius]"
            "[reticula::try_in_radius][reticula::in_radius]"
            "[reticula::try_radius][reticula::radius]") {

  SECTION("works for null graphs and graphs with one vertex") {
    // Null
    REQUIRE(
      reticula::try_out_radius(reticula::directed_network()) == std::nullopt);
    REQUIRE(
      reticula::try_in_radius(reticula::directed_network()) == std::nullopt);
    REQUIRE(
      reticula::try_radius(reticula::undirected_network()) == std::nullopt);
    REQUIRE_THROWS_AS(
      reticula::out_radius(reticula::directed_network()),
      reticula::distance_undefined_error);
    REQUIRE_THROWS_AS(
      reticula::in_radius(reticula::directed_network()),
      reticula::distance_undefined_error);
    REQUIRE_THROWS_AS(
      reticula::radius(reticula::undirected_network()),
      reticula::distance_undefined_error);

    REQUIRE(
      reticula::try_out_radius(reticula::directed_network({}, {0})).value() ==
      0);
    REQUIRE(
      reticula::try_in_radius(reticula::directed_network({}, {0})).value() ==
      0);
    REQUIRE(
      reticula::try_radius(reticula::undirected_network({}, {0})).value() == 0);
    REQUIRE(reticula::out_radius(reticula::directed_network({}, {0})) == 0);
    REQUIRE(reticula::in_radius(reticula::directed_network({}, {0})) == 0);
    REQUIRE(reticula::radius(reticula::undirected_network({}, {0})) == 0);
  }

  SECTION("returns nullopt when no suitable centre exists") {
    reticula::directed_network dir_disc(
      {{1, 2}, {2, 3}, {3, 5}, {5, 6}, {5, 4}}, {12});
    REQUIRE(reticula::try_out_radius(dir_disc) == std::nullopt);
    REQUIRE(reticula::try_in_radius(dir_disc) == std::nullopt);
    REQUIRE_THROWS_AS(
      reticula::out_radius(dir_disc), reticula::distance_undefined_error);
    REQUIRE_THROWS_AS(
      reticula::in_radius(dir_disc), reticula::distance_undefined_error);

    reticula::undirected_network und_disc({{1, 2}, {2, 3}, {3, 4}}, {99});
    REQUIRE(reticula::try_radius(und_disc) == std::nullopt);
    REQUIRE_THROWS_AS(
      reticula::radius(und_disc), reticula::distance_undefined_error);
  }

  SECTION("works for directed reversed path 6->5->4->3->2->1") {
    reticula::directed_network path_rev(
      {{6, 5}, {5, 4}, {4, 3}, {3, 2}, {2, 1}});
    REQUIRE(reticula::try_out_radius(path_rev).value() == 5);
    REQUIRE(reticula::out_radius(path_rev) == 5);
    REQUIRE(reticula::try_in_radius(path_rev).value() == 5);
    REQUIRE(reticula::in_radius(path_rev) == 5);
  }

  SECTION("works for directed SCCs with a one-way bridge (4<->3 -> 2<->1)") {
    reticula::directed_network bridge({{4, 3}, {3, 4}, {2, 1}, {1, 2}, {4, 2}});
    REQUIRE(reticula::try_out_radius(bridge).value() == 2);
    REQUIRE(reticula::out_radius(bridge) == 2);
    REQUIRE(reticula::try_in_radius(bridge).value() == 2);
    REQUIRE(reticula::in_radius(bridge) == 2);
  }

  SECTION("works for directed stars") {
    reticula::directed_network out_star({{0, 1}, {0, 2}, {0, 3}, {0, 4}});
    REQUIRE(reticula::try_out_radius(out_star).value() == 1);
    REQUIRE(reticula::out_radius(out_star) == 1);
    REQUIRE(reticula::try_in_radius(out_star) == std::nullopt);
    REQUIRE_THROWS_AS(
      reticula::in_radius(out_star), reticula::distance_undefined_error);

    reticula::directed_network in_star({{1, 0}, {2, 0}, {3, 0}, {4, 0}});
    REQUIRE(reticula::try_in_radius(in_star).value() == 1);
    REQUIRE(reticula::in_radius(in_star) == 1);
    REQUIRE(reticula::try_out_radius(in_star) == std::nullopt);
    REQUIRE_THROWS_AS(
      reticula::out_radius(in_star), reticula::distance_undefined_error);
  }

  SECTION("works for directed 6-cycle") {
    reticula::directed_network cyc6(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 1}});
    REQUIRE(reticula::try_out_radius(cyc6).value() == 5);
    REQUIRE(reticula::try_in_radius(cyc6).value() == 5);
    REQUIRE(reticula::out_radius(cyc6) == 5);
    REQUIRE(reticula::in_radius(cyc6) == 5);
  }

  SECTION("works for undirected networks") {
    reticula::undirected_network und_cyc(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 1}});
    REQUIRE(reticula::try_radius(und_cyc).value() == 3);
    REQUIRE(reticula::radius(und_cyc) == 3);

    reticula::undirected_network und_path(
      {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}});
    REQUIRE(reticula::try_radius(und_path).value() == 3);
    REQUIRE(reticula::radius(und_path) == 3);

    reticula::undirected_network und_star({{0, 1}, {0, 2}, {0, 3}, {0, 4}});
    REQUIRE(reticula::try_radius(und_star).value() == 1);
    REQUIRE(reticula::radius(und_star) == 1);
  }
}
