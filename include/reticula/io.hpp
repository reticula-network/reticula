#ifndef INCLUDE_RETICULA_IO_HPP_
#define INCLUDE_RETICULA_IO_HPP_

#include <filesystem>
#include <utility>

#include <csv.hpp>
#include <nlohmann/json.hpp>
#include <variant>

#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  template <
    network_edge EdgeT,
    ranges::input_range VertRange>
  network<EdgeT> read_edgelist(std::istream& in, VertRange&& extra_verts);

  template <network_edge EdgeT>
  network<EdgeT> read_edgelist(std::istream& in);


  template <
    network_edge EdgeT,
    ranges::input_range VertRange>
  network<EdgeT> read_edgelist(
      const std::filesystem::path& p,
      VertRange&& extra_verts);

  template <network_edge EdgeT>
  network<EdgeT> read_edgelist(
      const std::filesystem::path& path);


  template <network_edge EdgeT>
  void write_edgelist(
      const network<EdgeT>& network,
      std::filesystem::path path);


  template <network_edge EdgeT>
  network<EdgeT> read_hif(std::istream& in);

  template <network_edge EdgeT>
  network<EdgeT> read_hif(const std::filesystem::path& path);

  template <network_edge EdgeT>
  network<EdgeT> read_hif(std::istream& in);

  template <network_edge EdgeT>
  network<EdgeT> read_hif(const std::filesystem::path& path);

  template <network_edge EdgeT>
  void write_hif(const network<EdgeT>& net, std::ostream& out);

  template <network_edge EdgeT>
  void write_hif(
      const network<EdgeT>& net,
      const std::filesystem::path& path);
}  // namespace reticula

// Implementation
#include <filesystem>
#include <fstream>
#include <sstream>

#include <csv.hpp>
#include <nlohmann/json.hpp>

#include "networks.hpp"
#include "static_hyperedges.hpp"

namespace reticula {
  namespace detail {
    template <network_edge EdgeT>
    struct edge_from_edgelist_row {
      EdgeT operator()(const csv::CSVRow& row);
    };

    template <network_vertex VertT>
    struct edge_from_edgelist_row<undirected_edge<VertT>> {
      undirected_edge<VertT> operator()(const csv::CSVRow& row) {
        return {row[0].get<VertT>(), row[1].get<VertT>()};
      }
    };

    template <network_vertex VertT>
    struct edge_from_edgelist_row<directed_edge<VertT>> {
      directed_edge<VertT> operator()(const csv::CSVRow& row) {
        return {row[0].get<VertT>(), row[1].get<VertT>()};
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct edge_from_edgelist_row<undirected_temporal_edge<VertT, TimeT>> {
      undirected_temporal_edge<VertT, TimeT>
      operator()(const csv::CSVRow& row) {
        return {row[0].get<VertT>(), row[1].get<VertT>(), row[2].get<TimeT>()};
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct edge_from_edgelist_row<directed_temporal_edge<VertT, TimeT>> {
      directed_temporal_edge<VertT, TimeT>
      operator()(const csv::CSVRow& row) {
        return {row[0].get<VertT>(), row[1].get<VertT>(), row[2].get<TimeT>()};
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct edge_from_edgelist_row<
        directed_delayed_temporal_edge<VertT, TimeT>> {
      directed_delayed_temporal_edge<VertT, TimeT>
      operator()(const csv::CSVRow& row) {
        return {
          row[0].get<VertT>(), row[1].get<VertT>(),
          row[2].get<TimeT>(), row[3].get<TimeT>()};
      }
    };


    template <network_edge EdgeT>
    struct edgelist_row_from_edge {
      std::string operator()(const EdgeT& edge);
    };

    template <network_vertex VertT>
    struct edgelist_row_from_edge<undirected_edge<VertT>> {
      std::string operator()(const undirected_edge<VertT>& edge) {
        std::stringstream ss;
        auto ivs = edge.incident_verts();
        ss << ivs.front() << " " << ivs.back() << "\n";
        return ss.str();
      }
    };

    template <network_vertex VertT>
    struct edgelist_row_from_edge<directed_edge<VertT>> {
      std::string operator()(const directed_edge<VertT>& edge) {
        std::stringstream ss;
        ss << edge.tail() << " " << edge.head() << "\n";
        return ss.str();
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct edgelist_row_from_edge<undirected_temporal_edge<VertT, TimeT>> {
      std::string operator()(
          const undirected_temporal_edge<VertT, TimeT>& edge) {
        std::stringstream ss;
        ss.precision(std::numeric_limits<TimeT>::max_digits10);
        auto ivs = edge.incident_verts();
        ss << ivs.front() << " " << ivs.back()
          << " " << edge.cause_time() << "\n";
        return ss.str();
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct edgelist_row_from_edge<directed_temporal_edge<VertT, TimeT>> {
      std::string operator()(
          const directed_temporal_edge<VertT, TimeT>& edge) {
        std::stringstream ss;
        ss.precision(std::numeric_limits<TimeT>::max_digits10);
        ss << edge.tail() << " " << edge.head() <<
          " " << edge.cause_time() << "\n";
        return ss.str();
      }
    };

    template <network_vertex VertT, typename TimeT>
    struct edgelist_row_from_edge<
        directed_delayed_temporal_edge<VertT, TimeT>> {
      std::string operator()(
          const directed_delayed_temporal_edge<VertT, TimeT>& edge) {
        std::stringstream ss;
        ss.precision(std::numeric_limits<TimeT>::max_digits10);
        ss << edge.tail() << " " << edge.head() <<
          " " << edge.cause_time() << " " << edge.effect_time() << "\n";
        return ss.str();
      }
    };

    template <network_edge EdgeT>
    auto edgelist_edges(csv::CSVReader& rows) {
      return views::transform(
        rows, [](auto&& row) -> EdgeT {
          return edge_from_edgelist_row<EdgeT>{}(row);
        });
    }

    template <network_edge EdgeT>
    auto edgelist_rows(const network<EdgeT>& net) {
      return views::transform(net.edges(), [](const EdgeT& e) {
        return edgelist_row_from_edge<EdgeT>{}(e);
      });
    }

    template <
      network_edge EdgeT, ranges::input_range VertRange>
    network<EdgeT> read_edgelist(
        csv::CSVReader& rows, VertRange&& extra_verts) {
      auto edge_view = detail::edgelist_edges<EdgeT>(rows);
      return network<EdgeT>{edge_view, std::forward<VertRange>(extra_verts)};
    }

    template <network_edge EdgeT>
    network<EdgeT> read_edgelist(csv::CSVReader& rows) {
      return read_edgelist<EdgeT>(
        rows, std::vector<typename EdgeT::VertexType>{});
    }

  }  // namespace detail

  template <
    network_edge EdgeT,
    ranges::input_range VertRange>
  network<EdgeT> read_edgelist(std::istream& in, VertRange&& extra_verts) {
    csv::CSVFormat fmt;
    fmt.delimiter(' ').quote('"').no_header();

    csv::CSVReader reader(in, fmt);
    return detail::read_edgelist<EdgeT>(
      reader, std::forward<VertRange>(extra_verts));
  }

  template <network_edge EdgeT>
  network<EdgeT> read_edgelist(std::istream& in) {
    return read_edgelist<EdgeT>(in, std::vector<typename EdgeT::VertexType>{});
  }

  template <
    network_edge EdgeT,
    ranges::input_range VertRange>
  network<EdgeT> read_edgelist(
      const std::filesystem::path& p,
      VertRange&& extra_verts) {
    csv::CSVFormat fmt;
    fmt.delimiter(' ').quote('"').no_header();
    csv::CSVReader reader(std::filesystem::canonical(p).string(), fmt);

    return detail::read_edgelist<EdgeT>(
      reader, std::forward<VertRange>(extra_verts));
  }

  template <network_edge EdgeT>
  network<EdgeT> read_edgelist(
      const std::filesystem::path& p) {
    return read_edgelist<EdgeT>(
        p, std::vector<typename EdgeT::VertexType>{});
  }


  // writes

  template <network_edge EdgeT>
  void write_edgelist(const network<EdgeT>& net, std::ostream& out) {
    for (auto&& row : detail::edgelist_rows(net))
      out << row;
  }

  template <network_edge EdgeT>
  void write_edgelist(
      const network<EdgeT>& network,
      std::filesystem::path path) {
    std::ofstream file;
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    file.open(path, std::ofstream::out | std::ofstream::trunc);

    write_edgelist<EdgeT>(network, file);
  }


  // HIF functions

  namespace detail {
  template <network_vertex VertT>
  void validate_network_type(const nlohmann::json& hif_data, bool is_directed) {
    if (hif_data.contains("network-type")) {
      std::string network_type = hif_data["network-type"];
      if (is_directed && network_type != "directed")
        throw std::runtime_error(
          "HIF network-type '" + network_type +
          "' does not match expected directed hypergraph");
      if (!is_directed && network_type != "undirected")
        throw std::runtime_error(
          "HIF network-type '" + network_type +
          "' does not match expected undirected hypergraph");
    }
  }

  template <network_vertex VertT>
  void validate_node_type(const nlohmann::json& node_json) {
    if constexpr (std::is_same_v<VertT, std::string>) {
      if (!node_json.is_string())
        throw std::runtime_error(
          "HIF node must be string type, got: " +
          std::string(node_json.type_name()));
    } else if constexpr (std::is_integral_v<VertT>) {
      if (!node_json.is_number_integer())
        throw std::runtime_error(
          "HIF node must be integer type, got: " +
          std::string(node_json.type_name()));
    } else {
      throw std::runtime_error(
        "HIF node type must be string or integer");
    }
  }

    template <network_vertex VertT>
    network<undirected_hyperedge<VertT>>
    read_undirected_hif(const nlohmann::json& hif_data) {
      validate_network_type<VertT>(hif_data, false);

      if (!hif_data.contains("incidences"))
        throw std::runtime_error(
          "HIF file missing required 'incidences' field");

      std::unordered_map<
        std::variant<std::string, std::int64_t>, std::vector<VertT>>
        edge_to_vertices;
      std::vector<VertT> nodes;
      for (const auto& incidence : hif_data["incidences"]) {
        if (!incidence.contains("node"))
          throw std::runtime_error(
            "HIF incidence missing 'node' field");

        if (!incidence.contains("edge"))
          throw std::runtime_error(
            "HIF incidence missing 'edge' field");

        validate_node_type<VertT>(incidence["node"]);
        VertT node = incidence["node"].get<VertT>();
        auto edge_id = incidence["edge"];

        std::variant<std::string, std::int64_t> e;
        if (edge_id.is_string())
          e = edge_id.get<std::string>();
        else if (edge_id.is_number_integer())
          e = edge_id.get<std::int64_t>();
        else
          throw std::runtime_error(
            "HIF incidence 'edge' field must be string or integer");

        edge_to_vertices[e].push_back(node);
        nodes.push_back(node);
      }

      std::vector<undirected_hyperedge<VertT>> edges;
      for (const auto& [edge_id, vertices] : edge_to_vertices)
        edges.emplace_back(vertices);

      if (hif_data.contains("nodes")) {
        for (const auto& node_entry : hif_data["nodes"]) {
          if (!node_entry.contains("node"))
            throw std::runtime_error("HIF node entry missing 'node' field");
          validate_node_type<VertT>(node_entry["node"]);
          VertT node = node_entry["node"].get<VertT>();
          nodes.push_back(node);
        }
      }

      return network<undirected_hyperedge<VertT>>{edges, nodes};
    }

    template <network_vertex VertT>
    network<directed_hyperedge<VertT>> read_directed_hif(
        const nlohmann::json& hif_data) {
      validate_network_type<VertT>(hif_data, true);

      if (!hif_data.contains("incidences"))
        throw std::runtime_error(
          "HIF file missing required 'incidences' field");

      std::unordered_map<
        std::variant<std::string, std::int64_t>,
        std::pair<std::vector<VertT>, std::vector<VertT>>>
        edge_to_vertices;

      for (const auto& incidence : hif_data["incidences"]) {
        if (!incidence.contains("node") || !incidence.contains("edge"))
          throw std::runtime_error(
            "HIF incidence missing 'node' or 'edge' field");

        validate_node_type<VertT>(incidence["node"]);
        VertT node = incidence["node"].get<VertT>();
        auto edge_id = incidence["edge"];

        std::variant<std::string, std::int64_t> e;
        if (edge_id.is_string())
          e = edge_id.get<std::string>();
        else if (edge_id.is_number_integer())
          e = edge_id.get<std::int64_t>();
        else
          throw std::runtime_error(
            "HIF incidence 'edge' field must be string or integer");

        if (incidence.contains("direction")) {
          std::string direction = incidence["direction"];
          if (direction == "tail")
            edge_to_vertices[e].first.push_back(node);
          else if (direction == "head")
            edge_to_vertices[e].second.push_back(node);
          else
            throw std::runtime_error(
              "Invalid direction '" + direction + "' in HIF incidence");
        } else {
          throw std::runtime_error(
            "HIF incidence for directed hypergraph missing 'direction' field");
        }
      }

      std::vector<directed_hyperedge<VertT>> edges;
      for (const auto& [edge_id, vecs] : edge_to_vertices)
        edges.emplace_back(vecs.first, vecs.second);

      std::vector<VertT> nodes;
      if (hif_data.contains("nodes")) {
        for (const auto& node_entry : hif_data["nodes"]) {
          if (!node_entry.contains("node"))
            throw std::runtime_error("HIF node entry missing 'node' field");
          validate_node_type<VertT>(node_entry["node"]);
          VertT node = node_entry["node"].get<VertT>();
          nodes.push_back(node);
        }
      }

      return network<directed_hyperedge<VertT>>{edges, nodes};
    }

    template <network_vertex VertT>
    nlohmann::json
    write_undirected_hif(const network<undirected_hyperedge<VertT>>& net) {
      nlohmann::json hif_data;
      hif_data["network-type"] = "undirected";
      hif_data["incidences"] = nlohmann::json::array();

      std::size_t edge_counter = 0;
      for (const auto& edge : net.edges()) {
        for (const auto& vertex : edge.incident_verts()) {
          nlohmann::json incidence;
          incidence["node"] = vertex;
          incidence["edge"] = edge_counter;
          hif_data["incidences"].push_back(incidence);
        }
        ++edge_counter;
      }

      hif_data["nodes"] = nlohmann::json::array();
      for (const auto& vertex : net.vertices()) {
        nlohmann::json node_entry;
        node_entry["node"] = vertex;
        hif_data["nodes"].push_back(node_entry);
      }

      return hif_data;
    }

    template <network_vertex VertT>
    nlohmann::json
    write_directed_hif(const network<directed_hyperedge<VertT>>& net) {
      nlohmann::json hif_data;
      hif_data["network-type"] = "directed";
      hif_data["incidences"] = nlohmann::json::array();

      std::size_t edge_counter = 0;

      for (const auto& edge : net.edges()) {
        for (const auto& vertex : edge.tails()) {
          nlohmann::json incidence;
          incidence["node"] = vertex;
          incidence["edge"] = edge_counter;
          incidence["direction"] = "tail";
          hif_data["incidences"].push_back(incidence);
        }

        for (const auto& vertex : edge.heads()) {
          nlohmann::json incidence;
          incidence["node"] = vertex;
          incidence["edge"] = edge_counter;
          incidence["direction"] = "head";
          hif_data["incidences"].push_back(incidence);
        }
        ++edge_counter;
      }

      hif_data["nodes"] = nlohmann::json::array();
      for (const auto& vertex : net.vertices()) {
        nlohmann::json node_entry;
        node_entry["node"] = vertex;
        hif_data["nodes"].push_back(node_entry);
      }

      return hif_data;
    }
  }  // namespace detail

  template <network_edge EdgeT>
  network<EdgeT> read_hif(const std::filesystem::path& path) {
    std::ifstream file;
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    file.open(path, std::ifstream::in);

    return read_hif<EdgeT>(file);
  }

  template <network_edge EdgeT>
  network<EdgeT> read_hif(std::istream& in) {
    nlohmann::json hif_data;
    in >> hif_data;

    if constexpr (is_undirected_v<EdgeT>)
      return detail::read_undirected_hif<typename EdgeT::VertexType>(hif_data);
    else
      return detail::read_directed_hif<typename EdgeT::VertexType>(hif_data);
  }

  template <network_edge EdgeT>
  void write_hif(const network<EdgeT>& net, std::ostream& out) {
    nlohmann::json hif_data;

    if constexpr (is_undirected_v<EdgeT>)
      hif_data = detail::write_undirected_hif(net);
    else
      hif_data = detail::write_directed_hif(net);

    out << hif_data.dump();
  }

  template <network_edge EdgeT>
  void write_hif(const network<EdgeT>& net, const std::filesystem::path& path) {
    std::ofstream file;
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    file.open(path, std::ofstream::out | std::ofstream::trunc);
    write_hif(net, file);
  }
}  // namespace reticula


#endif  // INCLUDE_RETICULA_IO_HPP_

