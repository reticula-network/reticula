#include <catch2/catch_test_macros.hpp>

#include <reticula/concepts.hpp>
#include <reticula/generation/barabasi_albert.hpp>

TEST_CASE(
  "Barabasi-Albert random graph", "[reticula::random_barabasi_albert_graph]") {
  std::mt19937_64 gen(42);
  reticula::VertexType n = 1000, m = 3;
  reticula::undirected_network r =
    reticula::random_barabasi_albert_graph(n, m, gen);
  REQUIRE(r.vertices().size() == static_cast<std::size_t>(n));
  REQUIRE(r.edges().size() == (n - m) * m);
}
