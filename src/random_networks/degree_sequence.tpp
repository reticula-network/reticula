#include "../include/reticula/algorithms.hpp"
#include <cstddef>
#include <utility>

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
    constexpr std::size_t tries = 1000;
    while (!maybe_g) maybe_g = try_random_degree_sequence_graph<VertT>(
        std::forward<Range>(degree_sequence), gen, tries);
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

    for (std::size_t t = 0; t < max_tries; t++) {
      std::unordered_set<
        undirected_edge<VertT>,
        hash<undirected_edge<VertT>>> edges;
      edges.reserve(static_cast<std::size_t>(degree_sum)/2);

      std::vector<VertT> stubs = degrees;
      std::vector<VertT> repeated_stubs;
      repeated_stubs.reserve(static_cast<std::size_t>(degree_sum));

      // Initialize repeated_stubs
      for (VertT i = 0; i < static_cast<VertT>(degrees.size()); i++)
        for (VertT j = 0; j < degrees[static_cast<std::size_t>(i)]; j++)
          repeated_stubs.push_back(i);

      auto has_possible_edge = [&edges](const std::vector<VertT>& stubs) {
        std::size_t size = stubs.size();

        if (size < 2)
          return false;

        for (std::size_t i = 0; i < size; i++)
          for (std::size_t j = i + 1; j < size; j++)
            if (stubs[i] != stubs[j] &&
                !edges.contains({stubs[i], stubs[j]}))
              return true;

        return false;
      };

      auto suitable_edges_remaining = has_possible_edge(repeated_stubs);
      while (suitable_edges_remaining) {
        std::size_t current_size = repeated_stubs.size();
        std::uniform_int_distribution<std::size_t> dist_u{0, current_size - 1};
        std::size_t idx_u = dist_u(gen);
        VertT u = repeated_stubs[idx_u];
        std::swap(repeated_stubs[idx_u], repeated_stubs.at(current_size - 1));

        std::uniform_int_distribution<std::size_t> dist_v{0, current_size - 2};
        std::size_t idx_v = dist_v(gen);
        while (repeated_stubs[idx_v] == u) idx_v = dist_v(gen);
        VertT v = repeated_stubs[idx_v];
        std::swap(repeated_stubs[idx_v], repeated_stubs.at(current_size - 2));

        double puv =
          static_cast<double>(
              degrees[static_cast<std::size_t>(u)]*
              degrees[static_cast<std::size_t>(v)])/
            (2.0*static_cast<double>(degree_sum));
        undirected_edge<VertT> new_edge{u, v};

        // Bayati et al. 2009 suggests that the probability of accepting u, v is
        // proportional to remaining degrees of u and v and (1-d[u]d[v]/4m). We
        // have chosen u and v at random with weights proportional to remaining
        // degree, to get the complement out of the way we accept if x > puv
        // where x is randomly drawn in [0, 1).
        if (!edges.contains(new_edge) &&
            std::uniform_real_distribution{}(gen) > puv) {
          edges.insert(new_edge);

          repeated_stubs.pop_back();
          repeated_stubs.pop_back();

          stubs[static_cast<std::size_t>(u)]--;
          stubs[static_cast<std::size_t>(v)]--;

          if (stubs[static_cast<std::size_t>(u)] == 0 ||
              stubs[static_cast<std::size_t>(v)] == 0)
            suitable_edges_remaining = has_possible_edge(repeated_stubs);
        }
      }

      if (repeated_stubs.empty())
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
    constexpr std::size_t tries = 1000;
    while (!maybe_g) maybe_g = try_random_directed_degree_sequence_graph<VertT>(
        std::forward<PairRange>(in_out_degree_sequence), gen, tries);
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

    VertT degree_sum =
        std::accumulate(in_degrees.begin(), in_degrees.end(), VertT{});

    if (degree_sum !=
        std::accumulate(out_degrees.begin(), out_degrees.end(), VertT{}))
      throw std::invalid_argument(
          "in- and out-degree sequences should have equal sums");

    for (std::size_t t = 0; t < max_tries; t++) {
      std::unordered_set<
        directed_edge<VertT>,
        hash<directed_edge<VertT>>> edges;
      edges.reserve(static_cast<std::size_t>(degree_sum));

      std::vector<VertT> in_stubs = in_degrees;
      std::vector<VertT> out_stubs = out_degrees;

      std::vector<VertT> in_repeated_stubs;
      in_repeated_stubs.reserve(static_cast<std::size_t>(degree_sum));
      std::vector<VertT> out_repeated_stubs;
      out_repeated_stubs.reserve(static_cast<std::size_t>(degree_sum));

      for (VertT i = 0; i < static_cast<VertT>(in_degrees.size()); i++) {
        for (VertT j = 0; j < in_degrees[static_cast<std::size_t>(i)]; j++)
          in_repeated_stubs.push_back(i);
        for (VertT j = 0; j < out_degrees[static_cast<std::size_t>(i)]; j++)
          out_repeated_stubs.push_back(i);
      }

      auto has_possible_directed_edge =
          [&edges](const std::vector<VertT>& in_stubs,
                   const std::vector<VertT>& out_stubs) {
            if (out_stubs.empty() || in_stubs.empty())
              return false;

            for (const auto& u : out_stubs)
              for (const auto& v : in_stubs)
                if (u != v && !edges.contains({u, v}))
                  return true;
            return false;
          };

      bool suitable_edges_remaining = has_possible_directed_edge(
          in_repeated_stubs, out_repeated_stubs);
      while (suitable_edges_remaining) {
        std::size_t out_size = out_repeated_stubs.size();
        std::uniform_int_distribution<std::size_t> dist{0, out_size - 1};

        std::size_t idx_u = dist(gen);
        std::size_t idx_v = dist(gen);
        while (out_repeated_stubs[idx_u] == in_repeated_stubs[idx_v]) {
          idx_u = dist(gen);
          idx_v = dist(gen);
        }

        VertT u = out_repeated_stubs[idx_u];
        std::swap(out_repeated_stubs[idx_u], out_repeated_stubs[out_size - 1]);
        VertT v = in_repeated_stubs[idx_v];
        std::swap(in_repeated_stubs[idx_v], in_repeated_stubs[out_size - 1]);

        double puv =
          static_cast<double>(
              in_degrees[static_cast<std::size_t>(v)]*
              out_degrees[static_cast<std::size_t>(u)])/
            (2.0*static_cast<double>(degree_sum));
        directed_edge<VertT> new_edge{u, v};

        // Bayati et al. 2009 suggests that the probability of accepting u, v is
        // proportional to remaining degrees of u and v and (1-d[u]d[v]/4m). We
        // have chosen u and v at random with weights proportional to remaining
        // degree, to get the complement out of the way we accept if x > puv
        // where x is randomly drawn in [0, 1).
        if (!edges.contains(new_edge) &&
            std::uniform_real_distribution{}(gen) > puv) {
          edges.insert(new_edge);

          out_repeated_stubs.pop_back();
          in_repeated_stubs.pop_back();

          out_stubs[static_cast<std::size_t>(u)]--;
          in_stubs[static_cast<std::size_t>(v)]--;

          if (out_stubs[static_cast<std::size_t>(u)] == 0 ||
              in_stubs[static_cast<std::size_t>(v)] == 0)
            suitable_edges_remaining = has_possible_directed_edge(
                in_repeated_stubs, out_repeated_stubs);
        }
      }

      if (in_repeated_stubs.empty() && out_repeated_stubs.empty())
        return directed_network<VertT>(edges,
            views::iota(VertT{}, static_cast<VertT>(in_degrees.size())));
    }
    return std::nullopt;
  }
}  // namespace reticula
