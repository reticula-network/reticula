namespace dag {
  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT>
  gnp_random_graph(VertT n, double p, std::mt19937_64& generator) {
    if (p > 1.0 || p < 0.0)
      throw std::invalid_argument(
          "link probability p should be in [0,1] interval");

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
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT>
  ba_random_graph(VertT n, VertT m, std::mt19937_64& generator) {
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

  template <network_vertex VertT>
  bool _random_regular_suitable_hanging_edge(
      const std::unordered_set<
        undirected_edge<VertT>,
        hash<undirected_edge<VertT>>>& edges,
      const std::unordered_map<
        VertT, std::size_t, hash<VertT>>& hanging_stubs) {
    if (hanging_stubs.empty()) {
      return true;
    } else {
      for (auto& [node1, count1]: hanging_stubs)
        for (auto& [node2, count1]: hanging_stubs)
          if (node1 == node2)
            break;
          else if (edges.find({node1, node2}) == edges.end())
            return true;
    }

    return false;
  }


  template <network_vertex VertT>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> random_regular_graph(
      VertT size, VertT degree,
      std::mt19937_64& gen) {
    if (size*degree % 2 != 0)
      throw std::domain_error("size or degree must be even");

    if (degree >= size)
      throw std::domain_error("degree must be less than size");

       // There is always an answer. We Just have to look hard enough.
    while (true) {
      std::unordered_set<
        undirected_edge<VertT>,
        hash<undirected_edge<VertT>>> edges;
      std::vector<VertT> stubs;
      stubs.reserve(size*degree);
      for (std::size_t i = 0; i < size; i++)
        for (std::size_t d = 0; d < degree; d++)
          stubs.emplace_back(i);

      bool suitable = true;
      while (!stubs.empty() && suitable) {
        std::shuffle(stubs.begin(), stubs.end(), gen);
        std::unordered_map<VertT, std::size_t, hash<VertT>> hanging_stubs;
        for (auto iter = stubs.begin();
            iter < stubs.end();
            std::advance(iter, 2)) {
          if (*iter != *(iter+1) &&
              edges.find({*iter, *(iter+1)}) == edges.end()) {
            edges.emplace(*iter, *(iter+1));
          } else {
            hanging_stubs[*iter]++;
            hanging_stubs[*(iter+1)]++;
          }
        }

        suitable = _random_regular_suitable_hanging_edge(edges, hanging_stubs);

        stubs.clear();
        for (auto& [node, count]: hanging_stubs)
          for (std::size_t i = 0; i < count; i++)
             stubs.emplace_back(node);
      }

      if (suitable)
        return undirected_network<VertT>(
            std::vector<dag::undirected_edge<VertT>>(
              edges.begin(), edges.end()));
    }

    // Unreachable.
    return undirected_network<VertT>();
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

  // power_law_with_specified_mean
  template <class RealType>
  power_law_with_specified_mean<RealType>::power_law_with_specified_mean(
      RealType exponent, RealType mean) : _exponent(exponent), _mean(mean) {
    if (exponent <= 2)
      throw std::domain_error("exponent should be larger than 2");
    if (mean <= 0)
      throw std::domain_error("mean should be positive");

    _x_min = _mean*(_exponent - 2.0)/(_exponent - 1.0);
  }

  template <class RealType>
  template <class Generator>
  RealType
  power_law_with_specified_mean<RealType>::operator()(Generator& g) const {
    std::uniform_real_distribution<RealType> dist;
    return _x_min*std::pow((1.0 - dist(g)), 1.0/(1.0 - _exponent));
  }

  template <class RealType>
  RealType power_law_with_specified_mean<RealType>::x_min() const {
    return _x_min;
  }

  template <class RealType>
  RealType power_law_with_specified_mean<RealType>::exponent() const {
    return _exponent;
  }

  template <class RealType>
  RealType power_law_with_specified_mean<RealType>::mean() const {
    return _mean;
  }


  // residual_power_law_with_specified_mean
  template <class RealType>
  residual_power_law_with_specified_mean<RealType>::
    residual_power_law_with_specified_mean(RealType exponent, RealType mean)
      : _exponent(exponent), _mean(mean) {
    if (exponent <= 2)
      throw std::domain_error("exponent should be larger than 2");
    if (mean <= 0)
      throw std::domain_error("mean should be positive");

    _x_min = _mean*(_exponent - 2.0)/(_exponent - 1.0);
  }

  template <class RealType>
  template <class Generator>
  RealType residual_power_law_with_specified_mean<RealType>::operator()(
      Generator& g) const {
    std::uniform_real_distribution<RealType> dist;
    return _x_min*std::pow((1.0 - dist(g)), 1.0/(1.0 - _exponent));
  }

  template <class RealType>
  RealType residual_power_law_with_specified_mean<RealType>::x_min() const {
    return _x_min;
  }

  template <class RealType>
  RealType residual_power_law_with_specified_mean<RealType>::exponent() const {
    return _exponent;
  }

  template <class RealType>
  RealType residual_power_law_with_specified_mean<RealType>::mean() const {
    return _mean;
  }

  // hawkes_univariate_exponential
  template <class RealType>
  hawkes_univariate_exponential<RealType>::hawkes_univariate_exponential(
    RealType mu, RealType alpha, RealType theta, RealType phi) :
      _mu(mu), _alpha(alpha), _theta(theta), _phi(phi) {}

  // The implementation here is based on Ogata’s modified thinning algorithm.
  // Inspired by https://git.io/JD2cm
  template <class RealType>
  template <class Generator>
  RealType hawkes_univariate_exponential<RealType>::operator()(Generator& g) {
    RealType iet = {}, d = {}, ed, m, lda, r2;
    std::uniform_real_distribution dist;
    do {
      m = _mu + _alpha * _theta * (static_cast<RealType>(1.0) + _phi);

      RealType e = -std::log(dist(g))/m;
      iet += e;

      ed = std::exp(-theta * (e + d));
      lda = _mu + _alpha * _theta * ed * (static_cast<RealType>(1.0) + _phi);

      r2 = dist(g);
      d += e;
    } while (r2 * m > lda);

    _phi = ed * (1.0 + _phi);
    return iet;
  }

  template <class RealType>
  RealType hawkes_univariate_exponential<RealType>::mu() const {
    return _mu;
  }

  template <class RealType>
  RealType hawkes_univariate_exponential<RealType>::alpha() const {
    return _alpha;
  }

  template <class RealType>
  RealType hawkes_univariate_exponential<RealType>::theta() const {
    return _theta;
  }

  template <class RealType>
  RealType hawkes_univariate_exponential<RealType>::phi() const {
    return _phi;
  }

}  // namespace dag
