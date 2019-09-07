#ifndef DAG_RANDOM_NETWORKS_H
#define DAG_RANDOM_NETWORKS_H

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

#endif /* DAG_RANDOM_NETWORKS_H */
