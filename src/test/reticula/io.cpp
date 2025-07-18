#include <filesystem>
#include <cstdio>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::UnorderedRangeEquals;

#include <reticula/io.hpp>
#include <reticula/generators.hpp>

std::filesystem::path test_dir() {
  return std::filesystem::path(__FILE__).parent_path()/"io_test_files";
}

TEST_CASE("read undirected edgelist", "[reticula::read_edgelist]") {
  SECTION("reads utf-8 encoded files correctly") {
    auto g = reticula::read_edgelist<reticula::undirected_edge<std::string>>(
        test_dir()/"utf-8.csv");
    REQUIRE_THAT(g.vertices(), UnorderedRangeEquals(
          std::vector<std::string>
            {"سلام", "دنیا", "hello", "world"}));
    REQUIRE_THAT(g.edges(), UnorderedRangeEquals(
          std::vector<reticula::undirected_edge<std::string>>
            {{"سلام", "دنیا"}, {"hello", "world"}}));
  }

  SECTION("reads utf-8 and windows line-ending files correctly") {
    auto g = reticula::read_edgelist<reticula::undirected_edge<std::string>>(
        test_dir()/"utf-8+windows-le.csv");
    REQUIRE_THAT(g.vertices(), UnorderedRangeEquals(
          std::vector<std::string>
            {"سلام", "دنیا", "hello", "world"}));
    REQUIRE_THAT(g.edges(), UnorderedRangeEquals(
          std::vector<reticula::undirected_edge<std::string>>
            {{"سلام", "دنیا"}, {"hello", "world"}}));
  }
}

TEST_CASE("read directed edgelist", "[reticula::read_edgelist]") {
  SECTION("reads utf-8 encoded files correctly") {
    auto g = reticula::read_edgelist<reticula::directed_edge<std::string>>(
        test_dir()/"utf-8.csv");
    REQUIRE_THAT(g.vertices(), UnorderedRangeEquals(
          std::vector<std::string>
            {"سلام", "دنیا", "hello", "world"}));
    REQUIRE_THAT(g.edges(), UnorderedRangeEquals(
          std::vector<reticula::directed_edge<std::string>>
            {{"سلام", "دنیا"}, {"hello", "world"}}));
  }

  SECTION("reads utf-8 and windows line-ending files correctly") {
    auto g = reticula::read_edgelist<reticula::directed_edge<std::string>>(
        test_dir()/"utf-8+windows-le.csv");
    REQUIRE_THAT(g.vertices(), UnorderedRangeEquals(
          std::vector<std::string>
            {"سلام", "دنیا", "hello", "world"}));
    REQUIRE_THAT(g.edges(), UnorderedRangeEquals(
          std::vector<reticula::directed_edge<std::string>>
            {{"سلام", "دنیا"}, {"hello", "world"}}));
  }
}

TEST_CASE("write/read edgelists",
    "[reticula::write_edgelist][reticula::read_edgelist]") {
  auto g = reticula::complete_directed_graph(std::size_t{100});
  auto dir = std::filesystem::temp_directory_path();
  auto fn = dir / "test.edges";
  reticula::write_edgelist(g, fn);
  auto g2 = reticula::read_edgelist<
    reticula::directed_edge<std::size_t>>(fn);

  REQUIRE_THAT(g.edges(), UnorderedRangeEquals(g2.edges()));
}
