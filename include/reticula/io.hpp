#ifndef INCLUDE_RETICULA_IO_HPP_
#define INCLUDE_RETICULA_IO_HPP_

#include <filesystem>
#include <utility>

#include <csv.hpp>

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
}  // namespace reticula

// Implementation
#include <filesystem>
#include <fstream>
#include <sstream>

#include <csv.hpp>

#include "networks.hpp"

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
}  // namespace reticula


#endif  // INCLUDE_RETICULA_IO_HPP_

