#include <cmath>

namespace dag {
  namespace adjacency_prob {
    template <class EdgeT>
    deterministic<EdgeT>::deterministic(typename EdgeT::TimeType dt) :
      _dt{dt} {};

    template <class EdgeT>
    double deterministic<EdgeT>::p(const EdgeT& a, const EdgeT& b) const {
      if (b.cause_time() > a.effect_time() &&
          b.cause_time() - a.effect_time() < _dt)
        return 1;
      else
        return 0;
    }

    template <class EdgeT>
    typename EdgeT::TimeType
    deterministic<EdgeT>::cutoff_dt() const {
      return _dt;
    }

    template <class EdgeT>
    exponential<EdgeT>::exponential(typename EdgeT::TimeType expected_dt) :
      _dt{expected_dt} {};

    template <class EdgeT>
    double exponential<EdgeT>::p(const EdgeT& a, const EdgeT& b) const {
      if (b.cause_time() < a.effect_time())
        return 0;
      typename EdgeT::TimeType dt = b.cause_time() - a.effect_time();
      double lambda = (1.0/static_cast<double>(_dt));
      return lambda*std::exp(-lambda*(static_cast<double>(dt)));
    }

    template <class EdgeT>
    typename EdgeT::TimeType
    exponential<EdgeT>::expected_dt() const {
      return _dt;
    }

    template <class EdgeT>
    bool
    bernoulli_trial(const EdgeT& a, const EdgeT& b, double prob, size_t seed) {
      if (prob == 1) {
        return true;
      } else if (prob == 0) {
        return false;
      } else {
        size_t dag_edge_seed =
          utils::combine_hash(utils::combine_hash(seed, a), b);
        std::mt19937_64 gen(dag_edge_seed);
        std::bernoulli_distribution dist(prob);
        return dist(gen);
      }
    }
  }  // namespace adjacency_prob
}  // namespace dag
