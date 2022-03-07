#ifndef INCLUDE_DAG_IO_HPP_
#define INCLUDE_DAG_IO_HPP_

#include <filesystem>

#include "network_concepts.hpp"
#include "networks.hpp"

namespace dag {
  template <network_vertex VertT>
  undirected_network<VertT>
  read_undirected_edgelist(std::filesystem::path path,
          char delimiter = ' ', char quote = '"');

  template <network_vertex VertT>
  directed_network<VertT>
  read_directed_edgelist(
          std::filesystem::path path,
          char delimiter = ' ', char quote = '"');

  template <network_vertex VertT, typename TimeT>
  undirected_temporal_network<VertT, TimeT>
  read_undirected_eventlist(std::filesystem::path path,
          char delimiter = ' ', char quote = '"');

  template <network_vertex VertT, typename TimeT>
  directed_temporal_network<VertT, TimeT>
  read_directed_eventlist(std::filesystem::path path,
          char delimiter = ' ', char quote = '"');

  template <network_vertex VertT, typename TimeT>
  directed_delayed_temporal_network<VertT, TimeT>
  read_directed_eventlist(std::filesystem::path path,
          char delimiter = ' ', char quote = '"');
}  // namespace dag

#include "../../src/io.tpp"

#endif  // INCLUDE_DAG_IO_HPP_

