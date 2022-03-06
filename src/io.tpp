#include <csv.hpp>

namespace dag {
  template <network_vertex VertT>
  undirected_network<VertT>
  read_undirected_edgelist(
          std::filesystem::path path, char delimiter, char quote) {
    csv::CSVFormat format;
    format.delimiter(delimiter);
    format.quote(quote);
    format.no_header();

    csv::CSVReader reader(path.string(), format);

    std::vector<undirected_edge<VertT>> edges;
    for (auto& row: reader)
      edges.emplace_back(row[0].get<VertT>(), row[1].get<VertT>());

    return undirected_network<VertT>(edges);
  }

  template <network_vertex VertT>
  directed_network<VertT>
  read_directed_edgelist(
          std::filesystem::path path, char delimiter, char quote) {
    csv::CSVFormat format;
    format.delimiter(delimiter);
    format.quote(quote);
    format.no_header();

    csv::CSVReader reader(path.string(), format);

    std::vector<directed_edge<VertT>> edges;
    for (auto& row: reader)
      edges.emplace_back(row[0].get<VertT>(), row[1].get<VertT>());

    return directed_network<VertT>(edges);
  }

  template <network_vertex VertT, typename TimeT>
  undirected_temporal_network<VertT, TimeT>
  read_undirected_eventlist(
          std::filesystem::path path, char delimiter, char quote) {
    csv::CSVFormat format;
    format.delimiter(delimiter);
    format.quote(quote);
    format.no_header();

    csv::CSVReader reader(path.string(), format);

    std::vector<undirected_temporal_edge<VertT, TimeT>> edges;
    for (auto& row: reader)
      edges.emplace_back(
          row[0].get<VertT>(), row[1].get<VertT>(), row[2].get<TimeT>());

    return undirected_temporal_network<VertT, TimeT>(edges);
  }

  template <network_vertex VertT, typename TimeT>
  directed_temporal_network<VertT, TimeT>
  read_directed_eventlist(
          std::filesystem::path path, char delimiter, char quote) {
    csv::CSVFormat format;
    format.delimiter(delimiter);
    format.quote(quote);
    format.no_header();

    csv::CSVReader reader(path.string(), format);

    std::vector<directed_temporal_edge<VertT, TimeT>> edges;
    for (auto& row: reader)
      edges.emplace_back(
          row[0].get<VertT>(), row[1].get<VertT>(), row[2].get<TimeT>());

    return directed_temporal_network<VertT, TimeT>(edges);
  }

  template <network_vertex VertT, typename TimeT>
  directed_delayed_temporal_network<VertT, TimeT>
  read_directed_delayed_eventlist(
          std::filesystem::path path, char delimiter, char quote) {
    csv::CSVFormat format;
    format.delimiter(delimiter);
    format.quote(quote);
    format.no_header();

    csv::CSVReader reader(path.string(), format);

    std::vector<directed_delayed_temporal_edge<VertT, TimeT>> edges;
    for (auto& row: reader)
      edges.emplace_back(
          row[0].get<VertT>(), row[1].get<VertT>(),
          row[2].get<TimeT>(), row[3].get<TimeT>());

    return directed_delayed_temporal_network<VertT, TimeT>(edges);
  }
}  // namespace dag
