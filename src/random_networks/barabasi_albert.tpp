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

    std::vector<VertT> degrees(static_cast<std::size_t>(n));

    for (VertT i = 0; i < m; i++) {
      edges.emplace_back(i, m);
      degrees[static_cast<std::size_t>(i)] += 1;
      degrees[static_cast<std::size_t>(m)] += 1;
    }

    for (VertT current_node = m+1; current_node < n; current_node++) {
      std::discrete_distribution<VertT> dist(
          degrees.begin(),
          degrees.begin() + static_cast<std::ptrdiff_t>(current_node));

      std::unordered_set<VertT> targets;
      targets.reserve(static_cast<std::size_t>(m));

      while (targets.size() < static_cast<std::size_t>(m))
        targets.insert(dist(generator));

      for (const auto& i: targets) {
        edges.emplace_back(current_node, i);
        degrees[static_cast<std::size_t>(i)] += 1;
        degrees[static_cast<std::size_t>(current_node)] += 1;
      }
    }

    return undirected_network<VertT>(edges);
  }
}  // namespace reticula
