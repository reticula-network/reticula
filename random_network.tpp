#include <random>
namespace dag {
  template <typename VertT>
  undirected_network<VertT> gnp_random_graph(size_t n, double p,
      std::mt19937_64& generator) {
    static_assert(std::is_integral<VertT>::value,
        "vertices should be of integral type");
    uintmax_t max_n = std::numeric_limits<VertT>::max()
                    - std::numeric_limits<VertT>::min();
    if (max_n <= n)
      throw dag::vertex_type_too_small_error(
          "n is too large for selected vertex type");

    std::uniform_real_distribution<> rd;
    undirected_network<VertT> net;
    net.reserve(std::lround(std::pow(n, 2)*p));

    double lp = std::log(1.0 - p);
    size_t v = 0;
    size_t w = std::lround(std::log(1.0 - rd(generator))/lp);

    while (v < n) {
      while (w >= v and v < n) {
        w = w - v;
        v = v + 1;
      }
      if (v < n) net.add_edge({(VertT)v, (VertT)w});
      double lr = std::log(1.0 - rd(generator));
      w = w + 1 + std::lround(lr/lp);
    }

    return net;
  }

  template <typename VertT>
  undirected_network<VertT> ba_random_graph(size_t n, size_t m,
      std::mt19937_64& generator) {
    static_assert(std::is_integral<VertT>::value,
        "vertices should be of integral type");
    uintmax_t max_n = std::numeric_limits<VertT>::max()
                    - std::numeric_limits<VertT>::min();
    if (max_n <= n)
      throw dag::vertex_type_too_small_error(
          "n is too large for selected vertex type");

    if (n <= m || m < 1)
      throw std::invalid_argument(
          "BA network must have m >= 1 and n > m");

    undirected_network<VertT> net;
    net.reserve(m*(n-m));

    std::vector<size_t> degrees(n);

    for (size_t i = 0; i < m; i++) {
      net.add_edge({(VertT)i, (VertT)m});
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
        net.add_edge({(VertT)current_node, (VertT)i});
        degrees[i] += 1;
        degrees[current_node] += 1;
      }
    }

    return net;
  }

}
