#include <catch2/catch_test_macros.hpp>

#include <reticula/network_concepts.hpp>
#include <reticula/static_edges.hpp>
#include <reticula/networks.hpp>

TEST_CASE("edge and network traits", "[reticula::traits]") {
  STATIC_REQUIRE(reticula::is_undirected_v<
      reticula::undirected_edge<int>>);
  STATIC_REQUIRE_FALSE(reticula::is_directed_v<
      reticula::undirected_edge<int>>);
  STATIC_REQUIRE(reticula::is_directed_v<
      reticula::directed_edge<int>>);
  STATIC_REQUIRE(reticula::is_dyadic_v<
      reticula::directed_edge<int>>);
  STATIC_REQUIRE_FALSE(reticula::is_dyadic_v<
      reticula::undirected_hyperedge<int>>);

  STATIC_REQUIRE(reticula::is_undirected_v<
      reticula::undirected_network<int>>);
  STATIC_REQUIRE_FALSE(reticula::is_directed_v<
      reticula::undirected_network<int>>);
  STATIC_REQUIRE(reticula::is_directed_v<
      reticula::directed_network<int>>);
  STATIC_REQUIRE_FALSE(reticula::is_dyadic_v<
      reticula::undirected_hypernetwork<int>>);
}
