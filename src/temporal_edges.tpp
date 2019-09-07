#include <functional>
#include <istream>
#include <ostream>

#include "../include/dag/utils.hpp"

namespace std {
  template<typename VertT, typename TimeT>
  struct hash<dag::undirected_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(const dag::undirected_temporal_edge<VertT, TimeT>& e) const {
      return dag::utils::combine_hash(
          dag::utils::unordered_hash(e.v1, e.v2), e.time);
    }
  };

  template<typename VertT, typename TimeT>
  struct hash<dag::directed_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(const dag::directed_temporal_edge<VertT, TimeT>& e) const {
      return dag::utils::combine_hash(
          dag::utils::combine_hash(std::hash<VertT>{}(e.v1), e.v2), e.time);
    }
  };

  template<typename VertT, typename TimeT>
  struct hash<dag::directed_delayed_temporal_edge<VertT, TimeT>> {
    size_t
    operator()(const dag::directed_delayed_temporal_edge<VertT, TimeT>& e) const {
      return dag::utils::combine_hash(
              dag::utils::combine_hash(
                dag::utils::combine_hash(std::hash<VertT>{}(e.v1), e.v2),
                e.time),
              e.delay);
    }
  };
}

namespace dag {
  // undirected temporal edge IO:
  template <typename VertT, typename TimeT>
  std::ostream& operator<<(std::ostream& os,
      const undirected_temporal_edge<VertT, TimeT>& e) {
    return os <<  e.v1 << " " << e.v2 << " " << e.time;
  }

  template <typename VertT, typename TimeT>
  std::istream& operator>>(std::istream& is,
      undirected_temporal_edge<VertT, TimeT>& e) {
    return is >> e.v1 >> e.v2 >> e.time;
  }

  // directed temporal edge IO:
  template <typename VertT, typename TimeT>
  std::ostream& operator<<(std::ostream& os,
      const directed_temporal_edge<VertT, TimeT>& e) {
    return os << e.v1 << " " << e.v2 << " " << e.time;
  }

  template <typename VertT, typename TimeT>
  std::istream& operator>>(std::istream& is,
      directed_temporal_edge<VertT, TimeT>& e) {
    return is >> e.v1 >> e.v2 >> e.time;
  }

  // directed delayed temporal edge IO:
  template <typename VertT, typename TimeT>
  std::ostream& operator<<(std::ostream& os,
      const directed_delayed_temporal_edge<VertT, TimeT>& e) {
    return os << e.v1 << " " << e.v2 << " " << e.time << " " << e.delay;
  }

  template <typename VertT, typename TimeT>
  std::istream& operator>>(std::istream& is,
      directed_delayed_temporal_edge<VertT, TimeT>& e) {
    return is >> e.v1 >> e.v2 >> e.time >> e.delay;
  }
}
