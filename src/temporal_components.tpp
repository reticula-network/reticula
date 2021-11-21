#include <unordered_set>

namespace dag {
  template <temporal_edge EdgeT,
          template<typename> class NodeEstimatorT,
          template<typename> class EdgeEstimatorT>
  void
  temporal_component<EdgeT, NodeEstimatorT, EdgeEstimatorT>::insert(
      const EdgeT& e,
      const std::vector<VertexType>& nodes) {
    _edge_set.insert(e);

    for (auto&& n: nodes)
      _node_set.insert(n);

    _min_cause_time = std::min(_min_cause_time, e.cause_time());
    _max_cause_time = std::max(_max_cause_time, e.cause_time());
    _min_effect_time = std::min(_min_effect_time, e.effect_time());
    _max_effect_time = std::max(_max_effect_time, e.effect_time());
  }

  template <temporal_edge EdgeT,
          template<typename> class NodeEstimatorT,
          template<typename> class EdgeEstimatorT>
  void
  temporal_component<EdgeT, NodeEstimatorT, EdgeEstimatorT>::merge(
      const temporal_component<EdgeT, NodeEstimatorT, EdgeEstimatorT>& other) {
    _node_set.merge(other.node_set());
    _edge_set.merge(other.edge_set());

    auto c_lt = other.cause_lifetime();
    _min_cause_time = std::min(_min_cause_time, c_lt.first);
    _max_cause_time = std::max(_max_cause_time, c_lt.second);

    auto e_lt = other.effect_lifetime();
    _min_effect_time = std::min(_min_effect_time, e_lt.first);
    _max_effect_time = std::max(_max_effect_time, e_lt.second);
  }
}  // namespace dag
