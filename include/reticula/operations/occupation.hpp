#pragma once

#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <network_like NetT, std::uniform_random_bit_generator Gen>
[[nodiscard]]
auto uniformly_occupy_vertices(const NetT& g, double p, Gen& gen)
  -> reticula::network<typename NetT::EdgeType>;

template <
  network_like NetT, mapping<VertexType, double> ProbMapT,
  std::uniform_random_bit_generator Gen>
[[nodiscard]]
auto occupy_vertices(
  const NetT& g, const ProbMapT& prob_map, Gen& gen, double default_prob)
  -> reticula::network<typename NetT::EdgeType>;

template <
  network_like NetT, std::invocable<VertexType> ProbF,
  std::uniform_random_bit_generator Gen>
  requires std::convertible_to<std::invoke_result_t<ProbF, VertexType>, double>
[[nodiscard]]
auto occupy_vertices(const NetT& g, ProbF&& prob_func, Gen& gen)
  -> reticula::network<typename NetT::EdgeType>;

template <network_like NetT, std::uniform_random_bit_generator Gen>
[[nodiscard]]
auto uniformly_occupy_edges(const NetT& g, double p, Gen& gen)
  -> reticula::network<typename NetT::EdgeType>;

template <
  network_like NetT, mapping<typename NetT::EdgeType, double> ProbMapT,
  std::uniform_random_bit_generator Gen>
[[nodiscard]]
auto occupy_edges(
  const NetT& g, const ProbMapT& prob_map, Gen& gen, double default_prob)
  -> reticula::network<typename NetT::EdgeType>;

template <
  network_like NetT, std::invocable<typename NetT::EdgeType> ProbF,
  std::uniform_random_bit_generator Gen>
  requires std::convertible_to<
    std::invoke_result_t<ProbF, typename NetT::EdgeType>, double>
[[nodiscard]]
auto occupy_edges(const NetT& g, ProbF&& prob_func, Gen& gen)
  -> reticula::network<typename NetT::EdgeType>;
} // namespace reticula


#include <reticula/operations/removal.hpp>

namespace reticula {
template <
  network_like NetT, mapping<VertexType, double> ProbMapT,
  std::uniform_random_bit_generator Gen>
auto occupy_vertices(
  const NetT& g, const ProbMapT& prob_map, Gen& gen, double default_prob)
  -> reticula::network<typename NetT::EdgeType> {
  return occupy_vertices(
    g,
    [&](VertexType v) {
      auto it = prob_map.find(v);
      if (it != prob_map.end())
        return it->second;
      return default_prob;
    },
    gen);
}

template <
  network_like NetT, std::invocable<VertexType> ProbF,
  std::uniform_random_bit_generator Gen>
  requires std::convertible_to<std::invoke_result_t<ProbF, VertexType>, double>
auto occupy_vertices(const NetT& g, ProbF&& prob_func, Gen& gen)
  -> reticula::network<typename NetT::EdgeType> {
  return without_vertices(
    g, g.vertices() | std::views::filter([&prob_func, &gen](VertexType v) {
         return std::bernoulli_distribution{1.0 - prob_func(v)}(gen);
       }));
}

template <network_like NetT, std::uniform_random_bit_generator Gen>
auto uniformly_occupy_vertices(const NetT& g, double p, Gen& gen)
  -> reticula::network<typename NetT::EdgeType> {
  return occupy_vertices(g, [&](VertexType) { return p; }, gen);
}

template <
  network_like NetT, mapping<typename NetT::EdgeType, double> ProbMapT,
  std::uniform_random_bit_generator Gen>
auto occupy_edges(
  const NetT& g, const ProbMapT& prob_map, Gen& gen, double default_prob)
  -> reticula::network<typename NetT::EdgeType> {
  return occupy_edges(
    g,
    [&](const typename NetT::EdgeType& e) {
      auto it = prob_map.find(e);
      if (it != prob_map.end())
        return it->second;
      return default_prob;
    },
    gen);
}

template <
  network_like NetT, std::invocable<typename NetT::EdgeType> ProbF,
  std::uniform_random_bit_generator Gen>
  requires std::convertible_to<
    std::invoke_result_t<ProbF, typename NetT::EdgeType>, double>
auto occupy_edges(const NetT& g, ProbF&& prob_func, Gen& gen)
  -> reticula::network<typename NetT::EdgeType> {
  return without_edges(
    g, g.edges() | std::views::filter([&prob_func, &gen](const auto& e) {
         return std::bernoulli_distribution{1.0 - prob_func(e)}(gen);
       }));
}

template <network_like NetT, std::uniform_random_bit_generator Gen>
auto uniformly_occupy_edges(const NetT& g, double p, Gen& gen)
  -> reticula::network<typename NetT::EdgeType> {
  return occupy_edges(
    g, [&](const typename NetT::EdgeType&) { return p; }, gen);
}
} // namespace reticula
