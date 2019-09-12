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
      std::vector<EdgeT> events, const AdjacencyProbT& prob, size_t seed) :
    seed(seed), _topo(events), prob(prob) {
    std::sort(_topo.begin(), _topo.end());
    _topo.erase(std::unique(_topo.begin(), _topo.end()), _topo.end());
    _topo.shrink_to_fit();

    for (const auto& e: _topo) {
      for (auto&& v: e.mutator_verts())
        inc_out_map[v].push_back(e);
      for (auto&& v: e.mutated_verts())
        inc_in_map[v].push_back(e);
    }

    for (auto&& p: inc_in_map) {
      std::sort(p.second.begin(), p.second.end());
      p.second.erase(std::unique(p.second.begin(), p.second.end()),
          p.second.end());
      p.second.shrink_to_fit();

      std::sort(p.second.begin(), p.second.end(),
        [](const EdgeT& e1, const EdgeT& e2) {
          return std::make_pair(e1.effect_time(), e1) <
                    std::make_pair(e2.effect_time(), e2);
        });
    }

    for (auto&& p: inc_out_map) {
      std::sort(p.second.begin(), p.second.end());
      p.second.erase(std::unique(p.second.begin(), p.second.end()),
          p.second.end());
      p.second.shrink_to_fit();

      std::sort(p.second.begin(), p.second.end(),
          [](const EdgeT& e1, const EdgeT& e2) {
          return std::make_pair(e1.cause_time(), e1) <
            std::make_pair(e2.cause_time(), e2);
          });
    }
  }

  template <class EdgeT, class AdjacencyProbT>
  const std::vector<EdgeT>&
  implicit_event_graph<EdgeT, AdjacencyProbT>::topo() const { return _topo; }

  template <class EdgeT, class AdjacencyProbT>
  size_t
  implicit_event_graph<EdgeT, AdjacencyProbT>::event_count() const {
    return _topo.size();
  }

  template <class EdgeT, class AdjacencyProbT>
  size_t
  implicit_event_graph<EdgeT, AdjacencyProbT>::node_count() const {
    return inc_in_map.size();
  }

  template <class EdgeT, class AdjacencyProbT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  implicit_event_graph<EdgeT, AdjacencyProbT>::time_window() const {
    if (_topo.empty())
      return std::make_pair(0, 0);
    else
      return std::make_pair(
          _topo.front().cause_time(),
          _topo.back().cause_time());
  }

  template <class EdgeT, class AdjacencyProbT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::predecessors(
      const EdgeT& e, bool just_first) const {
    std::vector<EdgeT> pred;

    pred.reserve(e.mutator_verts().size());

    for (auto&& v : e.mutator_verts()) {
      size_t middle_offset = pred.size();
      auto res = predecessors_vert(e, v, just_first ||
          (enable_deterministic_shortcut && is_prob_deterministic));
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
      auto res = successors_vert(e, v, just_first ||
          (enable_deterministic_shortcut && is_prob_deterministic));
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
      size_t dag_edge_seed = utils::combine_hash(seed, a);
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
    auto inc = inc_out_map.find(v);
    if (inc != inc_out_map.end()) {
      auto other = std::lower_bound(inc->second.begin(), inc->second.end(), e,
          [](const EdgeT& e1, const EdgeT& e2) {
          return std::make_pair(e1.cause_time(), e1) <
                  std::make_pair(e2.cause_time(), e2);
          });
      res.reserve(std::min<size_t>(reserve_max, inc->second.end() - other));
      double last_p = 1.0;
      while ((other < inc->second.end()) && last_p > cutoff) {
        if (adjacent(e, *other)) {
          last_p = prob.p(e, *other);
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

    auto inc = inc_in_map.find(v);
    if (inc != inc_in_map.end()) {
      auto other = std::lower_bound(inc->second.begin(), inc->second.end(), e,
          [](const EdgeT& e1, const EdgeT& e2) {
          return std::make_pair(e1.effect_time(), e1) <
          std::make_pair(e2.effect_time(), e2);
          }) - 1;
      res.reserve(std::min<size_t>(reserve_max, other - inc->second.begin()));
      double last_p = 1.0;
      while ((other >= inc->second.begin()) && last_p > cutoff) {
        if (adjacent(*other, e)) {
          last_p = prob.p(*other, e);
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
    }
    return res;
  }
}  // namespace dag
