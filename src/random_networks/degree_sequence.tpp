#include "../include/reticula/algorithms.hpp"

namespace reticula {
  template <
    integer_network_vertex VertT,
    ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    degree_range<Range> &&
    std::convertible_to<ranges::range_value_t<Range>, VertT>
  undirected_network<VertT>
  random_degree_sequence_graph(
      Range&& degree_sequence,
      Gen& gen) {
    std::optional<undirected_network<VertT>> maybe_g = std::nullopt;
    while (!maybe_g) maybe_g = try_random_degree_sequence_graph<VertT>(
        degree_sequence, gen, 1000);
    return *maybe_g;
  }

  template <
    integer_network_vertex VertT,
    ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    degree_range<Range> &&
    std::convertible_to<ranges::range_value_t<Range>, VertT>
  std::optional<undirected_network<VertT>>
  try_random_degree_sequence_graph(
      Range&& degree_sequence,
      Gen& gen,
      std::size_t max_tries) {
    if (!is_graphic(degree_sequence))
      throw std::invalid_argument("degree_sequence is not graphical");

    std::vector<VertT> degrees;
    if constexpr (ranges::sized_range<Range>)
      degrees.reserve(std::size(degree_sequence));

    for (auto d: degree_sequence)
      degrees.push_back(d);

    if (degrees.empty())
      return undirected_network<VertT>();

    VertT degree_sum = std::accumulate(degrees.begin(), degrees.end(), VertT{});

    auto has_possible_edge = [](
      const std::unordered_set<
        undirected_edge<VertT>,
        hash<undirected_edge<VertT>>>& edges,
      auto nodes_begin, auto nodes_end) {
        std::size_t size = static_cast<std::size_t>(
            std::distance(nodes_begin, nodes_end));
        if (size == 0) {
          return false;
        } else {
          for (std::ptrdiff_t i = 0; i < static_cast<std::ptrdiff_t>(size); i++)
            for (std::ptrdiff_t j = 0; j < i; j++)
              if (!edges.contains({
                    static_cast<VertT>(*(nodes_begin+i)),
                    static_cast<VertT>(*(nodes_begin+j))}))
                return true;
        }

        return false;
    };

    for (std::size_t t = 0; t < max_tries; t++) {
      std::unordered_set<
        undirected_edge<VertT>,
        hash<undirected_edge<VertT>>> edges;
      edges.reserve(static_cast<std::size_t>(degree_sum)/2);

      VertT stubs_sum = degree_sum;

      std::vector<VertT> nodes;
      nodes.reserve(degrees.size());
      std::vector<VertT> stubs;
      stubs.reserve(degrees.size());

      for (VertT i = 0; i < static_cast<VertT>(degrees.size()); i++) {
        if (degrees[static_cast<std::size_t>(i)] > 0) {
          nodes.push_back(i);
          stubs.push_back(degrees[static_cast<std::size_t>(i)]);
        }
      }

      auto stubs_end = stubs.end();
      auto nodes_end = nodes.end();

      std::discrete_distribution<std::size_t> d(stubs.begin(), stubs_end);
      bool suitable_edges_remaining = has_possible_edge(edges,
            nodes.begin(), nodes_end);
      while (suitable_edges_remaining) {
        std::size_t i = d(gen), j = d(gen);
        while (i == j) j = d(gen);

        VertT u = nodes[i], v = nodes[j];

        double puv =
          static_cast<double>(
              degrees[static_cast<std::size_t>(u)]*
              degrees[static_cast<std::size_t>(v)])/
            (2.0*static_cast<double>(degree_sum));
        if (undirected_edge<VertT> new_edge(u, v);
            !edges.contains(new_edge) &&
              std::uniform_real_distribution{}(gen) > puv) {
          edges.insert(new_edge);

          bool node_deleted = false;

          stubs[i]--;
          if (stubs[i] == 0) {
            node_deleted = true;

            stubs[i] = *(stubs_end-1);
            stubs_end--;
            nodes[i] = *(nodes_end-1);
            nodes_end--;

            std::ptrdiff_t previous_last_index = std::distance(
                nodes.begin(), nodes_end);
            if (j == static_cast<std::size_t>(previous_last_index))
              j = i;
          }

          stubs[j]--;
          if (stubs[j] == 0) {
            node_deleted = true;

            stubs[j] = *(stubs_end-1);
            stubs_end--;
            nodes[j] = *(nodes_end-1);
            nodes_end--;
          }
          stubs_sum -= 2;

          if (node_deleted)
            suitable_edges_remaining = has_possible_edge(
                  edges, nodes.begin(), nodes_end);

          d = std::discrete_distribution<std::size_t>(stubs.begin(), stubs_end);
        }
      }

      if (stubs_sum == 0)
        return undirected_network<VertT>(edges,
            views::iota(
              VertT{}, static_cast<VertT>(degrees.size())));
    }
    return std::nullopt;
  }


  template <
    integer_network_vertex VertT,
    ranges::forward_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires
    degree_pair_range<PairRange> &&
    is_pairlike_of<ranges::range_value_t<PairRange>, VertT, VertT>
  directed_network<VertT>
  random_directed_degree_sequence_graph(
      PairRange&& in_out_degree_sequence,
      Gen& gen) {
    std::optional<directed_network<VertT>> maybe_g = std::nullopt;
    while (!maybe_g) maybe_g = try_random_directed_degree_sequence_graph<VertT>(
        in_out_degree_sequence, gen, 1000);
    return *maybe_g;
  }

  template <
    integer_network_vertex VertT,
    ranges::forward_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires
    degree_pair_range<PairRange> &&
    is_pairlike_of<ranges::range_value_t<PairRange>, VertT, VertT>
  std::optional<directed_network<VertT>>
  try_random_directed_degree_sequence_graph(
      PairRange&& in_out_degree_sequence,
      Gen& gen,
      std::size_t max_tries) {
    // TODO: pass a lazy zip_view of in_degrees and out_degrees instead of
    // reading the sequence twice. This way we can change the forward_range to
    // an input_range.
    if (!is_digraphic(in_out_degree_sequence))
      throw std::invalid_argument("degree_sequence is not digraphical");

    std::vector<VertT> in_degrees, out_degrees;
    if constexpr (ranges::sized_range<PairRange>) {
      in_degrees.reserve(ranges::size(in_out_degree_sequence));
      out_degrees.reserve(ranges::size(in_out_degree_sequence));
    }

    for (auto [in, out]: in_out_degree_sequence) {
      in_degrees.push_back(in);
      out_degrees.push_back(out);
    }

    if (in_degrees.empty())
      return directed_network<VertT>();

    VertT in_degree_sum = std::accumulate(
        in_degrees.begin(), in_degrees.end(), VertT{});

    VertT out_degree_sum = std::accumulate(
        out_degrees.begin(), out_degrees.end(), VertT{});

    if (in_degree_sum != out_degree_sum)
      throw std::invalid_argument(
          "in- and out-degree sequences should have equal sums");

    auto has_possible_directed_edge = [](
        const std::unordered_set<
          directed_edge<VertT>,
          hash<directed_edge<VertT>>>& edges,
        auto in_nodes_begin, auto in_nodes_end,
        auto out_nodes_begin, auto out_nodes_end) {
      if (in_nodes_begin == in_nodes_end ||
          out_nodes_begin == out_nodes_end) {
        return false;
      } else {
        for (auto out_it = out_nodes_begin; out_it < out_nodes_end; out_it++)
          for (auto in_it = in_nodes_begin; in_it < in_nodes_end; in_it++)
            if (*out_it != *in_it && !edges.contains({
                  static_cast<VertT>(*out_it),
                  static_cast<VertT>(*in_it)}))
              return true;
      }
      return false;
    };

    for (std::size_t t = 0; t < max_tries; t++) {
      std::unordered_set<
        directed_edge<VertT>,
        hash<directed_edge<VertT>>> edges;
      edges.reserve(static_cast<std::size_t>(in_degree_sum));

      VertT in_stubs_sum = in_degree_sum;

      std::vector<VertT> in_nodes, in_stubs;
      in_nodes.reserve(in_degrees.size());
      in_stubs.reserve(in_degrees.size());

      std::vector<VertT> out_nodes, out_stubs;
      out_nodes.reserve(in_degrees.size());
      out_stubs.reserve(in_degrees.size());

      for (VertT i = 0; i < static_cast<VertT>(in_degrees.size()); i++) {
        if (in_degrees[static_cast<std::size_t>(i)] > 0) {
          in_nodes.push_back(i);
          in_stubs.push_back(in_degrees[static_cast<std::size_t>(i)]);
        }
        if (out_degrees[static_cast<std::size_t>(i)] > 0) {
          out_nodes.push_back(i);
          out_stubs.push_back(out_degrees[static_cast<std::size_t>(i)]);
        }
      }

      auto in_nodes_end = in_nodes.end();
      auto in_stubs_end = in_stubs.end();

      auto out_nodes_end = out_nodes.end();
      auto out_stubs_end = out_stubs.end();

      std::discrete_distribution<std::size_t> in_d(
          in_stubs.begin(), in_stubs_end);
      std::discrete_distribution<std::size_t> out_d(
          out_stubs.begin(), out_stubs_end);
      bool suitable_edges_remaining = has_possible_directed_edge(edges,
            in_nodes.begin(), in_nodes_end,
            out_nodes.begin(), out_nodes_end);
      while (suitable_edges_remaining) {
        std::size_t i = in_d(gen), j = out_d(gen);
        while (in_nodes[i] == out_nodes[j]) j = out_d(gen);

        VertT u = in_nodes[i], v = out_nodes[j];

        double puv =
          static_cast<double>(
              in_degrees[static_cast<std::size_t>(u)]*
              out_degrees[static_cast<std::size_t>(v)])/
            (2.0*static_cast<double>(in_degree_sum));
        if (directed_edge<VertT> new_edge(
              static_cast<VertT>(u), static_cast<VertT>(v));
            !edges.contains(new_edge) &&
              std::uniform_real_distribution{}(gen) > puv) {
          edges.insert(new_edge);

          bool node_deleted = false;

          in_stubs[i]--;
          in_stubs_sum--;
          if (in_stubs[i] == 0) {
            node_deleted = true;

            in_stubs[i] = *(in_stubs_end-1);
            in_stubs_end--;
            in_nodes[i] = *(in_nodes_end-1);
            in_nodes_end--;
          }

          out_stubs[j]--;
          if (out_stubs[j] == 0) {
            node_deleted = true;

            out_stubs[j] = *(out_stubs_end-1);
            out_stubs_end--;
            out_nodes[j] = *(out_nodes_end-1);
            out_nodes_end--;
          }

          if (node_deleted)
            suitable_edges_remaining = has_possible_directed_edge(
                  edges,
                  in_nodes.begin(), in_nodes_end,
                  out_nodes.begin(), out_nodes_end);

          in_d = std::discrete_distribution<std::size_t>(
              in_stubs.begin(), in_stubs_end);
          out_d = std::discrete_distribution<std::size_t>(
              out_stubs.begin(), out_stubs_end);
        }
      }

      if (in_stubs_sum == 0)
        return directed_network<VertT>(edges,
            views::iota(
              VertT{}, static_cast<VertT>(in_degrees.size())));
    }
    return std::nullopt;
  }

}  // namespace reticula
