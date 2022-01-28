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
      return dag::utils::combine_hash<TimeType, dag::hash>(
          dag::utils::combine_hash<VertexType, dag::hash>(
            dag::hash<VertexType>{}(e._v1), e._v2), e._time);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_temporal_edge<VertexType, TimeType>> {
    size_t
    operator()(
        const dag::directed_temporal_edge<VertexType, TimeType>& e) const {
      return dag::utils::combine_hash<TimeType, dag::hash>(
          dag::utils::combine_hash<VertexType, dag::hash>(
            dag::hash<VertexType>{}(e._tail), e._head), e._time);
    }
  };

  template<dag::network_vertex VertexType, typename TimeType>
  struct hash<dag::directed_delayed_temporal_edge<VertexType, TimeType>> {
    size_t
    operator()(
        const
        dag::directed_delayed_temporal_edge<VertexType, TimeType>& e) const {
      return dag::utils::combine_hash<TimeType, dag::hash>(
              dag::utils::combine_hash<TimeType, dag::hash>(
                dag::utils::combine_hash<VertexType, dag::hash>(
                  dag::hash<VertexType>{}(e._tail), e._head),
                e._cause_time),
              e._effect_time);
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
    VertexType tail, VertexType head, TimeType time)
    : _time(time), _tail(tail), _head(head) {}

  template <network_vertex VertexType, typename TimeType>
  directed_edge<VertexType>
  directed_temporal_edge<VertexType, TimeType>::static_projection() const {
    return directed_edge<VertexType>(_tail, _head);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_temporal_edge<VertexType, TimeType>::effect_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_temporal_edge<VertexType, TimeType>::cause_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_edge<VertexType, TimeType>::is_out_incident(
      const VertexType& vert) const {
    return (_tail == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_edge<VertexType, TimeType>::is_in_incident(
      const VertexType& vert) const {
    return (_head == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_temporal_edge<VertexType, TimeType>::is_incident(
      const VertexType& vert) const {
    return (_tail == vert || _head == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_edge<VertexType, TimeType>::mutator_verts() const {
    return {_tail};
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_edge<VertexType, TimeType>::mutated_verts() const {
    return {_head};
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_temporal_edge<VertexType, TimeType>::incident_verts() const {
    if (_tail == _head)
      return {_tail};
    else
      return {_tail, _head};
  }

  template <network_vertex VertexType, typename TimeType>
  VertexType directed_temporal_edge<VertexType, TimeType>::tail() const {
    return _tail;
  }

  template <network_vertex VertexType, typename TimeType>
  VertexType directed_temporal_edge<VertexType, TimeType>::head() const {
    return _head;
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_temporal_edge<VertexType, TimeType>& a,
      const directed_temporal_edge<VertexType, TimeType>& b) {
    return ((b._time > a._time) && (a._head == b._tail));
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_temporal_edge<VertexType, TimeType>& a,
      const directed_temporal_edge<VertexType, TimeType>& b) {
    return std::make_tuple(a._time, a._head, a._tail) <
      std::make_tuple(b._time, b._head, b._tail);
  }

  template <network_vertex VertexType, typename TimeType>
  std::ostream& operator<<(std::ostream& os,
      const dag::directed_temporal_edge<VertexType, TimeType>& e) {
    return os << e._tail << " " << e._head << " " << e._time;
  }

  template <network_vertex VertexType, typename TimeType>
  std::istream& operator>>(
      std::istream& is,
      dag::directed_temporal_edge<VertexType, TimeType>& e) {
    return is >> e._tail >> e._head >> e._time;
  }

  // properties of directed delayed temporal edges:

  template <network_vertex VertexType, typename TimeType>
  directed_delayed_temporal_edge<VertexType, TimeType>::
    directed_delayed_temporal_edge(
      VertexType tail, VertexType head,
      TimeType cause_time, TimeType effect_time) :
      _cause_time(cause_time), _effect_time(effect_time),
      _tail(tail), _head(head) {
    if (_effect_time < _cause_time)
      throw std::invalid_argument("directed_delayed_temporal_edge cannot have a "
          "cause_time larger than effect_time");
  }

  template <network_vertex VertexType, typename TimeType>
  directed_edge<VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::
      static_projection() const {
    return directed_edge<VertexType>(_tail, _head);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_edge<VertexType, TimeType>::effect_time() const {
    return _effect_time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType
  directed_delayed_temporal_edge<VertexType, TimeType>::cause_time() const {
    return _cause_time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_edge<VertexType, TimeType>::is_out_incident(
      const VertexType& vert) const {
    return (_tail == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_edge<VertexType, TimeType>::is_in_incident(
      const VertexType& vert) const {
    return (_head == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool directed_delayed_temporal_edge<VertexType, TimeType>::is_incident(
      const VertexType& vert) const {
    return (_tail == vert || _head == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::mutator_verts() const {
    return {_tail};
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::mutated_verts() const {
    return {_head};
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  directed_delayed_temporal_edge<VertexType, TimeType>::incident_verts() const {
    if (_tail == _head)
      return {_tail};
    else
      return {_tail, _head};
  }

  template <network_vertex VertexType, typename TimeType>
  VertexType
  directed_delayed_temporal_edge<VertexType, TimeType>::tail() const {
    return _tail;
  }

  template <network_vertex VertexType, typename TimeType>
  VertexType
  directed_delayed_temporal_edge<VertexType, TimeType>::head() const {
    return _head;
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const directed_delayed_temporal_edge<VertexType, TimeType>& a,
      const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
    return (b._cause_time > a._effect_time) && (a._head == b._tail);
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const directed_delayed_temporal_edge<VertexType, TimeType>& a,
      const directed_delayed_temporal_edge<VertexType, TimeType>& b) {
    return std::make_tuple(a._effect_time, a._cause_time, a._head, a._tail) <
      std::make_tuple(b._effect_time, b._cause_time, b._head, b._tail);
  }

  template <network_vertex VertexType, typename TimeType>
  std::ostream& operator<<(std::ostream& os,
      const dag::directed_delayed_temporal_edge<VertexType, TimeType>& e) {
    return os << e._tail << " " << e._head << " "
      << e._cause_time << " " << e._effect_time;
  }

  template <network_vertex VertexType, typename TimeType>
  std::istream& operator>>(
      std::istream& is,
      dag::directed_delayed_temporal_edge<VertexType, TimeType>& e) {
    return is >> e._tail >> e._head >> e._cause_time >> e._effect_time;
  }


  // properties of undirected temporal edges:

  template <network_vertex VertexType, typename TimeType>
  undirected_temporal_edge<VertexType, TimeType>::undirected_temporal_edge(
    VertexType v1, VertexType v2, TimeType time) : _time(time) {
    std::tie(_v1, _v2) = std::minmax(v1, v2);
  }

  template <network_vertex VertexType, typename TimeType>
  undirected_edge<VertexType>
  undirected_temporal_edge<VertexType, TimeType>::static_projection() const {
    return undirected_edge<VertexType>(_v1, _v2);
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType undirected_temporal_edge<VertexType, TimeType>::effect_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  TimeType undirected_temporal_edge<VertexType, TimeType>::cause_time() const {
    return _time;
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_edge<VertexType, TimeType>::is_incident(
      const VertexType& vert) const {
    return (_v1 == vert || _v2 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_edge<VertexType, TimeType>::is_in_incident(
      const VertexType& vert) const {
    return (_v1 == vert || _v2 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  bool undirected_temporal_edge<VertexType, TimeType>::is_out_incident(
      const VertexType& vert) const {
    return (_v1 == vert || _v2 == vert);
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  undirected_temporal_edge<VertexType, TimeType>::mutator_verts() const {
    if (_v1 == _v2)
      return {_v1};
    else
      return {_v1, _v2};
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  undirected_temporal_edge<VertexType, TimeType>::mutated_verts() const {
    if (_v1 == _v2)
      return {_v1};
    else
      return {_v1, _v2};
  }

  template <network_vertex VertexType, typename TimeType>
  std::vector<VertexType>
  undirected_temporal_edge<VertexType, TimeType>::incident_verts() const {
    if (_v1 == _v2)
      return {_v1};
    else
      return {_v1, _v2};
  }

  template <network_vertex VertexType, typename TimeType>
  bool adjacent(
      const undirected_temporal_edge<VertexType, TimeType>& a,
      const undirected_temporal_edge<VertexType, TimeType>& b) {
    return ((b._time > a._time) &&
              (a._v1 == b._v1 ||
                a._v1 == b._v2 ||
                a._v2 == b._v1 ||
                a._v2 == b._v2));
  }

  template <network_vertex VertexType, typename TimeType>
  bool effect_lt(
      const undirected_temporal_edge<VertexType, TimeType>& a,
      const undirected_temporal_edge<VertexType, TimeType>& b) {
    return a < b;
  }

  template <network_vertex VertexType, typename TimeType>
  requires output_streamable<VertexType> && output_streamable<TimeType>
  std::ostream& operator<<(
      std::ostream& os,
      const dag::undirected_temporal_edge<VertexType, TimeType>& e) {
    return os << e._v1 << " " << e._v2 << " " << e._time;
  }

  template <network_vertex VertexType, typename TimeType>
  requires input_streamable<VertexType> && input_streamable<TimeType>
  std::istream& operator>>(
      std::istream& is,
      dag::undirected_temporal_edge<VertexType, TimeType>& e) {
    return is >> e._v1 >> e._v2 >> e._time;
  }
}  // namespace dag
