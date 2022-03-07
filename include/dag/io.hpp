#ifndef INCLUDE_DAG_IO_HPP_
#define INCLUDE_DAG_IO_HPP_

#include <filesystem>

#include "network_concepts.hpp"
#include "networks.hpp"

namespace dag {
  template <network_edge EdgeT>
  network<EdgeT>
  read_edgelist(std::filesystem::path path,
          char delimiter = ' ', char quote = '"');
}  // namespace dag

#include "../../src/io.tpp"

#endif  // INCLUDE_DAG_IO_HPP_

