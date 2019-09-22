#include <random>

namespace dag {
  template<class RealType = double>
    class truncated_power_law_distribution {
    public:
      RealType exponent, x0, x1, constant, mean;
      std::uniform_real_distribution<RealType> dist;

      truncated_power_law_distribution(
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

      template<class Generator>
        RealType operator()(Generator& g) {
          return std::pow(
              (constant*dist(g) + std::pow(x0, exponent+1.0)),
              (1.0/(exponent+1.0)))/mean;
        }
    };

  template <typename VertT>
  undirected_network<VertT>
  gnp_random_graph(size_t n, double p, std::mt19937_64& generator) {
    if (p > 1.0 || p < 0.0)
      throw std::invalid_argument(
          "link probability p should be in [0,1] interval");

    static_assert(std::is_integral<VertT>::value,
        "vertices should be of integral type");

    if (n == 0 || p == 0)
      return undirected_network<VertT>();

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(std::lround(std::pow(n, 2)*p));

    uintmax_t max_n = std::numeric_limits<VertT>::max()
                    - std::numeric_limits<VertT>::min();
    if (max_n <= n)
      throw utils::vertex_type_too_small_error(
          "n is too large for selected vertex type");

    std::uniform_real_distribution<> rd;;

    double lp = std::log(1.0 - p);
    size_t v = 0;
    size_t w = std::lround(std::log(1.0 - rd(generator))/lp);

    while (v < n) {
      while (w >= v && v < n) {
        w = w - v;
        v = v + 1;
      }
      if (v < n) edges.emplace_back((VertT)v, (VertT)w);
      double lr = std::log(1.0 - rd(generator));
      w = w + 1 + std::lround(lr/lp);
    }

    undirected_network<VertT> net;
    net.add_edge(edges);
    return net;
  }

  template <typename VertT>
  undirected_network<VertT>
  ba_random_graph(size_t n, size_t m, std::mt19937_64& generator) {
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

    std::vector<size_t> degrees(n, (size_t)0);

    for (size_t i = 0; i < m; i++) {
      edges.emplace_back((VertT)i, (VertT)m);
      degrees[i] += 1;
      degrees[m] += 1;
    }

    for (size_t current_node = m+1; current_node < n; current_node++) {
      std::discrete_distribution<> dist(degrees.begin(),
          degrees.begin() + current_node);

      std::unordered_set<size_t> targets;
      targets.reserve(m);

      while (targets.size() < m)
        targets.insert(dist(generator));

      for (const auto& i: targets) {
        edges.emplace_back((VertT)current_node, (VertT)i);
        degrees[i] += 1;
        degrees[current_node] += 1;
      }
    }

    undirected_network<VertT> net;
    net.add_edge(edges);
    return net;
  }

  template <class EdgeT, class Distribution>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      size_t seed,
      size_t size_hint = 0) {
    std::vector<EdgeT> temp;
    if (size_hint > 0) {
      temp.reserve(size_hint);
    } else {
      typename EdgeT::TimeType size_part =
        static_cast<double>(base_net.edges().size())*max_t;
      if constexpr (std::is_same<Distribution,
          std::exponential_distribution<typename EdgeT::TimeType>>::value)
          temp.reserve(std::lround(size_part*inter_event_time_dist.lambda));
      else if constexpr (std::is_same<Distribution,
          truncated_power_law_distribution<
                typename EdgeT::TimeType>>::value)
          temp.reserve(std::lround(size_part/inter_event_time_dist.mean));
      else if constexpr (std::is_same<Distribution,
          std::poisson_distribution<typename EdgeT::TimeType>>::value)
          temp.reserve(std::lround(size_part/inter_event_time_dist.mean));
    }

    for (const auto& e: base_net.edges()) {
      size_t edge_seed = utils::combine_hash(seed,
          std::hash<typename base_net::EdgeType>{}(e));
      std::mt19937_64 generator(edge_seed);
      typename EdgeT::TimeType t = static_cast<typename EdgeT::TimeType>(
          inter_event_time_dist(generator));
      while (t < max_t) {
        temp.emplace_back({e.v1, e.v2, t});
        typename EdgeT::TimeType iet = static_cast<typename EdgeT::TimeType>(
            inter_event_time_dist(generator));
        t += iet;
      }
    }
    return temp;
  }
}  // namespace dag
