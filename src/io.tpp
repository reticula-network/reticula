#include <csv.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "../include/reticula/networks.hpp"

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
  }  // namespace detail

  template <network_edge EdgeT>
  network<EdgeT> read_edgelist(std::filesystem::path path) {
    csv::CSVFormat format;
    format.delimiter(' ');
    format.quote('"');
    format.no_header();

    csv::CSVReader reader(std::filesystem::canonical(path).string(), format);

    std::vector<EdgeT> edges;
    for (auto& row: reader)
      edges.push_back(detail::edge_from_edgelist_row<EdgeT>{}(row));

    return network<EdgeT>(edges);
  }

  template <network_edge EdgeT>
  void write_edgelist(
      const network<EdgeT>& network,
      std::filesystem::path path) {
    std::ofstream file;
    file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    file.open(path, std::ofstream::out | std::ofstream::trunc);

    for (auto&& edge: network.edges())
      file << detail::edgelist_row_from_edge<EdgeT>{}(edge);
  }
}  // namespace reticula
