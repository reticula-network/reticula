#include "reticula/concepts.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include <reticula/networks.hpp>
#include <reticula/operations/relabel.hpp>

using Catch::Matchers::RangeEquals;
using Catch::Matchers::UnorderedRangeEquals;

TEST_CASE("relabel nodes", "[reticula::relabel_nodes]") {
  reticula::undirected_network graph({{1, 2}, {1, 3}});
  auto relabeled = reticula::relabel(graph);

  REQUIRE_THAT(
    relabeled.vertices(),
    UnorderedRangeEquals(std::vector<reticula::VertexType>({0, 1, 2})));
  REQUIRE_THAT(
    relabeled.edges(),
    UnorderedRangeEquals(
      std::vector<reticula::undirected_edge>({{0, 1}, {0, 2}})));
}

TEST_CASE("relabel_nodes with function mapping", "[reticula::relabel_nodes]") {
  SECTION("simple directed network with lambda") {
    reticula::directed_network net({{1, 2}, {2, 3}, {3, 1}});

    auto relabeled =
      reticula::relabel(net, [](reticula::VertexType v) { return v * 10; });

    REQUIRE_THAT(
      relabeled.vertices(),
      UnorderedRangeEquals(std::vector<std::size_t>({10, 20, 30})));
    REQUIRE_THAT(
      relabeled.edges(),
      UnorderedRangeEquals(
        std::vector<reticula::directed_edge>({{10, 20}, {20, 30}, {30, 10}})));
  }

  SECTION("undirected network with function object") {
    struct Doubler {
      auto operator()(reticula::VertexType x) const -> reticula::VertexType {
        return x * 2;
      }
    };

    reticula::undirected_network net({{1, 2}, {2, 3}});
    auto relabeled = reticula::relabel(net, Doubler{});

    REQUIRE_THAT(
      relabeled.vertices(),
      UnorderedRangeEquals(std::vector<reticula::VertexType>({2, 4, 6})));
    REQUIRE_THAT(
      relabeled.edges(),
      UnorderedRangeEquals(
        std::vector<reticula::undirected_edge>({{2, 4}, {4, 6}})));
  }
}

TEST_CASE("relabel_nodes with container mapping", "[reticula::relabel_nodes]") {
  reticula::directed_network net({{1, 2}, {2, 3}, {3, 1}}, {12});
  SECTION("std::map mapping") {
    std::map<reticula::VertexType, reticula::VertexType> mapping = {
      {1, 10}, {2, 20}, {3, 30}, {12, 120}};

    auto relabeled = reticula::relabel(net, mapping);
    REQUIRE_THAT(
      relabeled.vertices(),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({10, 20, 30, 120})));
    REQUIRE_THAT(
      relabeled.edges(),
      UnorderedRangeEquals(
        std::vector<reticula::directed_edge>({{10, 20}, {20, 30}, {30, 10}})));
  }

  SECTION("std::unordered_map mapping") {
    std::map<reticula::VertexType, reticula::VertexType> mapping = {
      {1, 10}, {2, 20}, {3, 30}, {12, 120}};

    auto relabeled = reticula::relabel(net, mapping);

    REQUIRE_THAT(
      relabeled.vertices(),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({10, 20, 30, 120})));
    REQUIRE_THAT(
      relabeled.edges(),
      UnorderedRangeEquals(
        std::vector<reticula::directed_edge>({{10, 20}, {20, 30}, {30, 10}})));
  }

  SECTION("missing vertices map to themselves") {
    std::map<reticula::VertexType, reticula::VertexType> incomplete_mapping = {
      {1, 10}, {2, 20}, {12, 120}};

    REQUIRE_THAT(
      reticula::relabel(net, incomplete_mapping).vertices(),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({10, 20, 3, 120})));
    REQUIRE_THAT(
      reticula::relabel(net, incomplete_mapping).edges(),
      UnorderedRangeEquals(
        std::vector<reticula::directed_edge>({{10, 20}, {20, 3}, {3, 10}})));
  }

  SECTION("extra mappings are ignored") {
    std::map<reticula::VertexType, reticula::VertexType> mapping = {
      {1, 10}, {2, 20}, {3, 30}, {12, 120}, {99, 999}};
    auto relabeled = reticula::relabel(net, mapping);
    REQUIRE_THAT(
      relabeled.vertices(),
      UnorderedRangeEquals(
        std::vector<reticula::VertexType>({10, 20, 30, 120})));
    REQUIRE_THAT(
      relabeled.edges(),
      UnorderedRangeEquals(
        std::vector<reticula::directed_edge>({{10, 20}, {20, 30}, {30, 10}})));
  }
}
