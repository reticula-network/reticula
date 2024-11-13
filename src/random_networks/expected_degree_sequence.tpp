#include <random>

#include "../../include/reticula/network_concepts.hpp"
#include "../../include/reticula/networks.hpp"

namespace reticula {
  template <
    integer_network_vertex VertT,
    ranges::input_range Range,
    std::uniform_random_bit_generator Gen>
  requires weight_range<Range>
  undirected_network<VertT>
  random_expected_degree_sequence_graph(
      Range&& weight_sequence,
      Gen& generator,
      bool self_loops) {
    std::vector<std::pair<double, VertT>> weight_node_pairs;
    if constexpr (ranges::sized_range<Range>)
      weight_node_pairs.reserve(std::size(weight_sequence));

    for (VertT i{}; auto&& w: weight_sequence)
      weight_node_pairs.emplace_back(static_cast<double>(w), i++);

    ranges::sort(weight_node_pairs, ranges::greater());

    if (weight_node_pairs.empty())
      return undirected_network<VertT>();

    if (weight_node_pairs.front().first == 0.0)
      return undirected_network<VertT>(
          ranges::empty_view<undirected_edge<VertT>>{},
          views::iota(
            VertT{}, static_cast<VertT>(weight_node_pairs.size())));

    double s = std::accumulate(
        weight_node_pairs.begin(),
        weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(s + 4.0*std::sqrt(s)));

    std::size_t last = weight_node_pairs.size();
    if (!self_loops)
      last--;

    for (std::size_t u = 0; u < last; u++) {
      std::size_t v = u;
      if (!self_loops)
        v++;

      auto [wu, i] = weight_node_pairs[u];
      auto [wv, j] = weight_node_pairs[v];

      double p = std::min(wu*wv/s, 1.0);
      while (v < weight_node_pairs.size() && p > 0) {
        if (p != 1.0)
          v += static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));
        if (v < weight_node_pairs.size()) {
          std::tie(wv, j) = weight_node_pairs[v];
          double q = std::min(wu*wv/s, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p)
            edges.emplace_back(i, j);
          p = q;
          v++;
        }
      }
    }

    return undirected_network<VertT>(edges,
        views::iota(
          VertT{}, static_cast<VertT>(weight_node_pairs.size())));
  }

  template <
    integer_network_vertex VertT,
    ranges::input_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires weight_pair_range<PairRange>
  directed_network<VertT> random_directed_expected_degree_sequence_graph(
      PairRange&& in_out_weight_sequence,
      Gen& generator,
      bool self_loops) {
    std::vector<std::pair<double, VertT>>
      in_weight_node_pairs, out_weight_node_pairs;

    if constexpr (ranges::sized_range<PairRange>) {
      in_weight_node_pairs.reserve(ranges::size(in_out_weight_sequence));
      out_weight_node_pairs.reserve(ranges::size(in_out_weight_sequence));
    }

    for (VertT i{}; auto& [w_in, w_out]: in_out_weight_sequence) {
      in_weight_node_pairs.emplace_back(static_cast<double>(w_in), i);
      out_weight_node_pairs.emplace_back(static_cast<double>(w_out), i++);
    }

    ranges::sort(in_weight_node_pairs, ranges::greater());
    ranges::sort(out_weight_node_pairs, ranges::greater());

    std::vector<std::size_t> node_index_in_in_weight_pair(
        in_weight_node_pairs.size(), 0);
    for (std::size_t j = 0; auto& [w, i]: in_weight_node_pairs)
      node_index_in_in_weight_pair[static_cast<std::size_t>(i)] = j++;

    if (in_weight_node_pairs.empty())
      return directed_network<VertT>();

    if (in_weight_node_pairs.front().first == 0.0 &&
        out_weight_node_pairs.front().first == 0.0)
      return directed_network<VertT>(
          ranges::empty_view<directed_edge<VertT>>{},
          views::iota(
            VertT{}, static_cast<VertT>(in_weight_node_pairs.size())));

    double s_in = std::accumulate(
        in_weight_node_pairs.begin(),
        in_weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    double s_out = std::accumulate(
        out_weight_node_pairs.begin(),
        out_weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    if (s_out > s_in*1.05 || s_out < s_in*0.95)
      throw std::invalid_argument(
          "in- and out-weight sequences should have (almost) equal sums");

    std::vector<directed_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(s_in + 4.0*std::sqrt(s_in)));

    for (std::size_t u = 0; u < out_weight_node_pairs.size(); u++) {
      std::size_t v = 0;

      auto [wu, i] = out_weight_node_pairs[u];
      auto [wv, j] = in_weight_node_pairs[v];

      double p = std::min(wu*wv/s_in, 1.0);
      while (v < in_weight_node_pairs.size() && p > 0) {
        if (p != 1.0) {
          auto delta = static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));

          if (!self_loops)
            if (node_index_in_in_weight_pair[
                  static_cast<std::size_t>(i)] >= v &&
                node_index_in_in_weight_pair[
                  static_cast<std::size_t>(i)] <= v+delta)
              delta++;
          v += delta;
        }

        if (v < in_weight_node_pairs.size()) {
          std::tie(wv, j) = in_weight_node_pairs[v];
          double q = std::min(wu*wv/s_in, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p)
            edges.emplace_back(i, j);
          p = q;
          v++;
        }
      }
    }

    return directed_network<VertT>(edges,
        views::iota(
          VertT{}, static_cast<VertT>(in_weight_node_pairs.size())));
  }

  template <
    integer_network_vertex VertT,
    ranges::input_range VertRange,
    ranges::input_range EdgeRange,
    std::uniform_random_bit_generator Gen>
  requires weight_range<VertRange> && weight_range<EdgeRange>
  undirected_hypernetwork<VertT>
  random_expected_degree_sequence_hypergraph(
      VertRange&& vertex_weight_sequence,
      EdgeRange&& edge_weight_sequence,
      Gen& generator) {
    std::vector<std::pair<double, VertT>> weight_node_pairs;
    if constexpr (ranges::sized_range<VertRange>)
      weight_node_pairs.reserve(std::size(vertex_weight_sequence));

    for (VertT i{}; auto&& w: vertex_weight_sequence)
      weight_node_pairs.emplace_back(static_cast<double>(w), i++);

    ranges::sort(weight_node_pairs, ranges::greater());

    double s = std::accumulate(
        weight_node_pairs.begin(),
        weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });


    std::vector<std::pair<double, std::size_t>> weight_edge_pairs;
    if constexpr (ranges::sized_range<EdgeRange>)
      weight_edge_pairs.reserve(std::size(edge_weight_sequence));

    for (std::size_t i{}; auto&& w: edge_weight_sequence)
      weight_edge_pairs.emplace_back(static_cast<double>(w), i++);

    ranges::sort(weight_node_pairs, ranges::greater());

    double s_edge = std::accumulate(
        weight_edge_pairs.begin(),
        weight_edge_pairs.end(),
        0.0, [](double total, std::pair<double, std::size_t> p){
          return total+p.first;
        });

    if (s > s_edge*1.05 || s < s_edge*0.95)
      throw std::invalid_argument(
          "vertex and edge weight sequences should have (almost) equal sums");

    if (weight_node_pairs.empty())
      return undirected_hypernetwork<VertT>();

    if (weight_node_pairs.front().first == 0.0)
      return undirected_hypernetwork<VertT>(
          ranges::empty_view<undirected_hyperedge<VertT>>{},
          views::iota(
            VertT{}, static_cast<VertT>(weight_node_pairs.size())));

    std::vector<undirected_hyperedge<VertT>> edges;
    edges.reserve(weight_edge_pairs.size());

    for (auto [wu, i] : weight_edge_pairs) {
      std::size_t v = 0;
      auto [wv, j] = weight_node_pairs[v];

      std::vector<VertT> incidents;
      incidents.reserve(static_cast<std::size_t>(wu + 4.0*std::sqrt(wu)));

      double p = std::min(wu*wv/s, 1.0);
      while (v < weight_node_pairs.size() && p > 0) {
        if (p != 1.0)
          v += static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));
        if (v < weight_node_pairs.size()) {
          std::tie(wv, j) = weight_node_pairs[v];
          double q = std::min(wu*wv/s, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p)
            incidents.push_back(j);
          p = q;
          v++;
        }
      }

      edges.emplace_back(incidents);
    }

    return undirected_hypernetwork<VertT>(edges,
        views::iota(
          VertT{}, static_cast<VertT>(weight_node_pairs.size())));
  }

  template <
    integer_network_vertex VertT,
    ranges::input_range VertPairRange,
    ranges::input_range EdgePairRange,
    std::uniform_random_bit_generator Gen>
  requires weight_pair_range<VertPairRange> &&
    weight_pair_range<EdgePairRange>
  directed_hypernetwork<VertT>
  random_directed_expected_degree_sequence_hypergraph(
      VertPairRange&& vertex_in_out_weight_sequence,
      EdgePairRange&& edge_in_out_weight_sequence,
      Gen& generator) {
    std::vector<std::pair<double, VertT>>
      in_weight_node_pairs, out_weight_node_pairs;

    if constexpr (ranges::sized_range<VertPairRange>) {
      in_weight_node_pairs.reserve(
          ranges::size(vertex_in_out_weight_sequence));
      out_weight_node_pairs.reserve(
          ranges::size(vertex_in_out_weight_sequence));
    }

    for (VertT i{}; auto& [w_in, w_out]: vertex_in_out_weight_sequence) {
      in_weight_node_pairs.emplace_back(static_cast<double>(w_in), i);
      out_weight_node_pairs.emplace_back(static_cast<double>(w_out), i++);
    }

    if (in_weight_node_pairs.empty())
      return directed_hypernetwork<VertT>();

    double s_in = std::accumulate(
        in_weight_node_pairs.begin(),
        in_weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    double s_out = std::accumulate(
        out_weight_node_pairs.begin(),
        out_weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    if (s_out > s_in*1.05 || s_out < s_in*0.95)
      throw std::invalid_argument(
          "vertex and edge in- and out-weight "
          "sequences should have (almost) equal sums");

    ranges::sort(in_weight_node_pairs, ranges::greater());
    ranges::sort(out_weight_node_pairs, ranges::greater());

    std::vector<std::pair<double, std::size_t>>
      in_weight_edge_pairs, out_weight_edge_pairs;

    if constexpr (ranges::sized_range<EdgePairRange>) {
      in_weight_node_pairs.reserve(
          ranges::size(edge_in_out_weight_sequence));
      out_weight_node_pairs.reserve(
          ranges::size(edge_in_out_weight_sequence));
    }

    for (std::size_t i{}; auto& [w_in, w_out]: edge_in_out_weight_sequence) {
      in_weight_edge_pairs.emplace_back(static_cast<double>(w_in), i);
      out_weight_edge_pairs.emplace_back(static_cast<double>(w_out), i++);
    }

    ranges::sort(in_weight_edge_pairs, ranges::greater());
    ranges::sort(out_weight_edge_pairs, ranges::greater());

    double s_edge_in = std::accumulate(
        in_weight_edge_pairs.begin(),
        in_weight_edge_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    double s_edge_out = std::accumulate(
        out_weight_edge_pairs.begin(),
        out_weight_edge_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    if (s_in > s_edge_in*1.05 || s_in < s_edge_in*0.95
        || s_in > s_edge_out*1.05 || s_out < s_edge_out*0.95)
      throw std::invalid_argument(
          "vertex and edge in- and out-weight "
          "sequences should have (almost) equal sums");

    if (in_weight_node_pairs.front().first == 0.0)
      return directed_hypernetwork<VertT>(
          ranges::empty_view<directed_hyperedge<VertT>>{},
          views::iota(
            VertT{}, static_cast<VertT>(in_weight_node_pairs.size())));

    std::vector<std::pair<std::size_t, std::vector<VertT>>> edges_out_inc;
    edges_out_inc.reserve(out_weight_edge_pairs.size());

    for (auto [wu, i] : out_weight_edge_pairs) {
      std::size_t v = 0;
      auto [wv, j] = in_weight_node_pairs[v];

      std::vector<VertT> out_incidents;
      out_incidents.reserve(static_cast<std::size_t>(wu + 4.0*std::sqrt(wu)));

      double p = std::min(wu*wv/s_in, 1.0);
      while (v < in_weight_node_pairs.size() && p > 0) {
        if (p != 1.0) {
          auto delta = static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));
          v += delta;
        }

        if (v < in_weight_node_pairs.size()) {
          std::tie(wv, j) = in_weight_node_pairs[v];
          double q = std::min(wu*wv/s_in, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p)
            out_incidents.emplace_back(j);
          p = q;
          v++;
        }
      }

      edges_out_inc.emplace_back(i, out_incidents);
    }

    std::vector<std::pair<std::size_t, std::vector<VertT>>> edges_in_inc;
    edges_in_inc.reserve(in_weight_edge_pairs.size());

    for (auto [wu, i] : in_weight_edge_pairs) {
      std::size_t v = 0;
      auto [wv, j] = out_weight_node_pairs[v];

      std::vector<VertT> in_incidents;
      in_incidents.reserve(static_cast<std::size_t>(wu + 4.0*std::sqrt(wu)));

      double p = std::min(wu*wv/s_in, 1.0);
      while (v < out_weight_node_pairs.size() && p > 0) {
        if (p != 1.0) {
          auto delta = static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));
          v += delta;
        }

        if (v < in_weight_node_pairs.size()) {
          std::tie(wv, j) = out_weight_node_pairs[v];
          double q = std::min(wu*wv/s_in, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p)
            in_incidents.emplace_back(j);
          p = q;
          v++;
        }
      }

      edges_in_inc.emplace_back(i, in_incidents);
    }

    ranges::sort(edges_in_inc);
    ranges::sort(edges_out_inc);

    std::vector<directed_hyperedge<VertT>> edges;
    edges.reserve(edges_in_inc.size());
    for (std::size_t i = 0; i < edges_in_inc.size(); i++)
      edges.emplace_back(edges_in_inc[i].second, edges_out_inc[i].second);

    return directed_hypernetwork<VertT>(edges,
        views::iota(
          VertT{}, static_cast<VertT>(in_weight_node_pairs.size())));
  }

}  // namespace reticula
