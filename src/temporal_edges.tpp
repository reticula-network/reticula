#include <functional>
#include <istream>
#include <ostream>

#include "../include/dag/utils.hpp"

namespace std {
  template<typename VertT, typename TimeT>
  struct hash<dag::undirected_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(
        const dag::undirected_temporal_edge<VertT, TimeT>& e) const {
      return dag::utils::combine_hash(
          dag::utils::unordered_hash(e.v1, e.v2), e.time);
    }
  };

  template<typename VertT, typename TimeT>
  struct hash<dag::directed_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(
        const dag::directed_temporal_edge<VertT, TimeT>& e) const {
      return dag::utils::combine_hash(
          dag::utils::combine_hash(std::hash<VertT>{}(e.v1), e.v2), e.time);
    }
  };

  template<typename VertT, typename TimeT>
  struct hash<dag::directed_delayed_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(
        const dag::directed_delayed_temporal_edge<VertT, TimeT>& e) const {
      return dag::utils::combine_hash(
              dag::utils::combine_hash(
                dag::utils::combine_hash(std::hash<VertT>{}(e.v1), e.v2),
                e.time),
              e.delay);
    }
  };
}  // namespace std


namespace hll {
  template<typename VertT, typename TimeT>
  struct hash<dag::undirected_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(
        const dag::undirected_temporal_edge<VertT, TimeT>& e) const {
      return
        dag::utils::combine_hash<TimeT, hll::hash>(
            dag::utils::unordered_hash<VertT, VertT, hll::hash>(e.v1, e.v2),
          e.time);
    }
  };

  template<typename VertT, typename TimeT>
  struct hash<dag::directed_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(
        const dag::directed_temporal_edge<VertT, TimeT>& e) const {
      return
        dag::utils::combine_hash<TimeT, hll::hash>(
          dag::utils::combine_hash<VertT, VertT, hll::hash>(
              hll::hash<VertT>{}(e.v1),
            e.v2),
          e.time);
    }
  };

  template<typename VertT, typename TimeT>
  struct hash<dag::directed_delayed_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(
        const dag::directed_delayed_temporal_edge<VertT, TimeT>& e) const {
      return
        dag::utils::combine_hash<TimeT, hll::hash>(
            dag::utils::combine_hash<TimeT, hll::hash>(
                dag::utils::combine_hash<VertT, hll::hash>(
                  hll::hash<VertT>{}(e.v1),
                  e.v2),
                e.time),
            e.delay);
    }
  };
}  // namespace hll
