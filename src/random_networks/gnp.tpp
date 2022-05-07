namespace reticula {
  template <integer_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT>
  random_gnp_graph(VertT n, double p, Gen& generator) {
    if (n < 0)
      throw std::domain_error("n must be non-negative");

    if (p > 1.0 || p < 0.0)
      throw std::invalid_argument(
          "edge probability p should be in [0,1] interval");

    if (n == 0 || p == 0)
      return undirected_network<VertT>(
          std::ranges::empty_view<undirected_edge<VertT>>(),
          std::views::iota(VertT{}, n));

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(
          static_cast<double>(n)*static_cast<double>(n/2)*p));

    std::uniform_real_distribution<> rd;

    double lp = std::log(1.0 - p);
    VertT v = 1;
    VertT w = static_cast<VertT>(
        std::floor(std::log(1.0 - rd(generator))/lp));

    while (v < n) {
      while (w >= v && v < n) {
        w = w - v;
        v = v + 1;
      }
      if (v < n)
        edges.emplace_back(v, w);
      double lr = std::log(1.0 - rd(generator));
      w = w + static_cast<VertT>(1 + std::floor(lr/lp));
    }

    return undirected_network<VertT>(edges,
        std::ranges::iota_view{VertT{}, n});
  }
}  // namespace reticula
