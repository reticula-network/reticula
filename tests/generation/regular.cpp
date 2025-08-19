#include <catch2/catch_test_macros.hpp>

#include <reticula/concepts.hpp>
#include <reticula/generation/regular.hpp>
#include <reticula/networks.hpp>

TEST_CASE("random k-regular graph", "[reticula::random_regular_graph]") {
  std::mt19937_64 gen(42);
  reticula::VertexType n = 1000, k = 4;
  reticula::undirected_network r = reticula::random_regular_graph(n, k, gen);
  REQUIRE(r.vertices().size() == n);
  REQUIRE(r.edges().size() == k * n / 2);

  REQUIRE_THROWS_AS(
    reticula::random_regular_graph(321, 13, gen), std::domain_error);

  auto verts = r.vertices();
  REQUIRE(
    std::ranges::all_of(verts, [&r, k](auto v) { return r.degree(v) == k; }));
}

TEST_CASE(
  "random directed k-regular graph",
  "[reticula::random_directed_regular_graph]") {
  std::mt19937_64 gen(42);
  reticula::VertexType n = 1000, k = 4;
  reticula::directed_network r =
    reticula::random_directed_regular_graph(n, k, gen);
  REQUIRE(r.vertices().size() == n);
  REQUIRE(r.edges().size() == k * n);

  auto verts = r.vertices();
  REQUIRE(std::ranges::all_of(verts, [&r, k](auto v) {
    return r.out_degree(v) == k;
    return r.in_degree(v) == k;
  }));
}
