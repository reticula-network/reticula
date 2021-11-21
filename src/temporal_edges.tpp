#include <functional>
#include <istream>
#include <ostream>

#include "../include/dag/utils.hpp"

namespace std {
  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::undirected_temporal_edge<VertexType, TimeType>> {
    size_t
    operator()(
        const dag::undirected_temporal_edge<VertexType, TimeType>& e) const {
      return dag::utils::combine_hash(
          dag::utils::unordered_hash(e.v1, e.v2), e.time);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_temporal_edge<VertexType, TimeType>> {
    size_t
    operator()(
        const dag::directed_temporal_edge<VertexType, TimeType>& e) const {
      return dag::utils::combine_hash(
        dag::utils::combine_hash(std::hash<VertexType>{}(e.v1), e.v2), e.time);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_delayed_temporal_edge<VertexType, TimeType>> {
    size_t
    operator()(
        const
        dag::directed_delayed_temporal_edge<VertexType, TimeType>& e) const {
      return dag::utils::combine_hash(
              dag::utils::combine_hash(
                dag::utils::combine_hash(std::hash<VertexType>{}(e.v1), e.v2),
                e.time),
              e.delay);
    }
  };
}  // namespace std


namespace hll {
  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::undirected_temporal_edge<VertexType, TimeType>> {
    size_t
    operator()(
        const dag::undirected_temporal_edge<VertexType, TimeType>& e,
        uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<dag::undirected_temporal_edge<VertexType, TimeType>>{}(e),
          seed);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_temporal_edge<VertexType, TimeType>> {
    uint64_t
    operator()(
        const dag::directed_temporal_edge<VertexType, TimeType>& e,
        uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<dag::directed_temporal_edge<VertexType, TimeType>>{}(e),
          seed);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_delayed_temporal_edge<VertexType, TimeType>> {
    uint64_t
    operator()(
        const dag::directed_delayed_temporal_edge<VertexType, TimeType>& e,
        uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<
            dag::directed_delayed_temporal_edge<VertexType, TimeType>>{}(e),
          seed);
    }
  };
}  // namespace hll

namespace dag {
  // properties of directed temporal edge:

  template <network_vertex VertexType, typename TimeType>
  directed_temporal_edge<VertexType, TimeType>::directed_temporal_edge(
    const VertexType v1, const VertexType v2, const TimeType time)
    : v1(v1), v2(v2), time(time) {}

  template <network_vertex VertexType, typename TimeType>
  directed_edge<VertexType>
  directed_temporal_edge<VertexType, TimeType>::static_projection() {
    return directed_edge<VertexType>(v1, v2);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_temporal_edge<VertexType, TimeType>::effect_time() const {
    return time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_temporal_edge<VertexType, TimeType>::cause_time() const {
    return time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_edge<VertexType, TimeType>::is_out_incident(
      const VertexType vert) const {
    return (v1 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_edge<VertexType, TimeType>::is_in_incident(
      const VertexType vert) const {
    return (v2 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_edge<VertexType, TimeType>::is_incident(
      const VertexType vert) const {
    return (v1 == vert || v2 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_edge<VertexType, TimeType>::mutator_verts() const {
    return {v1};
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_edge<VertexType, TimeType>::mutated_verts() const {
    return {v2};
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_temporal_edge<VertexType, TimeType>& a,
      const directed_temporal_edge<VertexType, TimeType>& b) {
    return ((b.time > a.time) && (a.v2 == b.v1));
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator!=(
        const directed_temporal_edge<VertexType, TimeType>& a,
      const directed_temporal_edge<VertexType, TimeType>& b) {
    return !(a == b);
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator==(
      const directed_temporal_edge<VertexType, TimeType>& a,
      const directed_temporal_edge<VertexType, TimeType>& b) {
    return (a.cause_comp_tuple() == b.cause_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator<(
      const directed_temporal_edge<VertexType, TimeType>& a,
      const directed_temporal_edge<VertexType, TimeType>& b) {
    return (a.cause_comp_tuple() < b.cause_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_temporal_edge<VertexType, TimeType>& a,
      const directed_temporal_edge<VertexType, TimeType>& b) {
    return (a.effect_comp_tuple() < b.effect_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  std::tuple<TimeType, VertexType, VertexType>
  directed_temporal_edge<VertexType, TimeType>::cause_comp_tuple() const {
    return std::make_tuple(time, v1, v2);
  }

  template <network_vertex VertexType, typename TimeType>
  std::tuple<TimeType, VertexType, VertexType>
  directed_temporal_edge<VertexType, TimeType>::effect_comp_tuple() const {
    return std::make_tuple(time, v2, v1);
  }

  template <network_vertex VertexType, typename TimeType>
  std::ostream& operator<<(std::ostream& os,
      const dag::directed_temporal_edge<VertexType, TimeType>& e) {
    return os << e.v1 << " " << e.v2 << " " << e.time;
  }

  template <network_vertex VertexType, typename TimeType>
  std::istream& operator>>(
      std::istream& is,
      dag::directed_temporal_edge<VertexType, TimeType>& e) {
    return is >> e.v1 >> e.v2 >> e.time;
  }

  // properties of directed delayed temporal edges:

  template <network_vertex VertexType, typename TimeType>
  directed_delayed_temporal_edge<VertexType, TimeType>::
    directed_delayed_temporal_edge(
      const VertexType v1, const VertexType v2,
      const TimeType time, const TimeType delay)
    : v1(v1), v2(v2), time(time), delay(delay) {}

  template <network_vertex VertexType, typename TimeType>
  directed_edge<VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::static_projection() {
    return directed_edge<VertexType>(v1, v2);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_edge<VertexType, TimeType>::effect_time() const {
    return time+delay;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_edge<VertexType, TimeType>::cause_time() const {
    return time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_edge<VertexType, TimeType>::is_out_incident(
      const VertexType vert) const {
    return (v1 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_edge<VertexType, TimeType>::is_in_incident(
      const VertexType vert) const {
    return (v2 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_edge<VertexType, TimeType>::is_incident(
      const VertexType vert) const {
    return (v1 == vert || v2 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::mutator_verts() const {
    return {v1};
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::mutated_verts() const {
    return {v2};
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_delayed_temporal_edge<VertexType, TimeType>& a,
      const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
    return ((b.time > a.effect_time()) && (a.v2 == b.v1));
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator!=(
      const directed_delayed_temporal_edge<VertexType, TimeType>& a,
      const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
    return !(a == b);
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator==(
      const directed_delayed_temporal_edge<VertexType, TimeType>& a,
      const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
    return (a.cause_comp_tuple() == b.cause_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator<(
      const directed_delayed_temporal_edge<VertexType, TimeType>& a,
      const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
    return (a.cause_comp_tuple() < b.cause_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_delayed_temporal_edge<VertexType, TimeType>& a,
      const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
    return (a.effect_comp_tuple() < b.effect_comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  std::tuple<TimeType, TimeType, VertexType, VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::
  cause_comp_tuple() const {
    return std::make_tuple(time, time+delay, v1, v2);
  }

  template <network_vertex VertexType, typename TimeType>
  std::tuple<TimeType, TimeType, VertexType, VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::
  effect_comp_tuple() const {
    return std::make_tuple(time+delay, time, v2, v1);
  }

  template <network_vertex VertexType, typename TimeType>
  std::ostream& operator<<(std::ostream& os,
      const dag::directed_delayed_temporal_edge<VertexType, TimeType>& e) {
    return os << e.v1 << " " << e.v2 << " " << e.time << " " << e.delay;
  }

  template <network_vertex VertexType, typename TimeType>
  std::istream& operator>>(
      std::istream& is,
      dag::directed_delayed_temporal_edge<VertexType, TimeType>& e) {
    return is >> e.v1 >> e.v2 >> e.time >> e.delay;
  }


  // properties of undirected temporal edges:

  template <network_vertex VertexType, typename TimeType>
  undirected_temporal_edge<VertexType, TimeType>::undirected_temporal_edge(
    const VertexType v1, const VertexType v2, const TimeType time)
    : v1(v1), v2(v2), time(time) {}

  template <network_vertex VertexType, typename TimeType>
  undirected_edge<VertexType>
  undirected_temporal_edge<VertexType, TimeType>::static_projection() {
    return undirected_edge<VertexType>(v1, v2);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType undirected_temporal_edge<VertexType, TimeType>::effect_time() const {
    return time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType undirected_temporal_edge<VertexType, TimeType>::cause_time() const {
    return time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_edge<VertexType, TimeType>::is_incident(
      const VertexType vert) const {
    return (v1 == vert || v2 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_edge<VertexType, TimeType>::is_in_incident(
      const VertexType vert) const {
    return (v1 == vert || v2 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_edge<VertexType, TimeType>::is_out_incident(
      const VertexType vert) const {
    return (v1 == vert || v2 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  undirected_temporal_edge<VertexType, TimeType>::mutator_verts() const {
    return {v1, v2};
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  undirected_temporal_edge<VertexType, TimeType>::mutated_verts() const {
    return {v1, v2};
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const undirected_temporal_edge<VertexType, TimeType>& a,
      const undirected_temporal_edge<VertexType, TimeType>& b) {
    return ((b.time > a.time) &&
              (a.v1 == b.v1 ||
                a.v1 == b.v2 ||
                a.v2 == b.v1 ||
                a.v2 == b.v2));
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator!=(
      const undirected_temporal_edge<VertexType, TimeType>& a,
      const undirected_temporal_edge<VertexType, TimeType>& b) {
    return !(a == b);
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator==(
      const undirected_temporal_edge<VertexType, TimeType>& a,
      const undirected_temporal_edge<VertexType, TimeType>& b) {
    return (a.comp_tuple() == b.comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool operator<(
      const undirected_temporal_edge<VertexType, TimeType>& a,
      const undirected_temporal_edge<VertexType, TimeType>& b) {
    return (a.comp_tuple() < b.comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const undirected_temporal_edge<VertexType, TimeType>& a,
      const undirected_temporal_edge<VertexType, TimeType>& b) {
    return (a.comp_tuple() < b.comp_tuple());
  }

  template <network_vertex VertexType, typename TimeType>
  std::tuple<TimeType, VertexType, VertexType>
  undirected_temporal_edge<VertexType, TimeType>::comp_tuple() const {
    return std::make_tuple(time, std::min(v1, v2), std::max(v1, v2));
  }

  template <network_vertex VertexType, typename TimeType>
  std::ostream& operator<<(
      std::ostream& os,
      const dag::undirected_temporal_edge<VertexType, TimeType>& e) {
    return os <<  e.v1 << " " << e.v2 << " " << e.time;
  }

  template <network_vertex VertexType, typename TimeType>
  std::istream& operator>>(
      std::istream& is,
      dag::undirected_temporal_edge<VertexType, TimeType>& e) {
    return is >> e.v1 >> e.v2 >> e.time;
  }
}  // namespace dag


