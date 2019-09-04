#include <queue>
#include <random>
#include <cmath>

namespace dag {
  template <class EdgeT>
  class implicit_event_graph {
    public:

    using TimeType = typename EdgeT::TimeType;
    using VertexType = typename EdgeT::VertexType;

    implicit_event_graph() = default;
    implicit_event_graph(std::vector<EdgeT> events,
        TimeType expected_dt,
        std::function<double(const EdgeT& a, const EdgeT& b, TimeType dt)> prob,
        bool deterministic,
        size_t seed) :
      seed(seed), _topo(events), _expected_dt(expected_dt),
      _deterministic(deterministic), prob(prob) {

      std::sort(_topo.begin(), _topo.end());
      auto last = std::unique(_topo.begin(), _topo.end());
      _topo.erase(last, _topo.end());
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
            return std::make_pair(e1.time, e1) <
              std::make_pair(e2.time, e2);
            });
      }
    }

    const std::vector<EdgeT>& topo() const { return _topo; }
    TimeType expected_dt() const { return _expected_dt; }
    bool deterministic() const { return _deterministic; }

    size_t event_count() const { return _topo.size(); }
    size_t node_count() const { return inc_in_map.size(); }

    std::pair<TimeType, TimeType> time_window() {
      if (_topo.empty())
        return std::make_pair(0, 0);
      else
        return std::make_pair(_topo.front().time, _topo.back().time);
    }

    std::vector<EdgeT>
    predecessors(const EdgeT& e, bool just_first=false) const {
      std::vector<EdgeT> pred;

      pred.reserve(e.mutator_verts().size());

      for (auto&& v : e.mutator_verts()) {
        size_t middle_offset = pred.size();
        auto res = predecessors_vert(e, v, just_first ||
            (enable_deterministic_shortcut && _deterministic));
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

    std::vector<EdgeT> successors(const EdgeT& e, bool just_first=false) const {
      std::vector<EdgeT> succ;

      succ.reserve(e.mutated_verts().size());

      for (auto&& v : e.mutated_verts()) {
        size_t middle_offset = succ.size();
        auto res = successors_vert(e, v, just_first ||
            (enable_deterministic_shortcut && _deterministic));
        succ.reserve(succ.size()+res.size());
        std::sort(res.begin(), res.end());
        std::copy(
            res.begin(), res.end(),
            std::back_inserter(succ));
        std::inplace_merge(succ.begin(), succ.begin()+middle_offset, succ.end());
      }

      succ.erase(std::unique(succ.begin(), succ.end()), succ.end());

      return succ;
    }


    void remove_events(const std::unordered_set<EdgeT>& events) {
      for (auto&& inc_map: {inc_in_map, inc_out_map})
        for (auto&& p: inc_map)
          p.second.erase(
              std::remove_if(p.second.begin(), p.second.end(),
                [&events](const EdgeT& e) {
                return events.find(e) != events.end();
                }),
              p.second.end());
    }

    private:

    size_t seed;
    std::vector<EdgeT> _topo;
    std::unordered_map<VertexType, std::vector<EdgeT>> inc_in_map, inc_out_map;
    TimeType _expected_dt;
    bool _deterministic;

    std::function<double(const EdgeT& a, const EdgeT& b, TimeType dt)> prob;

    bool bernoulli_trial(const EdgeT& a, const EdgeT& b, double p) const {
      if (p == 1)
        return true;
      else if (p == 0)
        return false;
      else {
        size_t dag_edge_seed = dag::utils::combine_hash(seed, a);
        dag_edge_seed = dag::utils::combine_hash(dag_edge_seed, b);

        std::mt19937_64 gen(dag_edge_seed);
        std::bernoulli_distribution dist(p);
        return dist(gen);
      }
    }

    std::vector<EdgeT>
    successors_vert(const EdgeT& e, VertexType v, bool just_first) const {
      constexpr double cutoff = 1e-20;

      size_t reserve_max = 32;
      if (just_first)
        reserve_max = 1;

      std::vector<EdgeT> res;
      auto inc = inc_out_map.find(v);
      if (inc != inc_out_map.end()) {
        auto other = std::lower_bound(inc->second.begin(), inc->second.end(), e,
            [](const EdgeT& e1, const EdgeT& e2) {
              return std::make_pair(e1.time, e1) < std::make_pair(e2.time, e2);
            });
        res.reserve(std::min<size_t>(reserve_max, inc->second.end() - other));
        double last_p = 1.0;
        while ((other < inc->second.end()) && last_p > cutoff) {
          if (e.is_adjacent_to(*other)) {
            last_p = prob(e, *other, _expected_dt);
            if (bernoulli_trial(e, *other, last_p)) {
              if (just_first && !res.empty() && res[0].time != other->time)
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


    std::vector<EdgeT>
    predecessors_vert(const EdgeT& e, VertexType v, bool just_first) const {
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
          if (other->is_adjacent_to(e)) {
            last_p = prob(*other, e, _expected_dt);
            if(bernoulli_trial(*other, e, last_p)) {
              if (just_first && !res.empty() && res[0].time != other->time)
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


    // Only enable deterministic shortcut (i.e. arXiv 1908.11831 appendix B) for
    // undirected temporal edges
    static constexpr bool enable_deterministic_shortcut = std::is_same<
      EdgeT, dag::undirected_temporal_edge<VertexType, TimeType>>::value;
  };
}
