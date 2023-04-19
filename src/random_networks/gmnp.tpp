#include <random>

#include "../../include/reticula/network_concepts.hpp"
#include "../../include/reticula/networks.hpp"

namespace reticula {
  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_hypernetwork<VertT>
  random_gmnp_hypergraph(VertT m, VertT n, double p, Gen& generator) {
    if (n < 0)
      throw std::domain_error("n must be non-negative");

    if (p > 1.0 || p < 0.0)
      throw std::invalid_argument(
          "edge probability p should be in [0,1] interval");

    if (n == 0 || p == 0)
      return undirected_hypernetwork<VertT>(
          ranges::empty_view<undirected_hyperedge<VertT>>(),
          views::iota(VertT{}, n));

    // std::vector<undirected_hyperedge<VertT>> edges;
    // edges.reserve(static_cast<std::size_t>(
    //       static_cast<double>(n)*static_cast<double>(n/2)*p));
    //
    // std::uniform_real_distribution<> rd;
    //
    // double lp = std::log(1.0 - p);
    // VertT v = 1;
    // VertT w = static_cast<VertT>(
    //     std::floor(std::log(1.0 - rd(generator))/lp));
    //
    // while (v < n) {
    //   while (w >= v && v < n) {
    //     w = w - v;
    //     v = v + 1;
    //   }
    //
    //   if (v < n)
    //     edges.emplace_back(v, w);
    //
    //   double lr = std::log(1.0 - rd(generator));
    //   w = w + static_cast<VertT>(1 + std::floor(lr/lp));
    // }
    //
    // return undirected_hypernetwork<VertT>(edges,
    //     views::iota(VertT{}, n));
  }

  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  directed_hypernetwork<VertT>
  random_directed_gmnp_hypergraph(VertT m, VertT n, double p, Gen& generator) {
    if (n < 0)
      throw std::domain_error("n must be non-negative");

    if (p > 1.0 || p < 0.0)
      throw std::invalid_argument(
          "edge probability p should be in [0,1] interval");

    if (n == 0 || p == 0)
      return directed_network<VertT>(
          ranges::empty_view<directed_hyperedge<VertT>>(),
          views::iota(VertT{}, n));

    // std::vector<directed_hyperedge<VertT>> edges;
    // edges.reserve(static_cast<std::size_t>(
    //       static_cast<double>(n)*static_cast<double>(n)*p));
    //
    // std::uniform_real_distribution<> rd;
    //
    // double lp = std::log(1.0 - p);
    // VertT v = 1;
    // VertT w = static_cast<VertT>(
    //     std::floor(std::log(1.0 - rd(generator))/lp));
    //
    // while (v < n) {
    //   while (w >= (n - 1) && v < n) {
    //     w = w - (n - 1);
    //     v = v + 1;
    //   }
    //
    //   if (v < n)
    //     edges.emplace_back(v, w < v ? w : w + 1);
    //
    //   double lr = std::log(1.0 - rd(generator));
    //   w = w + static_cast<VertT>(1 + std::floor(lr/lp));
    // }
    //
    // return directed_network<VertT>(edges,
    //     views::iota(VertT{}, n));
  }
}  // namespace reticula
