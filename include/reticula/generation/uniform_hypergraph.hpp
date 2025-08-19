#pragma once

#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_uniform_hypergraph(
  VertexType size, VertexType edge_degree, double edge_prob, Gen& gen)
  -> undirected_hypernetwork;

template <std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_directed_uniform_hypergraph(
  VertexType size, VertexType edge_in_degree, VertexType edge_out_degree,
  double edge_prob, Gen& gen) -> directed_hypernetwork;
} // namespace reticula

namespace reticula {
template <std::uniform_random_bit_generator Gen>
auto random_uniform_hypergraph(
  VertexType size, VertexType edge_degree, double edge_prob, Gen& gen)
  -> undirected_hypernetwork {

  if (edge_prob > 1.0 || edge_prob < 0.0)
    throw std::invalid_argument("edge probability should be in [0,1] interval");

  if (edge_degree > size)
    throw std::invalid_argument(
      "edge_degree must be less than or equal to size");

  if (size == 0 || edge_prob == 0)
    return undirected_hypernetwork(
      std::views::empty<undirected_hyperedge>,
      std::views::iota(VertexType{}, size));

  std::vector<undirected_hyperedge> edges;
  double logncm = std::lgamma(size + 1) - std::lgamma(edge_degree + 1) -
                  std::lgamma(size - edge_degree + 1);
  const double ncm_estimate = std::exp(logncm + std::log(edge_prob));

  if (
    ncm_estimate > static_cast<double>(std::numeric_limits<std::size_t>::max()))
    throw std::invalid_argument(
      "cannot generate a hypergraph with that many edges");

  edges.reserve(static_cast<std::size_t>(ncm_estimate * 1.1));

  std::uniform_real_distribution rd;

  const double lp = std::log1p(-edge_prob);
  double lr = std::log1p(-rd(gen));
  auto carry = static_cast<std::size_t>(std::floor(lr / lp));

  std::vector<VertexType> current(edge_degree);
  bool finished = false;

  while (!finished) {
    for (std::size_t i{}; i < edge_degree; i++) {
      std::size_t temp_sum = current[i] + carry;
      current[i] = temp_sum % size;
      carry = temp_sum / size;
    }

    if (carry > 0) {
      finished = true;
    } else {
      if (
        std::ranges::adjacent_find(current, std::ranges::greater_equal{}) ==
        current.end())
        edges.emplace_back(current);
    }

    lr = std::log1p(-rd(gen));
    carry = static_cast<std::size_t>(1 + std::floor(lr / lp));
  }

  return undirected_hypernetwork(edges, std::views::iota(VertexType{}, size));
}

template <std::uniform_random_bit_generator Gen>
auto random_directed_uniform_hypergraph(
  VertexType size, VertexType edge_in_degree, VertexType edge_out_degree,
  double edge_prob, Gen& gen) -> directed_hypernetwork {
  if (edge_prob > 1.0 || edge_prob < 0.0)
    throw std::invalid_argument("edge probability should be in [0,1] interval");

  if (edge_in_degree > size)
    throw std::invalid_argument(
      "edge_in_degree must be less than or equal to size");

  if (edge_out_degree > size)
    throw std::invalid_argument(
      "edge_out_degree must be less than or equal to size");

  if (size == 0 || edge_prob == 0)
    return directed_hypernetwork(
      std::views::empty<directed_hyperedge>,
      std::views::iota(VertexType{}, size));

  VertexType edge_degree = edge_in_degree + edge_out_degree;

  std::vector<directed_hyperedge> edges;
  double logncm = std::lgamma(size + 1) - std::lgamma(edge_in_degree + 1) -
                  std::lgamma(size - edge_in_degree + 1);
  logncm += std::lgamma(size + 1) - std::lgamma(edge_out_degree + 1) -
            std::lgamma(size - edge_out_degree + 1);

  double ncm_estimate = std::exp(logncm + std::log(edge_prob));

  if (
    ncm_estimate > static_cast<double>(std::numeric_limits<std::size_t>::max()))
    throw std::invalid_argument(
      "cannot generate a hypergraph with that many edges");

  edges.reserve(static_cast<std::size_t>(ncm_estimate * 1.1));

  std::uniform_real_distribution<> rd;

  const double lp = std::log1p(-edge_prob);
  double lr = std::log1p(-rd(gen));
  auto carry = static_cast<std::size_t>(std::floor(lr / lp));

  std::vector<VertexType> current(static_cast<std::size_t>(edge_degree));
  bool finished = false;

  while (!finished) {
    for (std::size_t i{}; i < edge_degree; i++) {
      std::size_t temp_sum = current[i] + carry;
      current[i] = temp_sum % size;
      carry = temp_sum / size;
    }

    if (carry > 0) {
      finished = true;
    } else {
      auto split =
        current.begin() + static_cast<std::ptrdiff_t>(edge_in_degree);
      std::ranges::subrange in{current.begin(), split},
        out{split, current.end()};
      if (
        std::ranges::adjacent_find(in, std::greater_equal<VertexType>{}) ==
          in.end() &&
        std::ranges::adjacent_find(out, std::greater_equal<VertexType>{}) ==
          out.end())
        edges.emplace_back(in, out);
    }

    lr = std::log1p(-rd(gen));
    carry = static_cast<std::size_t>(1 + std::floor(lr / lp));
  }

  return directed_hypernetwork(edges, std::views::iota(VertexType{}, size));
}
} // namespace reticula
