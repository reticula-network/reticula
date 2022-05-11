#ifndef INCLUDE_RETICULA_IO_HPP_
#define INCLUDE_RETICULA_IO_HPP_

#include <filesystem>

#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  template <network_edge EdgeT>
  network<EdgeT> read_edgelist(
      std::filesystem::path path);

  template <network_edge EdgeT>
  void write_edgelist(
      const network<EdgeT>& network,
      std::filesystem::path path);
}  // namespace reticula

#include "../../src/io.tpp"

#endif  // INCLUDE_RETICULA_IO_HPP_

