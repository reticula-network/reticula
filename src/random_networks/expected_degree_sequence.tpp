namespace reticula {
  template <
    integer_vertex VertT,
    std::ranges::input_range Range,
    std::uniform_random_bit_generator Gen>
  requires weight_range<Range>
  undirected_network<VertT>
  random_expected_degree_sequence_graph(
      const Range& weight_sequence,
      Gen& generator,
      bool self_loops) {
    std::vector<std::pair<double, VertT>> weight_node_pairs;
    if constexpr (std::ranges::sized_range<Range>)
      weight_node_pairs.reserve(std::size(weight_sequence));

    for (VertT i{}; auto&& w: weight_sequence)
      weight_node_pairs.emplace_back(static_cast<double>(w), i++);

    std::ranges::sort(weight_node_pairs, std::ranges::greater());

    if (weight_node_pairs.empty())
      return undirected_network<VertT>();

    if (weight_node_pairs.front().first == 0.0)
      return undirected_network<VertT>(
          std::ranges::empty_view<undirected_edge<VertT>>{},
          std::ranges::iota_view{
            VertT{}, static_cast<VertT>(weight_node_pairs.size())});

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
        std::ranges::iota_view{
          VertT{}, static_cast<VertT>(weight_node_pairs.size())});
  }

  template <
    integer_vertex VertT,
    std::ranges::input_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires weight_pair_range<PairRange>
  directed_network<VertT> random_directed_expected_degree_sequence_graph(
      const PairRange& in_out_weight_sequence,
      Gen& generator,
      bool self_loops) {
    std::vector<std::pair<double, VertT>>
      in_weight_node_pairs, out_weight_node_pairs;

    if constexpr (std::ranges::sized_range<PairRange>) {
      in_weight_node_pairs.reserve(std::ranges::size(in_out_weight_sequence));
      out_weight_node_pairs.reserve(std::ranges::size(in_out_weight_sequence));
    }

    for (VertT i{}; auto& [w_in, w_out]: in_out_weight_sequence) {
      in_weight_node_pairs.emplace_back(static_cast<double>(w_in), i);
      out_weight_node_pairs.emplace_back(static_cast<double>(w_out), i++);
    }

    std::ranges::sort(in_weight_node_pairs, std::ranges::greater());
    std::ranges::sort(out_weight_node_pairs, std::ranges::greater());

    std::vector<std::size_t> node_index_in_in_weight_pair(
        in_weight_node_pairs.size(), 0);
    for (std::size_t j = 0; auto& [w, i]: in_weight_node_pairs)
      node_index_in_in_weight_pair[static_cast<std::size_t>(i)] = j++;

    if (in_weight_node_pairs.empty())
      return directed_network<VertT>();

    if (in_weight_node_pairs.front().first == 0.0 &&
        out_weight_node_pairs.front().first == 0.0)
      return directed_network<VertT>(
          std::ranges::empty_view<directed_edge<VertT>>{},
          std::ranges::iota_view{
            VertT{}, static_cast<VertT>(in_weight_node_pairs.size())});

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

    if (s_out != s_in)
      throw std::invalid_argument(
          "in- and out-weight sequences should have equal sums");

    std::vector<directed_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(s_in + 4.0*std::sqrt(s_in)));

    for (std::size_t u = 0; u < out_weight_node_pairs.size(); u++) {
      std::size_t v = 0;

      auto [wu, i] = out_weight_node_pairs[u];
      auto [wv, j] = in_weight_node_pairs[v];

      double p = std::min(wu*wv/s_in, 1.0);
      while (v < in_weight_node_pairs.size() && p > 0) {
        if (p != 1.0) {
          std::size_t delta = static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));

          if (!self_loops)
            if (node_index_in_in_weight_pair[i] >= v &&
                node_index_in_in_weight_pair[i] <= v+delta)
              delta++;
          v += delta;
        }

        if (v < in_weight_node_pairs.size()) {
          std::tie(wv, j) = in_weight_node_pairs[v];
          double q = std::min(wu*wv/s_in, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p) {
            edges.emplace_back(i, j);
          }
          p = q;
          v++;
        }
      }
    }

    return directed_network<VertT>(edges,
        std::ranges::iota_view{
          VertT{}, static_cast<VertT>(in_weight_node_pairs.size())});
  }
}  // namespace reticula
