#include <random>
#include <vector>
#include <utility>

#include <catch2/catch_test_macros.hpp>

#include <reticula/algorithms/graphicallity.hpp>


TEST_CASE("is graphic", "[reticula::is_graphic]") {
  REQUIRE(reticula::is_graphic(std::vector<int>({})));
  REQUIRE(reticula::is_graphic(std::vector<int>({5, 3, 3, 3, 2, 2})));
  REQUIRE(reticula::is_graphic(std::vector<int>({3, 3, 2, 1, 1, 0})));
  REQUIRE(reticula::is_graphic(std::vector<int>({5, 1, 1, 1, 1, 1})));
  REQUIRE(reticula::is_graphic(std::vector<int>(5, 2)));
  REQUIRE(reticula::is_graphic(std::vector<int>(5, 4)));

  REQUIRE_FALSE(reticula::is_graphic(std::vector<int>({-1, 3, 3, 3, 2, 2})));
  REQUIRE_FALSE(reticula::is_graphic(std::vector<int>({4, 3, 3, 2, 2, 1})));
  REQUIRE_FALSE(reticula::is_graphic(std::vector<int>({4, 3, 2, 1})));
}

TEST_CASE("is digraphic", "[reticula::is_digraphic]") {
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>({})));
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, 0}, {2, 1}, {0, 1}, {1, 1}, {0, 1}})));
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{4, 0}, {0, 1}, {0, 1}, {0, 1}, {0, 1}})));
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{2, 0}, {2, 1}, {0, 1}, {0, 1}, {0, 1}})));
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{0, 1}, {1, 0}})));
  REQUIRE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, 1}, {1, 1}})));

  REQUIRE_FALSE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, -1}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{-1, 0}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{1, 0}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{4, 3}, {2, 1}, {1, 1}, {1, 1}, {0, 1}})));
  REQUIRE_FALSE(reticula::is_digraphic(std::vector<std::pair<int, int>>(
          {{4, 0}, {0, 1}, {1, 1}, {3, 1}, {0, 1}})));
}
