#include <algorithm>
#include <filesystem>
#include <cstdio>
#include <unordered_set>
#include <unordered_map>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
using Catch::Matchers::UnorderedRangeEquals;
using Catch::Matchers::Contains;

#include <reticula/io.hpp>
#include <reticula/generators.hpp>
#include <nlohmann/json.hpp>

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

TEST_CASE("read undirected HIF", "[reticula::read_hif]") {
  SECTION("reads simple undirected hypergraph") {
    std::stringstream hif_data(R"({
      "network-type": "undirected",
      "incidences": [
        {"node": 1, "edge": 0},
        {"node": 2, "edge": 0},
        {"node": 3, "edge": 0},
        {"node": 2, "edge": 1},
        {"node": 4, "edge": 1}
      ]
    })");

    auto net =
      reticula::read_hif<reticula::undirected_hyperedge<int>>(hif_data);

    REQUIRE(net.edges().size() == 2);
    REQUIRE_THAT(
      net.vertices(), UnorderedRangeEquals(std::vector<int>{1, 2, 3, 4}));

    auto edges_vec = std::vector<reticula::undirected_hyperedge<int>>(
      net.edges().begin(), net.edges().end());
    std::ranges::sort(edges_vec, [](const auto& a, const auto& b) {
      return a.incident_verts().size() > b.incident_verts().size();
    });

    REQUIRE_THAT(
      edges_vec[0].incident_verts(),
      UnorderedRangeEquals(std::vector<int>{1, 2, 3}));
    REQUIRE_THAT(
      edges_vec[1].incident_verts(),
      UnorderedRangeEquals(std::vector<int>{2, 4}));
  }

  SECTION("reads HIF with string vertices") {
    std::stringstream hif_data(R"({
      "network-type": "undirected",
      "incidences": [
        {"node": "alice", "edge": 0},
        {"node": "bob", "edge": 0},
        {"node": "charlie", "edge": 0}
      ]
    })");

    auto net =
      reticula::read_hif<reticula::undirected_hyperedge<std::string>>(hif_data);

    REQUIRE(net.edges().size() == 1);
    REQUIRE_THAT(
      net.vertices(),
      UnorderedRangeEquals(
        std::vector<std::string>{"alice", "bob", "charlie"}));
    REQUIRE_THAT(
      net.edges()[0].incident_verts(),
      UnorderedRangeEquals(
        std::vector<std::string>{"alice", "bob", "charlie"}));
  }

  SECTION("handles missing network-type") {
    std::stringstream hif_data(R"({
      "incidences": [
        {"node": 1, "edge": 0},
        {"node": 2, "edge": 0}
      ]
    })");

    auto net =
      reticula::read_hif<reticula::undirected_hyperedge<int>>(hif_data);
    REQUIRE(net.edges().size() == 1);
  }

  SECTION("throws on wrong network-type") {
    std::stringstream hif_data(R"({
      "network-type": "directed",
      "incidences": [
        {"node": 1, "edge": 0},
        {"node": 2, "edge": 0}
      ]
    })");

    REQUIRE_THROWS_AS(
      reticula::read_hif<reticula::undirected_hyperedge<int>>(hif_data),
      std::runtime_error);
  }

  SECTION("throws on missing incidences") {
    std::stringstream hif_data(R"({
      "network-type": "undirected"
    })");

    REQUIRE_THROWS_AS(
      reticula::read_hif<reticula::undirected_hyperedge<int>>(hif_data),
      std::runtime_error);
  }

  SECTION("reads isolated nodes from nodes field") {
    std::stringstream hif_data(R"({
      "network-type": "undirected",
      "incidences": [
        {"node": 1, "edge": 0},
        {"node": 2, "edge": 0}
      ],
      "nodes": [
        {"node": 3},
        {"node": 4}
      ]
    })");

    auto net =
      reticula::read_hif<reticula::undirected_hyperedge<int>>(hif_data);

    REQUIRE(net.edges().size() == 1);
    REQUIRE_THAT(
      net.vertices(), UnorderedRangeEquals(std::vector<int>{1, 2, 3, 4}));
    REQUIRE_THAT(
      net.edges()[0].incident_verts(),
      UnorderedRangeEquals(std::vector<int>{1, 2}));
  }
}

TEST_CASE("read directed HIF", "[reticula::read_hif]") {
  SECTION("reads simple directed hypergraph") {
    std::stringstream hif_data(R"({
      "network-type": "directed",
      "incidences": [
        {"node": 1, "edge": 0, "direction": "tail"},
        {"node": 2, "edge": 0, "direction": "tail"},
        {"node": 3, "edge": 0, "direction": "head"},
        {"node": 2, "edge": 1, "direction": "tail"},
        {"node": 4, "edge": 1, "direction": "head"}
      ]
    })");

    auto net = reticula::read_hif<reticula::directed_hyperedge<int>>(hif_data);

    REQUIRE(net.edges().size() == 2);
    REQUIRE_THAT(
      net.vertices(), UnorderedRangeEquals(std::vector<int>{1, 2, 3, 4}));

    auto edges_vec = std::vector<reticula::directed_hyperedge<int>>(
      net.edges().begin(), net.edges().end());
    std::ranges::sort(edges_vec, [](const auto& a, const auto& b) {
      return a.tails().size() > b.tails().size();
    });

    REQUIRE_THAT(
      edges_vec[0].tails(), UnorderedRangeEquals(std::vector<int>{1, 2}));
    REQUIRE_THAT(
      edges_vec[0].heads(), UnorderedRangeEquals(std::vector<int>{3}));
    REQUIRE_THAT(
      edges_vec[1].tails(), UnorderedRangeEquals(std::vector<int>{2}));
    REQUIRE_THAT(
      edges_vec[1].heads(), UnorderedRangeEquals(std::vector<int>{4}));
  }

  SECTION("throws on missing direction") {
    std::stringstream hif_data(R"({
      "network-type": "directed",
      "incidences": [
        {"node": 1, "edge": 0},
        {"node": 2, "edge": 0}
      ]
    })");

    REQUIRE_THROWS_AS(
      reticula::read_hif<reticula::directed_hyperedge<int>>(hif_data),
      std::runtime_error);
  }

  SECTION("throws on invalid direction") {
    std::stringstream hif_data(R"({
      "network-type": "directed",
      "incidences": [
        {"node": 1, "edge": 0, "direction": "invalid"},
        {"node": 2, "edge": 0, "direction": "head"}
      ]
    })");

    REQUIRE_THROWS_AS(
      reticula::read_hif<reticula::directed_hyperedge<int>>(hif_data),
      std::runtime_error);
  }

  SECTION("throws on wrong network-type") {
    std::stringstream hif_data(R"({
      "network-type": "undirected",
      "incidences": [
        {"node": 1, "edge": 0, "direction": "tail"},
        {"node": 2, "edge": 0, "direction": "head"}
      ]
    })");

    REQUIRE_THROWS_AS(
      reticula::read_hif<reticula::directed_hyperedge<int>>(hif_data),
      std::runtime_error);
  }

  SECTION("reads isolated nodes from nodes field") {
    std::stringstream hif_data(R"({
      "network-type": "directed",
      "incidences": [
        {"node": 1, "edge": 0, "direction": "tail"},
        {"node": 2, "edge": 0, "direction": "head"}
      ],
      "nodes": [
        {"node": 3},
        {"node": 4}
      ]
    })");

    auto net = reticula::read_hif<reticula::directed_hyperedge<int>>(hif_data);

    REQUIRE(net.edges().size() == 1);
    REQUIRE_THAT(
      net.vertices(), UnorderedRangeEquals(std::vector<int>{1, 2, 3, 4}));
    REQUIRE_THAT(
      net.edges()[0].tails(), UnorderedRangeEquals(std::vector<int>{1}));
    REQUIRE_THAT(
      net.edges()[0].heads(), UnorderedRangeEquals(std::vector<int>{2}));
  }
}

TEST_CASE("write undirected HIF", "[reticula::write_hif]") {
  SECTION("writes simple undirected hypergraph") {
    std::vector<reticula::undirected_hyperedge<int>> edges = {
      {{1, 2, 3}}, {{2, 4}}};
    reticula::network<reticula::undirected_hyperedge<int>> net(edges);

    std::stringstream output;
    reticula::write_hif(net, output);

    nlohmann::json result = nlohmann::json::parse(output.str());

    REQUIRE(result["network-type"] == "undirected");
    REQUIRE(result["incidences"].is_array());
    REQUIRE(result["incidences"].size() == 5);

    std::unordered_map<int, std::unordered_set<int>> edge_to_nodes;
    for (const auto& inc : result["incidences"]) {
      int edge_id = inc["edge"];
      int node = inc["node"];
      edge_to_nodes[edge_id].insert(node);
    }

    REQUIRE(edge_to_nodes.size() == 2);
    REQUIRE_THAT(
      std::vector<int>(edge_to_nodes[0].begin(), edge_to_nodes[0].end()),
      UnorderedRangeEquals(std::vector<int>{1, 2, 3}));
    REQUIRE_THAT(
      std::vector<int>(edge_to_nodes[1].begin(), edge_to_nodes[1].end()),
      UnorderedRangeEquals(std::vector<int>{2, 4}));
  }

  SECTION("writes isolated nodes to nodes field") {
    std::vector<reticula::undirected_hyperedge<int>> edges = {{{1, 2}}};
    std::vector<int> isolated_nodes = {3, 4};
    reticula::network<reticula::undirected_hyperedge<int>> net(
      edges, isolated_nodes);

    std::stringstream output;
    reticula::write_hif(net, output);

    nlohmann::json result = nlohmann::json::parse(output.str());

    REQUIRE(result["network-type"] == "undirected");
    REQUIRE(result["incidences"].is_array());
    REQUIRE(result["incidences"].size() == 2);
    REQUIRE(result["nodes"].is_array());

    std::vector<int> nodes;
    std::ranges::transform(
      result["nodes"], std::back_inserter(nodes),
      [](const auto& n) { return n.at("node").template get<int>(); });
    for (auto& i : isolated_nodes)
      REQUIRE_THAT(nodes, Contains(i));
  }
}

TEST_CASE("write directed HIF", "[reticula::write_hif]") {
  SECTION("writes simple directed hypergraph") {
    std::vector<reticula::directed_hyperedge<int>> edges = {
      {{1, 2}, {3}}, {{2}, {4}}};
    reticula::network<reticula::directed_hyperedge<int>> net(edges);

    std::stringstream output;
    reticula::write_hif(net, output);

    nlohmann::json result = nlohmann::json::parse(output.str());

    REQUIRE(result["network-type"] == "directed");
    REQUIRE(result["incidences"].is_array());
    REQUIRE(result["incidences"].size() == 5);

    std::unordered_map<int, std::unordered_set<int>> edge_to_tails;
    std::unordered_map<int, std::unordered_set<int>> edge_to_heads;

    for (const auto& inc : result["incidences"]) {
      int edge_id = inc["edge"];
      int node = inc["node"];
      std::string direction = inc["direction"];

      if (direction == "tail") {
        edge_to_tails[edge_id].insert(node);
      } else if (direction == "head") {
        edge_to_heads[edge_id].insert(node);
      }
    }

    REQUIRE(edge_to_tails.size() == 2);
    REQUIRE(edge_to_heads.size() == 2);

    REQUIRE_THAT(
      std::vector<int>(edge_to_tails[0].begin(), edge_to_tails[0].end()),
      UnorderedRangeEquals(std::vector<int>{1, 2}));
    REQUIRE_THAT(
      std::vector<int>(edge_to_heads[0].begin(), edge_to_heads[0].end()),
      UnorderedRangeEquals(std::vector<int>{3}));
    REQUIRE_THAT(
      std::vector<int>(edge_to_tails[1].begin(), edge_to_tails[1].end()),
      UnorderedRangeEquals(std::vector<int>{2}));
    REQUIRE_THAT(
      std::vector<int>(edge_to_heads[1].begin(), edge_to_heads[1].end()),
      UnorderedRangeEquals(std::vector<int>{4}));
  }

  SECTION("writes isolated nodes to nodes field") {
    std::vector<reticula::directed_hyperedge<int>> edges = {{{1}, {2}}};
    std::vector<int> isolated_nodes = {3, 4};
    reticula::network<reticula::directed_hyperedge<int>> net(
      edges, isolated_nodes);

    std::stringstream output;
    reticula::write_hif(net, output);

    nlohmann::json result = nlohmann::json::parse(output.str());

    REQUIRE(result["network-type"] == "directed");
    REQUIRE(result["incidences"].is_array());
    REQUIRE(result["incidences"].size() == 2);
    REQUIRE(result["nodes"].is_array());

    std::vector<int> nodes;
    std::ranges::transform(
      result["nodes"], std::back_inserter(nodes),
      [](const auto& n) { return n.at("node").template get<int>(); });
    for (auto& i : isolated_nodes)
      REQUIRE_THAT(nodes, Contains(i));
  }
}

TEST_CASE("HIF roundtrip", "[reticula::read_hif][reticula::write_hif]") {
  SECTION("undirected hypergraph roundtrip") {
    std::vector<reticula::undirected_hyperedge<int>> original_edges = {
      {{1, 2, 3}}, {{2, 4, 5}}, {6}};
    reticula::network<reticula::undirected_hyperedge<int>> original_net(
      original_edges);

    std::stringstream buffer;
    reticula::write_hif(original_net, buffer);

    auto reconstructed_net =
      reticula::read_hif<reticula::undirected_hyperedge<int>>(buffer);

    REQUIRE_THAT(
      original_net.vertices(),
      UnorderedRangeEquals(reconstructed_net.vertices()));
    REQUIRE(original_net.edges().size() == reconstructed_net.edges().size());

    auto orig_edges = std::vector<reticula::undirected_hyperedge<int>>(
      original_net.edges().begin(), original_net.edges().end());
    auto recon_edges = std::vector<reticula::undirected_hyperedge<int>>(
      reconstructed_net.edges().begin(), reconstructed_net.edges().end());
    std::ranges::sort(orig_edges);
    std::ranges::sort(recon_edges);

    REQUIRE_THAT(orig_edges, UnorderedRangeEquals(recon_edges));
  }

  SECTION("directed hypergraph roundtrip") {
    std::vector<reticula::directed_hyperedge<int>> original_edges = {
      {{1, 2}, {3, 4}}, {{5}, {6}}, {{7, 8}, {}}};
    reticula::network<reticula::directed_hyperedge<int>> original_net(
      original_edges);

    std::stringstream buffer;
    reticula::write_hif(original_net, buffer);

    auto reconstructed_net =
      reticula::read_hif<reticula::directed_hyperedge<int>>(buffer);

    REQUIRE_THAT(
      original_net.vertices(),
      UnorderedRangeEquals(reconstructed_net.vertices()));
    REQUIRE(original_net.edges().size() == reconstructed_net.edges().size());

    auto orig_edges = std::vector<reticula::directed_hyperedge<int>>(
      original_net.edges().begin(), original_net.edges().end());
    auto recon_edges = std::vector<reticula::directed_hyperedge<int>>(
      reconstructed_net.edges().begin(), reconstructed_net.edges().end());
    std::ranges::sort(orig_edges);
    std::ranges::sort(recon_edges);

    REQUIRE_THAT(orig_edges, UnorderedRangeEquals(recon_edges));
  }

  SECTION("roundtrip with isolated nodes") {
    std::vector<reticula::undirected_hyperedge<int>> original_edges = {
      {{1, 2, 3}}};
    std::vector<int> isolated_nodes = {4, 5, 6};
    reticula::network<reticula::undirected_hyperedge<int>> original_net(
      original_edges, isolated_nodes);

    std::stringstream buffer;
    reticula::write_hif(original_net, buffer);

    auto reconstructed_net =
      reticula::read_hif<reticula::undirected_hyperedge<int>>(buffer);

    REQUIRE_THAT(
      original_net.vertices(),
      UnorderedRangeEquals(reconstructed_net.vertices()));
    REQUIRE(original_net.edges().size() == reconstructed_net.edges().size());
    REQUIRE_THAT(
      original_net.edges(), UnorderedRangeEquals(reconstructed_net.edges()));
  }
}

TEST_CASE("HIF file I/O", "[reticula::read_hif][reticula::write_hif]") {
  SECTION("file-based I/O") {
    std::vector<reticula::undirected_hyperedge<std::string>> edges = {
      {{"alice", "باب", "charlie"}}, {{"باب", "david"}}};
    reticula::network<reticula::undirected_hyperedge<std::string>> net(edges);

    auto temp_dir = std::filesystem::temp_directory_path();
    auto temp_file = temp_dir / "test_hypergraph.hif";

    reticula::write_hif(net, temp_file);
    auto loaded_net =
      reticula::read_hif<reticula::undirected_hyperedge<std::string>>(
        temp_file);

    REQUIRE_THAT(net.vertices(), UnorderedRangeEquals(loaded_net.vertices()));
    REQUIRE_THAT(net.edges(), UnorderedRangeEquals(loaded_net.edges()));

    std::filesystem::remove(temp_file);
  }
}

TEST_CASE("HIF standard comprehensive test", "[reticula::read_hif]") {
  auto compliant_dir = test_dir() / "HIF-compliant";
  auto non_compliant_dir = test_dir() / "HIF-non-compliant";

  SECTION("undirected compliant files should load without throwing") {
    std::vector<std::string> compliant_int_files = {
      "empty_arrays.json",
      "empty_hypergraph.json",
      "single_edge.json",
      "single_edge_with_attrs.json",
      "single_incidence.json",
      "single_incidence_with_attrs.json",
      "single_incidence_with_weights.json",
      "single_node.json",
      "single_node_with_attrs.json"
    };

    for (const auto& filename : compliant_int_files) {
      DYNAMIC_SECTION(filename) {
        REQUIRE_NOTHROW(
          reticula::read_hif<reticula::undirected_hyperedge<int>>(
            compliant_dir / filename));
      }
    }

    std::vector<std::string> compliant_str_files = {
      "duplicated_nodes_edges.json",
    };

    for (const auto& filename : compliant_str_files) {
      DYNAMIC_SECTION(filename) {
        REQUIRE_NOTHROW(
          reticula::read_hif<reticula::undirected_hyperedge<std::string>>(
            compliant_dir / filename));
      }
    }
  }

  SECTION("directed compliant files should load without throwing") {
    std::vector<std::string> directed_files = {
      // "missing_direction.json",
      "valid_incidence_head.json",
      "valid_incidence_tail.json"
    };

    for (const auto& filename : directed_files) {
      DYNAMIC_SECTION(filename) {
        REQUIRE_NOTHROW(
          reticula::read_hif<reticula::directed_hyperedge<int>>(
            compliant_dir / filename));
      }
    }
  }

  SECTION("undirected non-compliant files should throw exceptions") {
    std::vector<std::string> non_compliant_files = {
      "bad_network_type.json",
      "bad_node_float.json",
      "bad_node_without_id.json",
      "empty.json",
      "missing_required_field_incidence.json"
    };

    for (const auto& filename : non_compliant_files) {
      DYNAMIC_SECTION(filename) {
        REQUIRE_THROWS_AS(
          reticula::read_hif<reticula::undirected_hyperedge<int>>(
            non_compliant_dir / filename),
          std::runtime_error);
      }
    }
  }

  SECTION("directed non-compliant files should throw exceptions") {
    std::vector<std::string> directed_files = {
      "bad_node_float.json",
      "invalid_direction_value.json",
      "missing_required_fields_with_direction.json",
      "single_incidence_with_direction_not_in_enum.json"
    };

    for (const auto& filename : directed_files) {
      DYNAMIC_SECTION(filename) {
        REQUIRE_THROWS_AS(
          reticula::read_hif<reticula::directed_hyperedge<int>>(
            non_compliant_dir / filename),
          std::runtime_error);
      }
    }
  }
}
