#include <functional>
#include <istream>
#include <ostream>

#include "../include/reticula/utils.hpp"

// Hashing rules for edges
namespace std {
  template<reticula::network_vertex VertexType>
  struct hash<reticula::directed_edge<VertexType>> {
    std::size_t
    operator()(const reticula::directed_edge<VertexType>& e) const {
      return reticula::utils::combine_hash<VertexType, reticula::hash>(
          reticula::hash<VertexType>{}(e._tail), e._head);
    }
  };

  template<reticula::network_vertex VertexType>
  struct hash<reticula::undirected_edge<VertexType>> {
    std::size_t operator()(
        const reticula::undirected_edge<VertexType>& e) const {
      return reticula::utils::combine_hash<VertexType, reticula::hash>(
          reticula::hash<VertexType>{}(e._v1), e._v2);
    }
  };
}  // namespace std

// HLL hashing rules for edges
namespace hll {
  template<reticula::network_vertex VertexType>
  struct hash<reticula::directed_edge<VertexType>> {
    uint64_t
    operator()(const reticula::directed_edge<
        VertexType>& e, std::uint64_t seed) const {
      return hll::hash<std::size_t>{}(
          std::hash<reticula::directed_edge<VertexType>>{}(e), seed);
    }
  };

  template<reticula::network_vertex VertexType>
  struct hash<reticula::undirected_edge<VertexType>> {
    uint64_t
    operator()(const reticula::undirected_edge<
        VertexType>& e, std::uint64_t seed) const {
      return hll::hash<std::size_t>{}(
          std::hash<reticula::undirected_edge<VertexType>>{}(e), seed);
    }
  };
}  // namespace hll

namespace reticula {
  // properties of directed edge:

  template <network_vertex VertexType>
  directed_edge<VertexType>::directed_edge(VertexType tail, VertexType head)
      : _tail(tail), _head(head) {}

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_out_incident(const VertexType& vert) const {
    return (_tail == vert);
  }

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_in_incident(const VertexType& vert) const  {
    return (_head == vert);
  }

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_incident(const VertexType& vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_edge<VertexType>::mutator_verts() const {
    return {_tail};
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_edge<VertexType>::mutated_verts() const {
    return {_head};
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_edge<VertexType>::incident_verts() const {
    if (_tail == _head)
      return {_tail};
    else
      return {_tail, _head};
  }

  template <network_vertex VertexType>
  VertexType directed_edge<VertexType>::tail() const {
    return _tail;
  }

  template <network_vertex VertexType>
  VertexType directed_edge<VertexType>::head() const {
    return _head;
  }

  template <network_vertex VertexType>
  bool effect_lt(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return std::make_tuple(a._head, a._tail) <
      std::make_tuple(b._head, b._tail);
  }

  template <network_vertex VertexType>
  bool adjacent(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a._head == b._tail);
  }

  template <network_vertex VertexType>
  std::ostream& operator<<(
      std::ostream &os,
      const directed_edge<VertexType>& e) {
    return os << e._tail << " " << e._head;
  }

  template <network_vertex VertexType>
  std::istream& operator>>(
      std::istream &is,
      directed_edge<VertexType>& e) {
    return is >> e._tail >> e._head;
  }


  // properties of undirected edge:

  template <network_vertex VertexType>
  undirected_edge<VertexType>::undirected_edge(VertexType v1, VertexType v2) {
    std::tie(_v1, _v2) = std::minmax(v1, v2);
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_incident(
      const VertexType& vert) const {
    return (_v1 == vert || _v2 == vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_in_incident(
      const VertexType& vert) const {
    return (_v1 == vert || _v2 == vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_out_incident(
      const VertexType& vert) const {
    return (_v1 == vert || _v2 == vert);
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  undirected_edge<VertexType>::mutator_verts() const {
    if (_v1 == _v2)
      return {_v1};
    else
      return {_v1, _v2};
  }

  template <network_vertex vertextype>
  std::vector<vertextype>
  undirected_edge<vertextype>::mutated_verts() const {
    if (_v1 == _v2)
      return {_v1};
    else
      return {_v1, _v2};
  }

  template <network_vertex vertextype>
  std::vector<vertextype>
  undirected_edge<vertextype>::incident_verts() const {
    if (_v1 == _v2)
      return {_v1};
    else
      return {_v1, _v2};
  }

  template <network_vertex VertexType>
  bool effect_lt(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
      return a < b;
  }

  template <network_vertex VertexType>
  bool adjacent(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
    return (a._v1 == b._v1 ||
            a._v1 == b._v2 ||
            a._v2 == b._v1 ||
            a._v2 == b._v2);
  }

  template <network_vertex VertexType>
  requires output_streamable<VertexType>
  std::ostream& operator<<(
      std::ostream &os,
      const undirected_edge<VertexType>& e) {
    return os << e._v1 << " " << e._v2;
  }

  template <network_vertex VertexType>
  requires input_streamable<VertexType>
  std::istream& operator>>(std::istream &is,
      undirected_edge<VertexType>& e) {
    return is >> e._v1 >> e._v2;
  }
}  // namespace reticula
