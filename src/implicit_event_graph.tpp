#include <queue>
#include <random>
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
  }  // namespace adjacency_prob


  template <class EdgeT, class AdjacencyProbT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::implicit_event_graph(
      const std::vector<EdgeT>& events,
      const AdjacencyProbT& prob,
      size_t seed) :
    _seed(seed), _temp(events), _prob(prob) {}

  template <class EdgeT, class AdjacencyProbT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::implicit_event_graph(
      const network<EdgeT>& temp,
      const AdjacencyProbT& prob,
      size_t seed) :
    _seed(seed), _temp(temp), _prob(prob) {}



  template <class EdgeT, class AdjacencyProbT>
  const std::vector<EdgeT>&
  implicit_event_graph<EdgeT, AdjacencyProbT>::events() const {
    return _temp.edges();
  }


  template <class EdgeT, class AdjacencyProbT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  implicit_event_graph<EdgeT, AdjacencyProbT>::time_window() const {
    if (_temp.edges().empty())
      return std::make_pair(0, 0);
    else
      return std::make_pair(
          _temp.edges().front().cause_time(),
          _temp.edges().back().cause_time());
  }


  template <class EdgeT, class AdjacencyProbT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::predecessors(
      const EdgeT& e, bool just_first) const {
    std::vector<EdgeT> pred;

    pred.reserve(e.mutator_verts().size());

    for (auto&& v : e.mutator_verts()) {
      size_t middle_offset = pred.size();
      auto res = predecessors_vert(e, v, just_first);
      pred.reserve(pred.size()+res.size());
      std::sort(res.begin(), res.end());
      std::copy(
          res.rbegin(), res.rend(),
          std::back_inserter(pred));
      std::inplace_merge(pred.begin(), pred.begin()+middle_offset, pred.end());
    }

    pred.erase(std::unique(pred.begin(), pred.end()), pred.end());

    return pred;
  }

  template <class EdgeT, class AdjacencyProbT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::successors(
      const EdgeT& e, bool just_first) const {
    std::vector<EdgeT> succ;

    succ.reserve(e.mutated_verts().size());

    for (auto&& v : e.mutated_verts()) {
      size_t middle_offset = succ.size();
      auto res = successors_vert(e, v, just_first);
      succ.reserve(succ.size()+res.size());
      std::sort(res.begin(), res.end());
      std::copy(
          res.begin(), res.end(),
          std::back_inserter(succ));
      std::inplace_merge(succ.begin(),
          succ.begin()+middle_offset,
          succ.end());
    }

    succ.erase(std::unique(succ.begin(), succ.end()), succ.end());

    return succ;
  }

  template <class EdgeT, class AdjacencyProbT>
  bool implicit_event_graph<EdgeT, AdjacencyProbT>::bernoulli_trial(
      const EdgeT& a, const EdgeT& b, double p) const {
    if (p == 1) {
      return true;
    } else if (p == 0) {
      return false;
    } else {
      size_t dag_edge_seed = utils::combine_hash(_seed, a);
      dag_edge_seed = utils::combine_hash(dag_edge_seed, b);

      std::mt19937_64 gen(dag_edge_seed);
      std::bernoulli_distribution dist(p);
      return dist(gen);
    }
  }

  template <class EdgeT, class AdjacencyProbT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::successors_vert(
      const EdgeT& e, VertexType v, bool just_first) const {
    constexpr double cutoff = 1e-20;

    size_t reserve_max = 32;
    if (just_first)
      reserve_max = 1;

    std::vector<EdgeT> res;
    auto out_edges = _temp.out_edges(v);
      auto other = std::lower_bound(out_edges.begin(), out_edges.end(), e,
          [](const EdgeT& e1, const EdgeT& e2) { return e1 < e2; });
      res.reserve(std::min<size_t>(reserve_max, out_edges.end() - other));
      double last_p = 1.0;
      while ((other < out_edges.end()) && last_p > cutoff) {
        if (adjacent(e, *other)) {
          last_p = _prob.p(e, *other);
          if (bernoulli_trial(e, *other, last_p)) {
            if (just_first && !res.empty() &&
                res[0].cause_time() != other->cause_time())
              return res;
            else
              res.push_back(*other);
          }
        }
        other++;
      }
    return res;
  }

  template <class EdgeT, class AdjacencyProbT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::predecessors_vert(
      const EdgeT& e, VertexType v, bool just_first) const {
    std::vector<EdgeT> res;
    constexpr double cutoff = 1e-20;

    size_t reserve_max = 32;
    if (just_first)
      reserve_max = 1;

    auto in_edges = _temp.in_edges(v);
    auto other = std::lower_bound(in_edges.begin(), in_edges.end(), e,
        [](const EdgeT& e1, const EdgeT& e2) { return effect_lt(e1, e2); }) - 1;
    res.reserve(std::min<size_t>(reserve_max, other - in_edges.begin()));
    double last_p = 1.0;
    while ((other >= in_edges.begin()) && last_p > cutoff) {
      if (adjacent(*other, e)) {
        last_p = _prob.p(*other, e);
        if (bernoulli_trial(*other, e, last_p)) {
          if (just_first && !res.empty() &&
              res[0].cause_time() != other->cause_time())
            return res;
          else
            res.push_back(*other);
        }
      }
      other--;
    }
    return res;
  }
}  // namespace dag
