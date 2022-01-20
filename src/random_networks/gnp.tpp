namespace dag {
  template <integer_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT>
  random_gnp_graph(VertT n, double p, Gen& generator) {
    if (n <= 0)
      throw std::domain_error("n must be positive");

    if (p > 1.0 || p < 0.0)
      throw std::invalid_argument(
          "edge probability p should be in [0,1] interval");

    if (n == 0 || p == 0)
      return undirected_network<VertT>();

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(
          std::lround(std::pow(n, 2)*p)));

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
        edges.emplace_back(static_cast<VertT>(v), static_cast<VertT>(w));
      double lr = std::log(1.0 - rd(generator));
      w = w + 1 + static_cast<VertT>(std::floor(lr/lp));
    }

    return undirected_network<VertT>(edges);
  }
}  // namespace dag
