#ifndef INCLUDE_DAG_RANDOM_NETWORKS_HPP_
#define INCLUDE_DAG_RANDOM_NETWORKS_HPP_

#include <random>

namespace dag {
  template <typename VertT>
  undirected_network<VertT> gnp_random_graph(size_t n, double p,
      std::mt19937_64& generator);

  template <typename VertT>
  undirected_network<VertT> ba_random_graph(size_t n, size_t m,
      std::mt19937_64& generator);
}

#include "../../src/random_networks.tpp"

#endif  // INCLUDE_DAG_RANDOM_NETWORKS_HPP_
