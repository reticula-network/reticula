#include <queue>
#include <random>

#include "../include/dag/adjacency_prob.hpp"

namespace dag {
  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::implicit_event_graph(
      const std::vector<EdgeT>& events,
      const AdjacencyProbT& prob,
      size_t seed) :
    _seed(seed), _temp(events), _prob(prob) {}

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::implicit_event_graph(
      const network<EdgeT>& temp,
      const AdjacencyProbT& prob,
      size_t seed) :
    _seed(seed), _temp(temp), _prob(prob) {}


  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  AdjacencyProbT
  implicit_event_graph<EdgeT, AdjacencyProbT>::adjacency_prob() const {
    return _prob;
  }

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  size_t
  implicit_event_graph<EdgeT, AdjacencyProbT>::seed() const {
    return _seed;
  }

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  const std::vector<EdgeT>&
  implicit_event_graph<EdgeT, AdjacencyProbT>::events_cause() const {
    return _temp.edges_cause();
  }

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  const std::vector<EdgeT>&
  implicit_event_graph<EdgeT, AdjacencyProbT>::events_effect() const {
    return _temp.edges_effect();
  }

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  implicit_event_graph<EdgeT, AdjacencyProbT>::time_window() const {
    if (_temp.edges().empty())
      return std::make_pair(0, 0);
    else
      return std::make_pair(
          _temp.edges().front().cause_time(),
          _temp.edges().back().cause_time());
  }

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
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
          res.begin(), res.end(),
          std::back_inserter(pred));
      std::inplace_merge(pred.begin(), pred.begin()+middle_offset, pred.end());
    }

    pred.erase(std::unique(pred.begin(), pred.end()), pred.end());

    return pred;
  }

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
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

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::successors_vert(
      const EdgeT& e, VertexType v, bool just_first) const {
    constexpr double cutoff = 1e-20;

    size_t reserve_max = 32;
    if (just_first)
      reserve_max = 1;

    std::vector<EdgeT> res;
    auto out_edges_it = _temp.out_edges().find(v);
    if (out_edges_it == _temp.out_edges().end())
      return res;

    auto other = std::lower_bound(
        out_edges_it->second.begin(), out_edges_it->second.end(), e,
        [](const EdgeT& e1, const EdgeT& e2) { return e1 < e2; });
    res.reserve(std::min<size_t>(
          reserve_max, out_edges_it->second.end() - other));
    double last_p = 1.0;
    while ((other < out_edges_it->second.end()) && last_p > cutoff) {
      if (adjacent(e, *other)) {
        last_p = _prob.p(e, *other);
        if (adjacency_prob::bernoulli_trial(e, *other, last_p, _seed)) {
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

  template <temporal_edge EdgeT, adjacency_prob::adjacency_prob AdjacencyProbT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjacencyProbT>::predecessors_vert(
      const EdgeT& e, VertexType v, bool just_first) const {
    constexpr double cutoff = 1e-20;

    size_t reserve_max = 32;
    if (just_first)
      reserve_max = 1;

    std::vector<EdgeT> res;
    auto in_edges_it = _temp.in_edges().find(v);
    if (in_edges_it == _temp.in_edges().end())
      return res;

    auto other = std::lower_bound(
        in_edges_it->second.begin(), in_edges_it->second.end(), e,
        [](const EdgeT& e1, const EdgeT& e2) { return effect_lt(e1, e2); });

    if (other > in_edges_it->second.begin())
      other--;

    res.reserve(std::min<size_t>(
          reserve_max, other - in_edges_it->second.begin()));
    double last_p = 1.0;
    while (other < in_edges_it->second.end() &&
            other >= in_edges_it->second.begin() &&
            last_p > cutoff) {
      if (adjacent(*other, e)) {
        last_p = _prob.p(*other, e);
        if (adjacency_prob::bernoulli_trial(*other, e, last_p, _seed)) {
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
