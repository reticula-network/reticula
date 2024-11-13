#include <cmath>
#include <functional>
#include <random>
#include <unordered_set>

#include "../../include/reticula/network_concepts.hpp"
#include "../../include/reticula/networks.hpp"

namespace reticula {
  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_hypernetwork<VertT>
  random_uniform_hypergraph(
      VertT size, VertT edge_degree, double edge_prob, Gen& generator) {
    if (size < 0)
      throw std::domain_error("size must be non-negative");

    if (edge_prob > 1.0 || edge_prob < 0.0)
      throw std::invalid_argument(
          "edge probability should be in [0,1] interval");

    if (edge_degree > size)
      throw std::invalid_argument(
          "edge_degree must be less than or equal to size");

    if (size == 0 || edge_prob == 0)
      return undirected_hypernetwork<VertT>(
          ranges::empty_view<undirected_hyperedge<VertT>>(),
          views::iota(VertT{}, size));

    std::vector<undirected_hyperedge<VertT>> edges;
    double logncm = std::lgamma(size + 1) -
                      std::lgamma(edge_degree + 1) -
                      std::lgamma(size - edge_degree + 1);
    const double ncm_estimate = std::exp(logncm + std::log(edge_prob));

    if (ncm_estimate > static_cast<double>(
        std::numeric_limits<std::size_t>::max()))
      throw std::invalid_argument(
          "cannot generate a hypergraph with that many edges");

    edges.reserve(static_cast<std::size_t>(ncm_estimate*1.1));

    std::uniform_real_distribution<> rd;

    const double lp = std::log(1.0 - edge_prob);
    double lr = std::log(1.0 - rd(generator));
    auto carry = static_cast<std::size_t>(std::floor(lr/lp));

    std::vector<VertT> current(static_cast<std::size_t>(edge_degree));
    bool finished = false;

    while (!finished) {
      for (std::size_t i{}; i < static_cast<std::size_t>(edge_degree); i++) {
        std::size_t temp_sum = static_cast<std::size_t>(current[i]) + carry;
        current[i] = static_cast<VertT>(
          temp_sum % static_cast<std::size_t>(size));
        carry = temp_sum/static_cast<std::size_t>(size);
      }

      if (carry > 0) {
        finished = true;
      } else {
        if (std::ranges::adjacent_find(
            current, std::greater_equal<VertT>{}) == current.end())
          edges.emplace_back(current);
      }

      lr = std::log(1.0 - rd(generator));
      carry = static_cast<std::size_t>(1 + std::floor(lr/lp));
    }

    return undirected_hypernetwork<VertT>(edges,
        views::iota(VertT{}, size));
  }

  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  directed_hypernetwork<VertT>
  random_directed_uniform_hypergraph(
      VertT size, VertT edge_in_degree,
      VertT edge_out_degree, double edge_prob,
      Gen& generator) {
    if (size < 0)
      throw std::domain_error("size must be non-negative");

    if (edge_prob > 1.0 || edge_prob < 0.0)
      throw std::invalid_argument(
          "edge probability should be in [0,1] interval");

    if (edge_in_degree > size)
      throw std::invalid_argument(
          "edge_in_degree must be less than or equal to size");

    if (edge_out_degree > size)
      throw std::invalid_argument(
          "edge_out_degree must be less than or equal to size");

    if (size == 0 || edge_prob == 0)
      return directed_hypernetwork<VertT>(
          ranges::empty_view<directed_hyperedge<VertT>>(),
          views::iota(VertT{}, size));

    VertT edge_degree = edge_in_degree + edge_out_degree;

    std::vector<directed_hyperedge<VertT>> edges;
    double logncm = std::lgamma(size + 1) -
                      std::lgamma(edge_in_degree + 1) -
                      std::lgamma(size - edge_in_degree + 1);
    logncm += std::lgamma(size + 1) -
                std::lgamma(edge_out_degree + 1) -
                std::lgamma(size - edge_out_degree + 1);

    double ncm_estimate = std::exp(logncm + std::log(edge_prob));

    if (ncm_estimate > static_cast<double>(
        std::numeric_limits<std::size_t>::max()))
      throw std::invalid_argument(
          "cannot generate a hypergraph with that many edges");

    edges.reserve(static_cast<std::size_t>(ncm_estimate*1.1));

    std::uniform_real_distribution<> rd;

    const double lp = std::log(1.0 - edge_prob);
    double lr = std::log(1.0 - rd(generator));
    auto carry = static_cast<std::size_t>(std::floor(lr/lp));

    std::vector<VertT> current(static_cast<std::size_t>(edge_degree));
    bool finished = false;

    while (!finished) {
      for (std::size_t i{}; i < static_cast<std::size_t>(edge_degree); i++) {
        std::size_t temp_sum = static_cast<std::size_t>(current[i]) + carry;
        current[i] = static_cast<VertT>(
          temp_sum % static_cast<std::size_t>(size));
        carry = temp_sum/static_cast<std::size_t>(size);
      }

      if (carry > 0) {
        finished = true;
      } else {
        auto split = current.begin() +
          static_cast<std::ptrdiff_t>(edge_in_degree);
        std::ranges::subrange
          in{current.begin(), split}, out{split, current.end()};
        if (std::ranges::adjacent_find(
              in, std::greater_equal<VertT>{}) == in.end() &&
            std::ranges::adjacent_find(
              out, std::greater_equal<VertT>{}) == out.end())
          edges.emplace_back(in, out);
      }

      lr = std::log(1.0 - rd(generator));
      carry = static_cast<std::size_t>(1 + std::floor(lr/lp));
    }

    return directed_hypernetwork<VertT>(edges,
        views::iota(VertT{}, size));
  }
}  // namespace reticula
