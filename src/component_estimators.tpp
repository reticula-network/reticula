#include <unordered_set>
#include <cmath>

namespace dag {
  template <typename EdgeT,
          template<typename> class NodeEstimatorT,
          template<typename> class EdgeEstimatorT=NodeEstimatorT>
  class component_size_counter {
  public:
    using TimeType = typename EdgeT::TimeType;
    using VertexType = typename EdgeT::VertexType;

    component_size_counter(uint32_t seed, size_t node_est=0, size_t edge_est=0):
      _node_set(seed, node_est),
      _edge_set(seed, edge_est),
      min_time(std::numeric_limits<TimeType>::max()),
      max_time(std::numeric_limits<TimeType>::lowest()) {}

    template <template<typename> class OtherNodeEstimatorT,
            template<typename> class OtherEdgeEstimatorT>
    component_size_counter(const component_size_counter<EdgeT,
        OtherNodeEstimatorT,
        OtherEdgeEstimatorT>& other):
      _node_set(other.node_set()),
      _edge_set(other.edge_set()),
      min_time(other.lifetime().first),
      max_time(other.lifetime().second) {}

    void insert(const EdgeT& e) {
      _edge_set.insert(e);

      for (auto&& n: e.mutated_verts())
        _node_set.insert(n);

      min_time = std::min(min_time, e.time);
      max_time = std::max(max_time, e.time);
    }

    void merge(const component_size_counter<EdgeT,
        NodeEstimatorT,
        EdgeEstimatorT>& other) {
      _node_set.merge(other.node_set());
      _edge_set.merge(other.edge_set());

      auto other_lt = other.lifetime();
      min_time = std::min(min_time, other_lt.first);
      max_time = std::max(max_time, other_lt.second);
    }

    const NodeEstimatorT<VertexType>& node_set() const { return _node_set; }
    const EdgeEstimatorT<EdgeT>& edge_set() const { return _edge_set; }
    std::pair<TimeType, TimeType> lifetime() const {
      return std::make_pair(min_time, max_time);
    }

  private:
    NodeEstimatorT<VertexType> _node_set;
    EdgeEstimatorT<EdgeT> _edge_set;

    TimeType min_time, max_time;
  };

  template <typename T, typename HyperLogLogT>
  class hll_estimator {
  public:
    hll_estimator(uint32_t seed, size_t /*size_est*/) : _estimator(true, seed) {}

    double estimate() const { return _estimator.estimate(); }

    void insert(const T& item) { _estimator.insert(item); }

    void merge(const hll_estimator<T, HyperLogLogT>& other) {
      _estimator.merge(other.estimator());
    }

    const HyperLogLogT& estimator() const { return _estimator; }

    static double p_larger(double estimate, size_t limit,
        size_t max_size=std::numeric_limits<std::size_t>::max() ) {

      if (limit > max_size)
        return 0.0;

      constexpr double cutoff = 1e-10;

      double sigma = 1.05/pow(2.0, HyperLogLogT::dense_prec/2.0);

      double p_larger = 0.0;
      double p_total = 0.0;

      if (estimate < (double)limit*(1-sigma*6))
        return 0;

      double p_size = normal_pdf(estimate, (double)limit, sigma*(double)limit);

      if (p_size < cutoff) {
        if (estimate < (double)limit)
          return 0;
        else
          return 1;
      }

      double min_search = (1 > 6*sigma) ? estimate - 6*sigma*estimate : 1;
      size_t i = (size_t)min_search;
      while (i <= limit || (p_size > cutoff && i <= max_size)) {

        p_size = normal_pdf(estimate, (double)i, sigma*(double)i);
        p_total += p_size;
        if (i > limit)
          p_larger += p_size;
        i++;
      }

      return p_larger/p_total;
    }

  private:
    HyperLogLogT _estimator;

    static double normal_pdf(double x, double mean, double stddev) {
      constexpr double inv_sqrt_2pi = 0.3989422804014327;
      double a = (x - mean)/stddev;

      return inv_sqrt_2pi/stddev*std::exp(-0.5*a*a);
    }

  };

  template <typename T>
  class exact_estimator {
  public:
    exact_estimator(uint32_t /*seed*/, size_t size_est) {
      if (size_est > 0)
        _set.reserve(size_est);
    }

    size_t size() const { return _set.size(); }

    void insert(const T& item) { _set.insert(item); }

    void merge(const exact_estimator<T>& other) { _set.merge(other.set()); }

    bool contains(const T& item) const { return _set.find(item) != _set.end(); }

    const std::unordered_set<T>& set() const { return _set; }

  private:
    std::unordered_set<T> _set;
  };

  template <typename T, typename HyperLogLogT>
  class hll_estimator_readonly {
  public:
    hll_estimator_readonly(uint32_t /*seed*/, size_t size_est)
      : _est((double)size_est) {}

    hll_estimator_readonly(const hll_estimator<T, HyperLogLogT>& hll_est) {
      _est = hll_est.estimate();
    }

    double estimate() const { return _est; }
    void insert(const T& item) {
      throw std::logic_error("cannot insert into read-only hll estimator");
    }
    void merge(const hll_estimator_readonly<T, HyperLogLogT>& other) {
      throw std::logic_error("cannot merge read-only hll estimator");
    }

    static double p_larger(double estimate, size_t limit,
        size_t max_size=std::numeric_limits<std::size_t>::max() ) {
      return hll_estimator<T, HyperLogLogT>::p_larger(estimate, limit, max_size);
    }

  private:
    double _est;
  };
}
