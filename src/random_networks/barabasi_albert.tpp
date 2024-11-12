#include <cstddef>
#include <random>

#include "../../include/reticula/network_concepts.hpp"
#include "../../include/reticula/networks.hpp"

namespace reticula {
  template <
    integer_network_vertex VertT,
    std::uniform_random_bit_generator Gen>
  undirected_network<VertT>
  random_barabasi_albert_graph(
      VertT n, VertT m,
      Gen& generator) {
    if (n <= m || m < 1)
      throw std::invalid_argument(
          "BA network must have m >= 1 and n > m");

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(m*(n-m)));

    std::vector<VertT> repeated_nodes;
    repeated_nodes.reserve(static_cast<std::size_t>(2 * m * (n - m)));


    for (VertT i = 0; i < m; i++) {
      edges.emplace_back(i, m);
      repeated_nodes.push_back(i);
      repeated_nodes.push_back(m);
    }

    for (VertT current_node = m+1; current_node < n; current_node++) {
      std::uniform_int_distribution<std::size_t> dist(
        0, repeated_nodes.size()-1);

      std::unordered_set<VertT> targets;
      targets.reserve(static_cast<std::size_t>(m));

      while (targets.size() < static_cast<std::size_t>(m))
        targets.insert(repeated_nodes[dist(generator)]);

      for (const auto& i: targets) {
        edges.emplace_back(current_node, i);
        repeated_nodes.push_back(i);
        repeated_nodes.push_back(current_node);
      }
    }

    return undirected_network<VertT>(edges);
  }
}  // namespace reticula
