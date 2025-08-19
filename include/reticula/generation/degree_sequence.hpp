#pragma once

#include <optional>
#include <random>
#include <ranges>
#include <unordered_set>
#include <utility>
#include <vector>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen, degree_range Range>
[[nodiscard]] auto degree_sequence_graph(Range&& degree_sequence, Gen& gen)
  -> undirected_network;

template <std::uniform_random_bit_generator Gen, degree_range Range>
[[nodiscard]] auto try_degree_sequence_graph(
  Range&& degree_sequence, Gen& gen, std::size_t max_tries = 1000)
  -> std::optional<undirected_network>;

template <std::uniform_random_bit_generator Gen, degree_pair_range Range>
[[nodiscard]] auto
directed_degree_sequence_graph(Range&& degree_sequence, Gen& gen)
  -> directed_network;

template <std::uniform_random_bit_generator Gen, degree_pair_range Range>
[[nodiscard]] auto try_directed_degree_sequence_graph(
  Range&& in_out_degree_sequence, Gen& gen, std::size_t max_tries = 1000)
  -> std::optional<directed_network>;
} // namespace reticula

#include <reticula/algorithms/graphicallity.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen, degree_range Range>
auto degree_sequence_graph(Range&& degree_sequence, Gen& gen)
  -> undirected_network {
  std::optional<undirected_network> maybe_g = std::nullopt;
  while (!maybe_g)
    maybe_g = try_degree_sequence_graph(
      std::forward<Range>(degree_sequence), gen, 1000);
  return *maybe_g;
}

template <std::uniform_random_bit_generator Gen, degree_range Range>
auto try_degree_sequence_graph(
  Range&& degree_sequence, Gen& gen, std::size_t max_tries)
  -> std::optional<undirected_network> {

  std::vector<VertexType> degrees;
  if constexpr (std::ranges::sized_range<Range>)
    degrees.reserve(std::size(degree_sequence));

  for (auto d : degree_sequence)
    degrees.push_back(d);

  if (degrees.empty())
    return undirected_network{};

  if (!is_graphic(degrees))
    throw std::invalid_argument("degree_sequence is not graphical");

  VertexType degree_sum =
    std::accumulate(degrees.begin(), degrees.end(), VertexType{});

  for (std::size_t t = 0; t < max_tries; t++) {
    std::unordered_set<undirected_edge> edges;
    edges.reserve(degree_sum / 2);

    std::vector<VertexType> stubs = degrees;
    std::vector<VertexType> repeated_stubs;
    repeated_stubs.reserve(degree_sum);

    // Initialize repeated_stubs
    for (VertexType i = 0; i < degrees.size(); i++)
      for (VertexType j = 0; j < degrees[i]; j++)
        repeated_stubs.push_back(i);

    auto has_possible_edge = [&edges](const std::vector<VertexType>& stubs) {
      std::size_t size = stubs.size();

      if (size < 2)
        return false;

      for (std::size_t i = 0; i < size; i++)
        for (std::size_t j = i + 1; j < size; j++)
          if (stubs[i] != stubs[j] && !edges.contains({stubs[i], stubs[j]}))
            return true;

      return false;
    };

    auto suitable_edges_remaining = has_possible_edge(repeated_stubs);
    while (suitable_edges_remaining) {
      std::size_t current_size = repeated_stubs.size();
      std::uniform_int_distribution<std::size_t> dist_u{0, current_size - 1};
      std::size_t idx_u = dist_u(gen);
      VertexType u = repeated_stubs[idx_u];
      std::swap(repeated_stubs[idx_u], repeated_stubs[current_size - 1]);

      std::uniform_int_distribution<std::size_t> dist_v{0, current_size - 2};
      std::size_t idx_v = dist_v(gen);
      while (repeated_stubs[idx_v] == u)
        idx_v = dist_v(gen);
      VertexType v = repeated_stubs[idx_v];
      std::swap(repeated_stubs[idx_v], repeated_stubs[current_size - 2]);

      double puv = static_cast<double>(degrees[u] * degrees[v]) /
                   (2.0 * static_cast<double>(degree_sum));
      undirected_edge new_edge{u, v};

      // Bayati et al. 2009 suggests that the probability of accepting u, v is
      // proportional to remaining degrees of u and v and (1-d[u]d[v]/4m). We
      // have chosen u and v at random with weights proportional to remaining
      // degree, to get the complement out of the way we accept if x > puv where
      // x is randomly drawn in [0, 1).
      if (
        !edges.contains(new_edge) &&
        std::uniform_real_distribution{}(gen) > puv) {
        edges.insert(new_edge);

        repeated_stubs.pop_back();
        repeated_stubs.pop_back();

        stubs[u]--;
        stubs[v]--;

        if (stubs[u] == 0 || stubs[v] == 0)
          suitable_edges_remaining = has_possible_edge(repeated_stubs);
      }
    }

    if (repeated_stubs.empty())
      return undirected_network(
        edges, std::views::iota(VertexType{}, degrees.size()));
  }
  return std::nullopt;
}

template <std::uniform_random_bit_generator Gen, degree_pair_range Range>
auto directed_degree_sequence_graph(Range&& degree_sequence, Gen& gen)
  -> directed_network {
  std::optional<directed_network> maybe_g = std::nullopt;
  while (!maybe_g)
    maybe_g = try_directed_degree_sequence_graph(
      std::forward<Range>(degree_sequence), gen, 1000);
  return *maybe_g;
}

template <std::uniform_random_bit_generator Gen, degree_pair_range Range>
auto try_directed_degree_sequence_graph(
  Range&& in_out_degree_sequence, Gen& gen, std::size_t max_tries)
  -> std::optional<directed_network> {
  std::vector<VertexType> in_degrees, out_degrees;
  if constexpr (std::ranges::sized_range<Range>) {
    in_degrees.reserve(std::ranges::size(in_out_degree_sequence));
    out_degrees.reserve(std::ranges::size(in_out_degree_sequence));
  }

  for (auto [in, out] : in_out_degree_sequence) {
    in_degrees.push_back(in);
    out_degrees.push_back(out);
  }

  if (in_degrees.empty())
    return directed_network{};

  if (!is_digraphic(std::views::zip(in_degrees, out_degrees)))
    throw std::invalid_argument("degree_sequence is not digraphical");

  VertexType degree_sum =
    std::accumulate(in_degrees.begin(), in_degrees.end(), VertexType{});

  if (
    degree_sum !=
    std::accumulate(out_degrees.begin(), out_degrees.end(), VertexType{}))
    throw std::invalid_argument(
      "in- and out-degree sequences should have equal sums");

  for (std::size_t t = 0; t < max_tries; t++) {
    std::unordered_set<directed_edge, hash<directed_edge>> edges;
    edges.reserve(degree_sum);

    std::vector<VertexType> in_stubs = in_degrees;
    std::vector<VertexType> out_stubs = out_degrees;

    std::vector<VertexType> in_repeated_stubs;
    in_repeated_stubs.reserve(degree_sum);
    std::vector<VertexType> out_repeated_stubs;
    out_repeated_stubs.reserve(degree_sum);

    for (VertexType i = 0; i < in_degrees.size(); i++) {
      for (VertexType j = 0; j < in_degrees[i]; j++)
        in_repeated_stubs.push_back(i);
      for (VertexType j = 0; j < out_degrees[i]; j++)
        out_repeated_stubs.push_back(i);
    }

    auto has_possible_directed_edge =
      [&edges](
        const std::vector<VertexType>& in_stubs,
        const std::vector<VertexType>& out_stubs) {
        if (out_stubs.empty() || in_stubs.empty())
          return false;

        for (const auto& u : out_stubs)
          for (const auto& v : in_stubs)
            if (u != v && !edges.contains({u, v}))
              return true;
        return false;
      };

    bool suitable_edges_remaining =
      has_possible_directed_edge(in_repeated_stubs, out_repeated_stubs);
    while (suitable_edges_remaining) {
      std::size_t out_size = out_repeated_stubs.size();
      std::uniform_int_distribution<std::size_t> dist{0, out_size - 1};

      std::size_t idx_u = dist(gen);
      std::size_t idx_v = dist(gen);
      while (out_repeated_stubs[idx_u] == in_repeated_stubs[idx_v]) {
        idx_u = dist(gen);
        idx_v = dist(gen);
      }

      VertexType u = out_repeated_stubs[idx_u];
      std::swap(out_repeated_stubs[idx_u], out_repeated_stubs[out_size - 1]);
      VertexType v = in_repeated_stubs[idx_v];
      std::swap(in_repeated_stubs[idx_v], in_repeated_stubs[out_size - 1]);

      double puv = static_cast<double>(in_degrees[v] * out_degrees[u]) /
                   (2.0 * static_cast<double>(degree_sum));
      directed_edge new_edge{u, v};

      // Bayati et al. 2009 suggests that the probability of accepting u, v is
      // proportional to remaining degrees of u and v and (1-d[u]d[v]/4m). We
      // have chosen u and v at random with weights proportional to remaining
      // degree, to get the complement out of the way we accept if x > puv
      // where x is randomly drawn in [0, 1).
      if (
        !edges.contains(new_edge) &&
        std::uniform_real_distribution{}(gen) > puv) {
        edges.insert(new_edge);

        out_repeated_stubs.pop_back();
        in_repeated_stubs.pop_back();

        out_stubs[u]--;
        in_stubs[v]--;

        if (out_stubs[u] == 0 || in_stubs[v] == 0)
          suitable_edges_remaining =
            has_possible_directed_edge(in_repeated_stubs, out_repeated_stubs);
      }
    }

    if (in_repeated_stubs.empty() && out_repeated_stubs.empty())
      return directed_network(
        edges, std::views::iota(VertexType{}, in_degrees.size()));
  }
  return std::nullopt;
}
} // namespace reticula
