#include <filesystem>

#include <catch2/catch.hpp>

#include <dag/io.hpp>
using Catch::Matchers::UnorderedEquals;

std::filesystem::path test_dir() {
  return std::filesystem::path(__FILE__).parent_path()/"io_test_files";
}

TEST_CASE("read undirected edgelist", "[dag::read_edgelist]") {
  SECTION("reads utf-8 encoded files correctly") {
    auto g = dag::read_edgelist<dag::undirected_edge<std::string>>(
        test_dir()/"utf-8.csv", ',');
    REQUIRE_THAT(g.vertices(), UnorderedEquals(
          std::vector<std::string>
            {"سلام", "دنیا", "hello", "world"}));
    REQUIRE_THAT(g.edges(), UnorderedEquals(
          std::vector<dag::undirected_edge<std::string>>
            {{"سلام", "دنیا"}, {"hello", "world"}}));
  }

  SECTION("reads utf-8 and windows line-ending files correctly") {
    auto g = dag::read_edgelist<dag::undirected_edge<std::string>>(
        test_dir()/"utf-8+windows-le.csv", ',');
    REQUIRE_THAT(g.vertices(), UnorderedEquals(
          std::vector<std::string>
            {"سلام", "دنیا", "hello", "world"}));
    REQUIRE_THAT(g.edges(), UnorderedEquals(
          std::vector<dag::undirected_edge<std::string>>
            {{"سلام", "دنیا"}, {"hello", "world"}}));
  }
}

TEST_CASE("read directed edgelist", "[dag::read_edgelist]") {
  SECTION("reads utf-8 encoded files correctly") {
    auto g = dag::read_edgelist<dag::directed_edge<std::string>>(
        test_dir()/"utf-8.csv", ',');
    REQUIRE_THAT(g.vertices(), UnorderedEquals(
          std::vector<std::string>
            {"سلام", "دنیا", "hello", "world"}));
    REQUIRE_THAT(g.edges(), UnorderedEquals(
          std::vector<dag::directed_edge<std::string>>
            {{"سلام", "دنیا"}, {"hello", "world"}}));
  }

  SECTION("reads utf-8 and windows line-ending files correctly") {
    auto g = dag::read_edgelist<dag::directed_edge<std::string>>(
        test_dir()/"utf-8+windows-le.csv", ',');
    REQUIRE_THAT(g.vertices(), UnorderedEquals(
          std::vector<std::string>
            {"سلام", "دنیا", "hello", "world"}));
    REQUIRE_THAT(g.edges(), UnorderedEquals(
          std::vector<dag::directed_edge<std::string>>
            {{"سلام", "دنیا"}, {"hello", "world"}}));
  }
}
