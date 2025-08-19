#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/concepts.hpp>
#include <reticula/generation/watts_strogatz.hpp>
#include <reticula/generation/deterministic.hpp>

TEST_CASE("Watts-Strogatz random graph",
    "[reticula::random_watts_strogatz_graph]") {
  std::mt19937_64 gen(42);
  reticula::VertexType n = 1000, k = 4;

  SECTION("no rewiring") {
    auto ws = reticula::random_watts_strogatz_graph(n, k, 0.0, gen);
    auto ring = reticula::regular_ring_lattice(n, k);

    REQUIRE(ws.edges().size() == n * k / 2);
    REQUIRE_THAT(ws.edges(), UnorderedRangeEquals(ring.edges()));
  }

  SECTION("degree preserved") {
    double p = 0.5;
    auto ws = reticula::random_watts_strogatz_graph(n, k, p, gen);

    REQUIRE(ws.vertices().size() == n);
    REQUIRE(ws.edges().size() == n * k / 2);
    REQUIRE(std::ranges::none_of(ws.edges(), [](const auto& e) {
      return e.incident_verts().size() == 1;
    }));
  }
}
