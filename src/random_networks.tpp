namespace dag {
  template <class RealType>
  truncated_power_law_distribution<RealType>::truncated_power_law_distribution(
      RealType exponent,
      RealType x0,
      RealType x1,
      RealType average) :
    exponent(exponent), x0(x0), x1(x1) {
      constant = (std::pow(x1, (exponent+1.0)) -
          std::pow(x0, (exponent+1.0)));
      mean = ((std::pow(x1, exponent+2.0)-std::pow(x0, exponent+2.0))
          /(exponent+2.0))/(constant/(exponent+1.0));
      mean = mean/average;
  }

  template <class RealType>
  template <class Generator>
  RealType
  truncated_power_law_distribution<RealType>::operator()(Generator& g) {
          return std::pow(
              (constant*dist(g) + std::pow(x0, exponent+1.0)),
              (1.0/(exponent+1.0)))/mean;
  }

  template <network_vertex VertT>
  undirected_network<VertT>
  gnp_random_graph(VertT n, double p, std::mt19937_64& generator) {
    if (p > 1.0 || p < 0.0)
      throw std::invalid_argument(
          "link probability p should be in [0,1] interval");

    static_assert(std::is_integral<VertT>::value,
        "vertices should be of integral type");

    if (n == 0 || p == 0)
      return undirected_network<VertT>();

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(
          std::lround(std::pow(n, 2)*p)));

    uintmax_t max_n = std::numeric_limits<VertT>::max()
                    - std::numeric_limits<VertT>::min();
    if (max_n <= n)
      throw utils::vertex_type_too_small_error(
          "n is too large for selected vertex type");

    std::uniform_real_distribution<> rd;

    double lp = std::log(1.0 - p);
    std::size_t v = 1;
    std::size_t w = static_cast<std::size_t>(
        std::floor(std::log(1.0 - rd(generator))/lp));

    while (v < n) {
      while (w >= v && v < n) {
        w = w - v;
        v = v + 1;
      }
      if (v < n)
        edges.emplace_back(static_cast<VertT>(v), static_cast<VertT>(w));
      double lr = std::log(1.0 - rd(generator));
      w = w + 1 + static_cast<std::size_t>(std::floor(lr/lp));
    }

    return undirected_network<VertT>(edges);
  }

  template <network_vertex VertT>
  undirected_network<VertT>
  ba_random_graph(VertT n, VertT m, std::mt19937_64& generator) {
    static_assert(std::is_integral<VertT>::value,
        "vertices should be of integral type");

    uintmax_t max_n = std::numeric_limits<VertT>::max()
                    - std::numeric_limits<VertT>::min();
    if (max_n <= n)
      throw utils::vertex_type_too_small_error(
          "n is too large for selected vertex type");

    if (n <= m || m < 1)
      throw std::invalid_argument(
          "BA network must have m >= 1 and n > m");

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(m*(n-m));

    std::vector<std::size_t> degrees(n);

    for (std::size_t i = 0; i < m; i++) {
      edges.emplace_back((VertT)i, (VertT)m);
      degrees[i] += 1;
      degrees[m] += 1;
    }

    for (std::size_t current_node = m+1; current_node < n; current_node++) {
      std::discrete_distribution<std::size_t> dist(
          degrees.begin(),
          degrees.begin() + static_cast<std::ptrdiff_t>(current_node));

      std::unordered_set<std::size_t> targets;
      targets.reserve(m);

      while (targets.size() < m)
        targets.insert(dist(generator));

      for (const auto& i: targets) {
        edges.emplace_back((VertT)current_node, (VertT)i);
        degrees[i] += 1;
        degrees[current_node] += 1;
      }
    }

    return undirected_network<VertT>(edges);
  }

  template <temporal_edge EdgeT, class Distribution, class ResDistribution>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      ResDistribution residual_time_dist,
      std::size_t seed,
      std::size_t size_hint) {
    std::vector<EdgeT> temp;
    if (size_hint > 0)
      temp.reserve(size_hint);

    for (const auto& e: base_net.edges()) {
      std::size_t edge_seed = utils::combine_hash(seed, e);
      std::mt19937_64 generator(edge_seed);
      typename EdgeT::TimeType t = static_cast<typename EdgeT::TimeType>(
          residual_time_dist(generator));
      while (t < max_t) {
        auto v = e.mutated_verts();  // e is an undirected event
        temp.emplace_back(v[0], v[1], t);
        typename EdgeT::TimeType iet = static_cast<typename EdgeT::TimeType>(
            inter_event_time_dist(generator));
        t += iet;
      }
    }
    return temp;
  }

  template <temporal_edge EdgeT, class Distribution>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      std::size_t seed,
      std::size_t size_hint) {
    std::vector<EdgeT> temp;
    if (size_hint > 0)
      temp.reserve(size_hint);

    for (const auto& e: base_net.edges()) {
      std::size_t edge_seed = utils::combine_hash(seed, e);
      std::mt19937_64 generator(edge_seed);
      // we can't be sure TimeType is signed so we start at zero and warm-up
      // until max_t then record t - max_t from there up to max_t*2
      typename EdgeT::TimeType t {};
      while (t < max_t * 2) {
        auto v = e.mutated_verts();  // e is an undirected event
        if (t > max_t)
          temp.emplace_back(v[0], v[1], t - max_t);
        typename EdgeT::TimeType iet = static_cast<typename EdgeT::TimeType>(
          inter_event_time_dist(generator));
        t += iet;
      }
    }
    return temp;
  }
}  // namespace dag
