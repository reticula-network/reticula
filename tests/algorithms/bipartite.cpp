#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/algorithms/bipartite.hpp>

TEST_CASE(
  "two-colouring and bipartite tests",
  "[reticula::try_two_colouring][reticula::is_bipartite]") {
  SECTION("bipartite graph is coloured") {
    reticula::undirected_network g({{1, 2}, {2, 3}, {3, 4}, {4, 1}});
    auto colouring = reticula::try_two_colouring(g);
    REQUIRE(colouring.has_value());
    auto [c11, c12] = *colouring;
    REQUIRE(c11.size() + c12.size() == g.vertices().size());

    std::unordered_set<int> vs(c11.begin(), c11.end());
    vs.insert(c12.begin(), c12.end());
    REQUIRE_THAT(vs, UnorderedRangeEquals(g.vertices()));

    for (const auto& e : g.edges()) {
      auto vs = e.incident_verts();
      if (vs.size() == 2)
        REQUIRE((c11.contains(vs[0]) != c11.contains(vs[1])));
    }

    auto [c21, c22] = reticula::two_colouring(g);
    REQUIRE(c21.size() + c22.size() == g.vertices().size());

    std::unordered_set<int> vs2(c21.begin(), c21.end());
    vs2.insert(c22.begin(), c22.end());
    REQUIRE_THAT(vs2, UnorderedRangeEquals(g.vertices()));

    for (const auto& e : g.edges()) {
      auto vs2 = e.incident_verts();
      if (vs2.size() == 2)
        REQUIRE((c21.contains(vs2[0]) != c21.contains(vs2[1])));
    }
    REQUIRE(reticula::is_bipartite(g));
  }

  SECTION("non-bipartite graph is rejected") {
    reticula::undirected_network g({{1, 2}, {2, 3}, {3, 1}});
    REQUIRE_FALSE(reticula::try_two_colouring(g).has_value());
    REQUIRE_FALSE(reticula::is_bipartite(g));

    REQUIRE_THROWS_AS(
      reticula::two_colouring(g), reticula::not_bipartite_error);
  }
}
