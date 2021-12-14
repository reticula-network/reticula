#include <functional>
#include <istream>
#include <ostream>

#include "../include/dag/utils.hpp"

// Hashing rules for edges
namespace std {
  template<dag::network_vertex VertexType>
  struct hash<dag::directed_edge<VertexType>> {
    size_t
    operator()(const dag::directed_edge<VertexType>& e) const {
      return dag::utils::combine_hash<VertexType, hash>(
          std::hash<VertexType>{}(e.v1), e.v2);
    }
  };

  template<dag::network_vertex VertexType>
  struct hash<dag::undirected_edge<VertexType>> {
    size_t operator()(const dag::undirected_edge<VertexType>& e) const {
      return dag::utils::unordered_hash<VertexType, VertexType, hash>(
          e.v1, e.v2);
    }
  };
}  // namespace std

// HLL hashing rules for edges
namespace hll {
  template<dag::network_vertex VertexType>
  struct hash<dag::directed_edge<VertexType>> {
    uint64_t
    operator()(const dag::directed_edge<VertexType>& e, uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<dag::directed_edge<VertexType>>{}(e), seed);
    }
  };

  template<dag::network_vertex VertexType>
  struct hash<dag::undirected_edge<VertexType>> {
    uint64_t
    operator()(const dag::undirected_edge<VertexType>& e, uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<dag::directed_edge<VertexType>>{}(e), seed);
    }
  };
}  // namespace hll

namespace dag {
  // properties of directed edge:

  template <network_vertex VertexType>
  directed_edge<VertexType>::directed_edge(VertexType v1, VertexType v2)
      : v1(v1), v2(v2) {}

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_out_incident(const VertexType& vert) const {
    return (v1 == vert);
  }

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_in_incident(const VertexType& vert) const  {
    return (v2 == vert);
  }

  template <network_vertex VertexType>
  inline bool
  directed_edge<VertexType>::is_incident(const VertexType& vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_edge<VertexType>::mutator_verts() const {
    return {v1};
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  directed_edge<VertexType>::mutated_verts() const {
    return {v2};
  }

  template <network_vertex VertexType>
  bool operator!=(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return !(a == b);
  }

  template <network_vertex VertexType>
  bool operator==(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a.cause_comp_tuple() == b.cause_comp_tuple());
  }

  template <network_vertex VertexType>
  bool operator<(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a.cause_comp_tuple() < b.cause_comp_tuple());
  }

  template <network_vertex VertexType>
  bool effect_lt(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a.effect_comp_tuple() < b.effect_comp_tuple());
  }

  template <network_vertex VertexType>
  bool adjacent(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a.v2 == b.v1);
  }

  template <network_vertex VertexType>
  inline std::tuple<VertexType, VertexType>
  directed_edge<VertexType>::cause_comp_tuple() const {
    return std::make_tuple(v1, v2);
  }


  template <network_vertex VertexType>
  inline std::tuple<VertexType, VertexType>
  directed_edge<VertexType>::effect_comp_tuple() const {
      return std::make_tuple(v2, v1);
  }

  template <network_vertex VertexType>
  std::ostream& operator<<(
      std::ostream &os,
      const dag::directed_edge<VertexType>& e) {
    return os << e.v1 << " " << e.v2;
  }

  template <network_vertex VertexType>
  std::istream& operator>>(
      std::istream &is,
      dag::directed_edge<VertexType>& e) {
    return is >> e.v1 >> e.v2;
  }


  // properties of undirected edge:

  template <network_vertex VertexType>
  undirected_edge<VertexType>::undirected_edge(VertexType v1, VertexType v2)
    : v1(v1), v2(v2) {}

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_incident(
      const VertexType& vert) const {
    return (v1 == vert || v2 == vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_in_incident(
      const VertexType& vert) const {
    return (v1 == vert || v2 == vert);
  }

  template <network_vertex VertexType>
  inline bool undirected_edge<VertexType>::is_out_incident(
      const VertexType& vert) const {
    return (v1 == vert || v2 == vert);
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  undirected_edge<VertexType>::mutator_verts() const {
    return {v1, v2};
  }

  template <network_vertex VertexType>
  std::vector<VertexType>
  undirected_edge<VertexType>::mutated_verts() const {
    return {v1, v2};
  }

  template <network_vertex VertexType>
  bool operator!=(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
    return !(a == b);
  }

  template <network_vertex VertexType>
  bool operator==(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
    return (a.comp_tuple() == b.comp_tuple());
  }

  template <network_vertex VertexType>
  bool operator<(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
      return (a.comp_tuple() < b.comp_tuple());
  }

  template <network_vertex VertexType>
  bool effect_lt(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
      return (a.comp_tuple() < b.comp_tuple());
  }

  template <network_vertex VertexType>
  bool adjacent(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
    return (a.v1 == b.v1 ||
            a.v1 == b.v2 ||
            a.v2 == b.v1 ||
            a.v2 == b.v2);
  }

  template <network_vertex VertexType>
  inline std::tuple<VertexType, VertexType>
  undirected_edge<VertexType>::comp_tuple() const {
    return std::make_tuple(std::min(v1, v2), std::max(v1, v2));
  }

  template <network_vertex VertexType>
  requires output_streamable<VertexType>
  std::ostream& operator<<(
      std::ostream &os,
      const dag::undirected_edge<VertexType>& e) {
    return os << e.v1 << " " << e.v2;
  }

  template <network_vertex VertexType>
  requires input_streamable<VertexType>
  std::istream& operator>>(std::istream &is,
      dag::undirected_edge<VertexType>& e) {
    return is >> e.v1 >> e.v2;
  }
}  // namespace dag
