#pragma once

#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_fully_mixed_temporal_network(
  VertexType n, double rate, double max_t, Gen& gen)
  -> undirected_temporal_network;

template <std::uniform_random_bit_generator Gen>
[[nodiscard]] auto random_directed_fully_mixed_temporal_network(
  VertexType n, double rate, double max_t, Gen& gen)
  -> directed_temporal_network;
} // namespace reticula

#include <reticula/generation/deterministic.hpp>
#include <reticula/generation/link_activation.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen>
auto random_fully_mixed_temporal_network(
  VertexType n, double rate, double max_t, Gen& gen)
  -> undirected_temporal_network {
  return random_link_activation_temporal_network(
    complete_graph(n), max_t, std::exponential_distribution<double>(rate),
    std::exponential_distribution<double>(rate), gen,
    static_cast<std::size_t>(max_t * rate + 6 * std::sqrt(max_t * rate)));
}

template <std::uniform_random_bit_generator Gen>
auto random_directed_fully_mixed_temporal_network(
  VertexType n, double rate, double max_t, Gen& gen)
  -> directed_temporal_network {
  return random_link_activation_temporal_network(
    complete_directed_graph(n), max_t,
    std::exponential_distribution<double>(rate),
    std::exponential_distribution<double>(rate), gen,
    static_cast<std::size_t>(max_t * rate + 6 * std::sqrt(max_t * rate)));
}
} // namespace reticula
