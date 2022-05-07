#include <csv.hpp>

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
  }  // namespace detail

  template <network_edge EdgeT>
  network<EdgeT> read_edgelist(
          std::filesystem::path path, char delimiter, char quote) {
    csv::CSVFormat format;
    format.delimiter(delimiter);
    format.quote(quote);
    format.no_header();

    csv::CSVReader reader(std::filesystem::canonical(path).string(), format);

    std::vector<EdgeT> edges;
    for (auto& row: reader)
      edges.push_back(detail::edge_from_edgelist_row<EdgeT>{}(row));

    return network<EdgeT>(edges);
  }
}  // namespace reticula
