#pragma once

#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {

template <typename Range>
concept weight_range = std::ranges::input_range<Range> &&
                       std::is_arithmetic_v<std::ranges::range_value_t<Range>>;

template <std::uniform_random_bit_generator Gen, weight_range Range>
[[nodiscard]] auto expected_degree_sequence_graph(
  Range&& weight_sequence, Gen& gen, bool self_loops = false)
  -> undirected_network;

template <typename Range>
concept weight_pair_range =
  std::ranges::input_range<Range> &&
  pairlike_of<
    std::ranges::range_value_t<Range>,
    std::tuple_element_t<0, std::ranges::range_value_t<Range>>,
    std::tuple_element_t<0, std::ranges::range_value_t<Range>>> &&
  std::is_arithmetic_v<
    std::tuple_element_t<0, std::ranges::range_value_t<Range>>>;

template <std::uniform_random_bit_generator Gen, weight_pair_range Range>
[[nodiscard]] auto directed_expected_degree_sequence_graph(
  Range&& in_out_weight_sequence, Gen& gen, bool self_loops = false)
  -> directed_network;

template <
  std::uniform_random_bit_generator Gen, weight_range VertexRange,
  weight_range EdgeRange>
[[nodiscard]] auto expected_degree_sequence_hypergraph(
  VertexRange&& vertex_weight_sequence, EdgeRange&& edge_weight_sequence,
  Gen& gen) -> undirected_hypernetwork;

template <
  std::uniform_random_bit_generator Gen, weight_pair_range VertexRange,
  weight_pair_range EdgeRange>
[[nodiscard]] auto directed_expected_degree_sequence_hypergraph(
  VertexRange&& vertex_in_out_weight_sequence,
  EdgeRange&& edge_in_out_weight_sequence, Gen& gen) -> directed_hypernetwork;
} // namespace reticula

namespace reticula {
template <std::uniform_random_bit_generator Gen, weight_range Range>
auto expected_degree_sequence_graph(
  Range&& weight_sequence, Gen& gen, bool self_loops) -> undirected_network {

  std::vector<std::pair<double, VertexType>> weight_node_pairs;
  if constexpr (std::ranges::sized_range<Range>)
    weight_node_pairs.reserve(std::ranges::size(weight_sequence));

  for (VertexType i = 0; auto&& w : weight_sequence) {
    if (static_cast<double>(w) < 0.0)
      throw std::domain_error("weights must be non-negative");
    weight_node_pairs.emplace_back(static_cast<double>(w), i++);
  }

  std::ranges::sort(weight_node_pairs, std::ranges::greater());

  if (weight_node_pairs.empty())
    return {};

  if (weight_node_pairs.front().first == 0.0)
    return undirected_network(
      std::views::empty<undirected_edge>,
      std::views::iota(VertexType{}, weight_node_pairs.size()));

  double s = std::accumulate(
    weight_node_pairs.begin(), weight_node_pairs.end(), 0.0,
    [](double total, const std::pair<double, VertexType>& p) {
      return total + p.first;
    });

  std::vector<undirected_edge> edges;
  edges.reserve(static_cast<std::size_t>(s / 2 + 0.5 * std::sqrt(s)));

  std::size_t last = weight_node_pairs.size();
  if (!self_loops)
    last--;

  for (std::size_t u = 0; u < last; u++) {
    std::size_t v = u;
    if (!self_loops)
      v++;

    auto [wu, i] = weight_node_pairs[u];
    auto [wv, j] = weight_node_pairs[v];

    double p = std::min(wu * wv / s, 1.0);
    while (v < weight_node_pairs.size() && p > 0) {
      if (p != 1.0)
        v += static_cast<std::size_t>(
          std::log(std::uniform_real_distribution{}(gen)) / std::log1p(-p));
      if (v < weight_node_pairs.size()) {
        std::tie(wv, j) = weight_node_pairs[v];
        double q = std::min(wu * wv / s, 1.0);
        if (!self_loops && j == i) {
          p = q;
          ++v;
          continue;
        }
        if (std::uniform_real_distribution{}(gen) < q / p)
          edges.emplace_back(i, j);
        p = q;
        v++;
      }
    }
  }

  return undirected_network(
    edges, std::views::iota(VertexType{}, weight_node_pairs.size()));
}

template <std::uniform_random_bit_generator Gen, weight_pair_range Range>
auto directed_expected_degree_sequence_graph(
  Range&& in_out_weight_sequence, Gen& gen, bool self_loops)
  -> directed_network {
  std::vector<std::pair<double, VertexType>> in_weight_node_pairs,
    out_weight_node_pairs;
  if constexpr (std::ranges::sized_range<Range>) {
    in_weight_node_pairs.reserve(std::ranges::size(in_out_weight_sequence));
    out_weight_node_pairs.reserve(std::ranges::size(in_out_weight_sequence));
  }

  for (VertexType i = 0; auto& [w_in, w_out] : in_out_weight_sequence) {
    if (static_cast<double>(w_in) < 0.0 || static_cast<double>(w_out) < 0.0)
      throw std::domain_error("weights must be non-negative");
    in_weight_node_pairs.emplace_back(static_cast<double>(w_in), i);
    out_weight_node_pairs.emplace_back(static_cast<double>(w_out), i++);
  }

  std::ranges::sort(in_weight_node_pairs, std::ranges::greater());
  std::ranges::sort(out_weight_node_pairs, std::ranges::greater());

  std::vector<std::size_t> node_index_in_in_weight_pair(
    in_weight_node_pairs.size(), 0);
  for (std::size_t j = 0; auto& [w, i] : in_weight_node_pairs)
    node_index_in_in_weight_pair[static_cast<std::size_t>(i)] = j++;

  if (in_weight_node_pairs.empty())
    return {};

  if (
    in_weight_node_pairs.front().first == 0.0 &&
    out_weight_node_pairs.front().first == 0.0)
    return directed_network(
      std::views::empty<directed_edge>,
      std::views::iota(VertexType{}, in_weight_node_pairs.size()));

  double s_in = std::accumulate(
    in_weight_node_pairs.begin(), in_weight_node_pairs.end(), 0.0,
    [](double total, const std::pair<double, VertexType>& p) {
      return total + p.first;
    });

  double s_out = std::accumulate(
    out_weight_node_pairs.begin(), out_weight_node_pairs.end(), 0.0,
    [](double total, const std::pair<double, VertexType>& p) {
      return total + p.first;
    });

  if (s_out > s_in * 1.05 || s_out < s_in * 0.95)
    throw std::invalid_argument(
      "in- and out-weight sequences should have (almost) equal sums");

  std::vector<directed_edge> edges;
  edges.reserve(static_cast<std::size_t>(s_in + 4.0 * std::sqrt(s_in)));

  for (auto [wu, i] : out_weight_node_pairs) {
    std::size_t v = 0;

    auto [wv, j] = in_weight_node_pairs[v];

    double p = std::min(wu * wv / s_in, 1.0);
    while (v < in_weight_node_pairs.size() && p > 0) {
      if (p != 1.0) {
        auto delta = static_cast<std::size_t>(
          std::log(std::uniform_real_distribution{}(gen)) / std::log1p(-p));

        if (!self_loops)
          if (
            node_index_in_in_weight_pair[static_cast<std::size_t>(i)] >= v &&
            node_index_in_in_weight_pair[static_cast<std::size_t>(i)] <=
              v + delta)
            delta++;
        v += delta;
      }

      if (v < in_weight_node_pairs.size()) {
        std::tie(wv, j) = in_weight_node_pairs[v];
        double q = std::min(wu * wv / s_in, 1.0);
        if (!self_loops && j == i) {
          p = q;
          ++v;
          continue;
        }
        if (std::uniform_real_distribution{}(gen) < q / p)
          edges.emplace_back(i, j);
        p = q;
        v++;
      }
    }
  }

  return directed_network(
    edges, std::views::iota(VertexType{}, in_weight_node_pairs.size()));
}

template <
  std::uniform_random_bit_generator Gen, weight_range VertexRange,
  weight_range EdgeRange>
auto expected_degree_sequence_hypergraph(
  VertexRange&& vertex_weight_sequence, EdgeRange&& edge_weight_sequence,
  Gen& gen) -> undirected_hypernetwork {
  std::vector<std::pair<double, VertexType>> weight_node_pairs;
  if constexpr (std::ranges::sized_range<VertexRange>)
    weight_node_pairs.reserve(std::size(vertex_weight_sequence));

  for (VertexType i{}; auto&& w : vertex_weight_sequence) {
    if (static_cast<double>(w) < 0.0)
      throw std::domain_error("weights must be non-negative");
    weight_node_pairs.emplace_back(static_cast<double>(w), i++);
  }

  std::ranges::sort(weight_node_pairs, std::ranges::greater());

  double s = std::accumulate(
    weight_node_pairs.begin(), weight_node_pairs.end(), 0.0,
    [](double total, const std::pair<double, VertexType>& p) {
      return total + p.first;
    });

  std::vector<std::pair<double, std::size_t>> weight_edge_pairs;
  if constexpr (std::ranges::sized_range<EdgeRange>)
    weight_edge_pairs.reserve(std::size(edge_weight_sequence));

  for (std::size_t i{}; auto&& w : edge_weight_sequence) {
    if (static_cast<double>(w) < 0.0)
      throw std::domain_error("weights must be non-negative");
    weight_edge_pairs.emplace_back(static_cast<double>(w), i++);
  }

  std::ranges::sort(weight_edge_pairs, std::ranges::greater());

  double s_edge = std::accumulate(
    weight_edge_pairs.begin(), weight_edge_pairs.end(), 0.0,
    [](double total, const std::pair<double, std::size_t>& p) {
      return total + p.first;
    });

  if (s > s_edge * 1.05 || s < s_edge * 0.95)
    throw std::invalid_argument(
      "vertex and edge weight sequences should have (almost) equal sums");

  if (weight_node_pairs.empty())
    return {};

  if (weight_node_pairs.front().first == 0.0)
    return undirected_hypernetwork(
      std::views::empty<undirected_hyperedge>,
      std::views::iota(VertexType{}, weight_node_pairs.size()));

  std::vector<undirected_hyperedge> edges;
  edges.reserve(weight_edge_pairs.size());

  std::uniform_real_distribution uniform;

  for (auto [wu, i] : weight_edge_pairs) {
    std::size_t v = 0;
    auto [wv, j] = weight_node_pairs[v];

    std::vector<VertexType> incidents;
    incidents.reserve(static_cast<std::size_t>(wu + 4.0 * std::sqrt(wu)));

    double p = std::min(wu * wv / s, 1.0);
    while (v < weight_node_pairs.size() && p > 0) {
      if (p != 1.0)
        v += static_cast<std::size_t>(std::log(uniform(gen)) / std::log1p(-p));
      if (v < weight_node_pairs.size()) {
        std::tie(wv, j) = weight_node_pairs[v];
        double q = std::min(wu * wv / s, 1.0);
        if (uniform(gen) < q / p)
          incidents.push_back(j);
        p = q;
        v++;
      }
    }

    edges.emplace_back(incidents);
  }

  return undirected_hypernetwork(
    edges, std::views::iota(VertexType{}, weight_node_pairs.size()));
}

template <
  std::uniform_random_bit_generator Gen, weight_pair_range VertexRange,
  weight_pair_range EdgeRange>
auto directed_expected_degree_sequence_hypergraph(
  VertexRange&& vertex_in_out_weight_sequence,
  EdgeRange&& edge_in_out_weight_sequence, Gen& gen) -> directed_hypernetwork {

  std::vector<std::pair<double, VertexType>> in_weight_node_pairs,
    out_weight_node_pairs;

  if constexpr (std::ranges::sized_range<VertexRange>) {
    in_weight_node_pairs.reserve(
      std::ranges::size(vertex_in_out_weight_sequence));
    out_weight_node_pairs.reserve(
      std::ranges::size(vertex_in_out_weight_sequence));
  }

  for (VertexType i{}; auto& [w_in, w_out] : vertex_in_out_weight_sequence) {
    if (static_cast<double>(w_in) < 0.0 || static_cast<double>(w_out) < 0.0)
      throw std::domain_error("weights must be non-negative");
    in_weight_node_pairs.emplace_back(static_cast<double>(w_in), i);
    out_weight_node_pairs.emplace_back(static_cast<double>(w_out), i++);
  }

  if (in_weight_node_pairs.empty())
    return {};

  double s_in = std::accumulate(
    in_weight_node_pairs.begin(), in_weight_node_pairs.end(), 0.0,
    [](double total, const std::pair<double, VertexType>& p) {
      return total + p.first;
    });

  double s_out = std::accumulate(
    out_weight_node_pairs.begin(), out_weight_node_pairs.end(), 0.0,
    [](double total, const std::pair<double, VertexType>& p) {
      return total + p.first;
    });

  if (s_out > s_in * 1.05 || s_out < s_in * 0.95)
    throw std::invalid_argument(
      "vertex and edge in- and out-weight "
      "sequences should have (almost) equal sums");

  std::ranges::sort(in_weight_node_pairs, std::ranges::greater());
  std::ranges::sort(out_weight_node_pairs, std::ranges::greater());

  std::vector<std::pair<double, std::size_t>> in_weight_edge_pairs,
    out_weight_edge_pairs;

  if constexpr (std::ranges::sized_range<EdgeRange>) {
    in_weight_edge_pairs.reserve(
      std::ranges::size(edge_in_out_weight_sequence));
    out_weight_edge_pairs.reserve(
      std::ranges::size(edge_in_out_weight_sequence));
  }

  for (std::size_t i{}; auto& [w_in, w_out] : edge_in_out_weight_sequence) {
    if (static_cast<double>(w_in) < 0.0 || static_cast<double>(w_out) < 0.0)
      throw std::domain_error("weights must be non-negative");
    in_weight_edge_pairs.emplace_back(static_cast<double>(w_in), i);
    out_weight_edge_pairs.emplace_back(static_cast<double>(w_out), i++);
  }

  std::ranges::sort(in_weight_edge_pairs, std::ranges::greater());
  std::ranges::sort(out_weight_edge_pairs, std::ranges::greater());

  double s_edge_in = std::accumulate(
    in_weight_edge_pairs.begin(), in_weight_edge_pairs.end(), 0.0,
    [](double total, const std::pair<double, VertexType>& p) {
      return total + p.first;
    });

  double s_edge_out = std::accumulate(
    out_weight_edge_pairs.begin(), out_weight_edge_pairs.end(), 0.0,
    [](double total, const std::pair<double, VertexType>& p) {
      return total + p.first;
    });

  if (
    s_in > s_edge_in * 1.05 || s_in < s_edge_in * 0.95 ||
    s_in > s_edge_out * 1.05 || s_out < s_edge_out * 0.95)
    throw std::invalid_argument(
      "vertex and edge in- and out-weight "
      "sequences should have (almost) equal sums");

  if (in_weight_node_pairs.front().first == 0.0)
    return directed_hypernetwork(
      std::views::empty<directed_hyperedge>,
      std::views::iota(VertexType{}, in_weight_node_pairs.size()));

  std::vector<std::pair<std::size_t, std::vector<VertexType>>> edges_out_inc;
  edges_out_inc.reserve(out_weight_edge_pairs.size());

  std::uniform_real_distribution uniform;

  for (auto [wu, i] : out_weight_edge_pairs) {
    std::size_t v = 0;
    auto [wv, j] = in_weight_node_pairs[v];

    std::vector<VertexType> out_incidents;
    out_incidents.reserve(static_cast<std::size_t>(wu + 4.0 * std::sqrt(wu)));

    double p = std::min(wu * wv / s_in, 1.0);
    while (v < in_weight_node_pairs.size() && p > 0) {
      if (p != 1.0) {
        auto delta =
          static_cast<std::size_t>(std::log(uniform(gen)) / std::log1p(-p));
        v += delta;
      }

      if (v < in_weight_node_pairs.size()) {
        std::tie(wv, j) = in_weight_node_pairs[v];
        double q = std::min(wu * wv / s_in, 1.0);
        if (uniform(gen) < q / p)
          out_incidents.emplace_back(j);
        p = q;
        v++;
      }
    }

    edges_out_inc.emplace_back(i, out_incidents);
  }

  std::vector<std::pair<std::size_t, std::vector<VertexType>>> edges_in_inc;
  edges_in_inc.reserve(in_weight_edge_pairs.size());

  for (auto [wu, i] : in_weight_edge_pairs) {
    std::size_t v = 0;
    auto [wv, j] = out_weight_node_pairs[v];

    std::vector<VertexType> in_incidents;
    in_incidents.reserve(static_cast<std::size_t>(wu + 4.0 * std::sqrt(wu)));

    double p = std::min(wu * wv / s_out, 1.0);
    while (v < out_weight_node_pairs.size() && p > 0) {
      if (p != 1.0) {
        auto delta =
          static_cast<std::size_t>(std::log(uniform(gen)) / std::log1p(-p));
        v += delta;
      }

      if (v < out_weight_node_pairs.size()) {
        std::tie(wv, j) = out_weight_node_pairs[v];
        double q = std::min(wu * wv / s_out, 1.0);
        if (uniform(gen) < q / p)
          in_incidents.emplace_back(j);
        p = q;
        v++;
      }
    }

    edges_in_inc.emplace_back(i, in_incidents);
  }

  std::ranges::sort(edges_in_inc);
  std::ranges::sort(edges_out_inc);

  std::vector<directed_hyperedge> edges;
  edges.reserve(edges_in_inc.size());
  for (std::size_t i = 0; i < edges_in_inc.size(); i++)
    edges.emplace_back(edges_in_inc[i].second, edges_out_inc[i].second);

  return directed_hypernetwork(
    edges, std::views::iota(VertexType{}, in_weight_node_pairs.size()));
}
} // namespace reticula
