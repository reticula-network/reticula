#include "../include/dag/algorithms.hpp"

namespace dag {
  template <network_vertex VertT, std::uniform_random_bit_generator Gen>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT>
  gnp_random_graph(VertT n, double p, Gen& generator) {
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

  template <network_vertex VertT, std::uniform_random_bit_generator Gen>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT>
  ba_random_graph(VertT n, VertT m, Gen& generator) {
    if (n <= m || m < 1)
      throw std::invalid_argument(
          "BA network must have m >= 1 and n > m");

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(m*(n-m)));

    std::vector<VertT> degrees(static_cast<std::size_t>(n));

    for (VertT i = 0; i < m; i++) {
      edges.emplace_back(i, m);
      degrees[static_cast<std::size_t>(i)] += 1;
      degrees[static_cast<std::size_t>(m)] += 1;
    }

    for (VertT current_node = m+1; current_node < n; current_node++) {
      std::discrete_distribution<VertT> dist(
          degrees.begin(),
          degrees.begin() + static_cast<std::ptrdiff_t>(current_node));

      std::unordered_set<VertT> targets;
      targets.reserve(static_cast<std::size_t>(m));

      while (targets.size() < static_cast<std::size_t>(m))
        targets.insert(dist(generator));

      for (const auto& i: targets) {
        edges.emplace_back(current_node, i);
        degrees[static_cast<std::size_t>(i)] += 1;
        degrees[static_cast<std::size_t>(current_node)] += 1;
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
      return false;
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

  template <network_vertex VertT, std::uniform_random_bit_generator Gen>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> random_regular_graph(
      VertT size, VertT degree,
      Gen& gen) {
    std::optional<undirected_network<VertT>> maybe_g = std::nullopt;
    while (!maybe_g) maybe_g = try_random_regular_graph(
        size, degree, gen, 1000);
    return *maybe_g;
  }

  template <network_vertex VertT, std::uniform_random_bit_generator Gen>
  requires std::numeric_limits<VertT>::is_integer
  std::optional<undirected_network<VertT>>
  try_random_regular_graph(
      VertT size, VertT degree,
      Gen& gen,
      std::size_t max_tries) {
    if (size*degree % 2 != 0)
      throw std::domain_error("size or degree must be even");

    if (size <= 0)
      throw std::domain_error("size must be positive");

    if (degree >= size)
      throw std::domain_error("degree must be less than size");

    for (std::size_t t = 0; t < max_tries; t++) {
      std::unordered_set<
        undirected_edge<VertT>,
        hash<undirected_edge<VertT>>> edges;
      std::vector<VertT> stubs;
      stubs.reserve(static_cast<std::size_t>(size*degree));
      for (VertT i = 0; i < size; i++)
        for (VertT d = 0; d < degree; d++)
          stubs.emplace_back(i);

      bool suitable = true;
      while (suitable) {
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

        stubs.clear();
        for (auto& [node, count]: hanging_stubs)
          for (std::size_t i = 0; i < count; i++)
             stubs.emplace_back(node);

        suitable = _random_regular_suitable_hanging_edge(edges, hanging_stubs);
      }

      if (edges.size() == static_cast<std::size_t>(size*degree)/2)
        return undirected_network<VertT>(
            std::vector<dag::undirected_edge<VertT>>(
              edges.begin(), edges.end()));
    }

    return std::nullopt;
  }

  // checks if there is at least one suitable new edge possible with current
  // stubs
  template <network_vertex VertT, typename It>
  bool _random_degree_sequence_suitable_hanging_edge(
      const std::unordered_set<
        undirected_edge<VertT>,
        hash<undirected_edge<VertT>>>& edges,
      It nodes_begin, It nodes_end) {
    std::size_t size = static_cast<std::size_t>(
        std::distance(nodes_begin, nodes_end));
    if (size == 0) {
      return false;
    } else {
      for (std::size_t i = 0; i < size; i++)
        for (std::size_t j = 0; j < i; j++)
          if (!edges.contains({*(nodes_begin+i), *(nodes_begin+j)}))
            return true;
    }

    return false;
  }

  template <
    network_vertex VertT,
    std::ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    std::convertible_to<std::ranges::range_value_t<Range>, VertT> &&
    std::numeric_limits<VertT>::is_integer
  undirected_network<VertT>
  random_degree_sequence_graph(
      const Range& degree_sequence,
      Gen& gen) {
    std::optional<undirected_network<VertT>> maybe_g = std::nullopt;
    while (!maybe_g) maybe_g = try_random_degree_sequence_graph<VertT>(
        degree_sequence, gen, 1000);
    return *maybe_g;
  }

  template <
    network_vertex VertT,
    std::ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    std::convertible_to<std::ranges::range_value_t<Range>, VertT> &&
    std::numeric_limits<VertT>::is_integer
  std::optional<undirected_network<VertT>>
  try_random_degree_sequence_graph(
      const Range& degree_sequence,
      Gen& gen,
      std::size_t max_tries) {
    if (!is_graphic(degree_sequence))
      throw std::invalid_argument("degree_sequence is not graphical");

    std::vector<VertT> degrees;
    if constexpr (std::ranges::sized_range<Range>)
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

      VertT stubs_sum = degree_sum;

      std::vector<std::size_t> nodes;
      nodes.reserve(degrees.size());
      std::vector<std::size_t> stubs;
      stubs.reserve(degrees.size());

      for (std::size_t i = 0; i < degrees.size(); i++) {
        if (degrees[i] > 0) {
          nodes.push_back(i);
          stubs.push_back(degrees[i]);
        }
      }

      auto stubs_end = stubs.end();
      auto nodes_end = nodes.end();

      std::discrete_distribution<std::size_t> d(stubs.begin(), stubs_end);
      bool suitable_edges_remaining =
        _random_degree_sequence_suitable_hanging_edge(edges,
            nodes.begin(), nodes_end);
      while (suitable_edges_remaining) {
        std::size_t i = d(gen), j = d(gen);
        while (i == j) j = d(gen);

        std::size_t u = nodes[i], v = nodes[j];

        double puv =
          static_cast<double>(degrees[u]*degrees[v])/
            (2.0*static_cast<double>(degree_sum));
        if (undirected_edge<VertT> new_edge(
              static_cast<VertT>(u), static_cast<VertT>(v));
            !edges.contains(new_edge) &&
              std::uniform_real_distribution{}(gen) > puv) {
          edges.insert(std::move(new_edge));

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
            suitable_edges_remaining =
              _random_degree_sequence_suitable_hanging_edge(
                  edges, nodes.begin(), nodes_end);

          d = std::discrete_distribution<std::size_t>(stubs.begin(), stubs_end);
        }
      }

      if (stubs_sum == 0)
        return undirected_network<VertT>(edges,
            std::ranges::iota_view{
              VertT{},
              static_cast<VertT>(degrees.size())
            });
    }
    return std::nullopt;
  }

  template <
    network_vertex VertT,
    std::ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    std::is_arithmetic_v<std::ranges::range_value_t<Range>> &&
    std::numeric_limits<VertT>::is_integer
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
          std::size_t{}, weight_node_pairs.size()});
  }

  template <
    network_vertex VertT,
    std::ranges::forward_range Range1,
    std::ranges::forward_range Range2,
    std::uniform_random_bit_generator Gen>
  requires
    std::is_arithmetic_v<std::ranges::range_value_t<Range1>> &&
    std::is_arithmetic_v<std::ranges::range_value_t<Range2>> &&
    std::numeric_limits<VertT>::is_integer
  directed_network<VertT> random_directed_expected_degree_sequence_graph(
      const Range1& in_weight_sequence,
      const Range2& out_weight_sequence,
      Gen& generator,
      bool self_loops) {
    std::vector<std::pair<double, VertT>>
      in_weight_node_pairs, out_weight_node_pairs;
    if constexpr (std::ranges::sized_range<Range1>)
      in_weight_node_pairs.reserve(std::size(in_weight_sequence));
    if constexpr (std::ranges::sized_range<Range2>)
      out_weight_node_pairs.reserve(std::size(out_weight_sequence));

    for (VertT i{}; auto&& w: in_weight_sequence)
      in_weight_node_pairs.emplace_back(static_cast<double>(w), i++);

    for (VertT i{}; auto&& w: out_weight_sequence)
      out_weight_node_pairs.emplace_back(static_cast<double>(w), i++);

    if (in_weight_node_pairs.size() != out_weight_node_pairs.size())
      throw std::invalid_argument(
          "in- and out-degree sequences are not of the same size");

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
          "in- and out-degree sequences should have the same sum");

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
          std::size_t{}, in_weight_node_pairs.size()});
  }

  template <
    temporal_edge EdgeT,
    random_number_distribution Distribution,
    random_number_distribution ResDistribution,
    std::uniform_random_bit_generator Gen>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      ResDistribution residual_time_dist,
      Gen& generator,
      std::size_t size_hint) {
    std::vector<EdgeT> temp;
    if (size_hint > 0)
      temp.reserve(size_hint);

    // reproducable since base_net.edges() is always sorted
    for (const auto& e: base_net.edges()) {
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

  template <
    temporal_edge EdgeT,
    random_number_distribution Distribution,
    std::uniform_random_bit_generator Gen>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      Gen& generator,
      std::size_t size_hint) {
    std::vector<EdgeT> temp;
    if (size_hint > 0)
      temp.reserve(size_hint);

    // reproducable since base_net.edges() is always sorted
    for (const auto& e: base_net.edges()) {
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

      ed = std::exp(-_theta * (e + d));
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
