#include <functional>
#include <istream>
#include <ostream>

#include "../include/dag/utils.hpp"

// Hashing rules for edges
namespace std {
  template<typename VertexType>
  struct hash<dag::directed_edge<VertexType>> {
    size_t
    operator()(const dag::directed_edge<VertexType>& e) const {
      return dag::utils::combine_hash(std::hash<VertexType>{}(e.v1), e.v2);
    }
  };

  template<typename VertexType>
  struct hash<dag::undirected_edge<VertexType>> {
    size_t operator()(const dag::undirected_edge<VertexType>& e) const {
      return dag::utils::unordered_hash(e.v1, e.v2);
    }
  };
}  // namespace std

// HLL hashing rules for edges
namespace hll {
  template<typename VertexType>
  struct hash<dag::directed_edge<VertexType>> {
    uint64_t
    operator()(const dag::directed_edge<VertexType>& e, uint32_t seed) const {
      return hll::hash<size_t>{}(
          std::hash<dag::directed_edge<VertexType>>{}(e), seed);
    }
  };

  template<typename VertexType>
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

  template <typename VertexType>
  directed_edge<VertexType>::directed_edge(
      const VertexType v1,
      const VertexType v2)
      : v1(v1), v2(v2) {}

  template <typename VertexType>
  inline bool
  directed_edge<VertexType>::is_out_incident(const VertexType vert) const {
    return (v1 == vert);
  }

  template <typename VertexType>
  inline bool
  directed_edge<VertexType>::is_in_incident(const VertexType vert) const  {
    return (v2 == vert);
  }

  template <typename VertexType>
  inline bool
  directed_edge<VertexType>::is_incident(const VertexType vert) const {
    return (is_out_incident(vert) || is_in_incident(vert));
  }

  template <typename VertexType>
  std::vector<VertexType>
  directed_edge<VertexType>::mutator_verts() const {
    return {v1};
  }

  template <typename VertexType>
  std::vector<VertexType>
  directed_edge<VertexType>::mutated_verts() const {
    return {v2};
  }

  template <typename VertexType>
  bool operator!=(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return !(a == b);
  }

  template <typename VertexType>
  bool operator==(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a.cause_comp_tuple() == b.cause_comp_tuple());
  }

  template <typename VertexType>
  bool operator<(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a.cause_comp_tuple() < b.cause_comp_tuple());
  }

  template <typename VertexType>
  bool ::dag::effect_lt(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a.effect_comp_tuple() < b.effect_comp_tuple());
  }

  template <typename VertexType>
  bool adjacent(
      const directed_edge<VertexType>& a,
      const directed_edge<VertexType>& b) {
    return (a.v2 == b.v1);
  }

  template <typename VertexType>
  std::ostream& operator<<(
      std::ostream &os,
      const directed_edge<VertexType>& e) {
    return os << e.v1 << " " << e.v2;
  }

  template <typename VertexType>
  std::istream& operator>>(
      std::istream &is,
      directed_edge<VertexType>& e) {
    return is >> e.v1 >> e.v2;
  }


  template <typename VertexType>
  inline std::tuple<VertexType, VertexType>
  directed_edge<VertexType>::cause_comp_tuple() const {
    return std::make_tuple(v1, v2);
  }


  template <typename VertexType>
  inline std::tuple<VertexType, VertexType>
  directed_edge<VertexType>::effect_comp_tuple() const {
      return std::make_tuple(v2, v1);
  }


  // properties of undirected edge:

  template <typename VertexType>
  undirected_edge<VertexType>::undirected_edge(
      const VertexType v1, const VertexType v2)
    : v1(v1), v2(v2) {}

  template <typename VertexType>
  inline bool undirected_edge<VertexType>::is_incident(
      const VertexType vert) const {
    return (v1 == vert || v2 == vert);
  }

  template <typename VertexType>
  std::vector<VertexType>
  undirected_edge<VertexType>::mutator_verts() const {
    return {v1, v2};
  }

  template <typename VertexType>
  std::vector<VertexType>
  undirected_edge<VertexType>::mutated_verts() const {
    return {v1, v2};
  }

  template <typename VertexType>
  bool operator!=(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
    return !(a == b);
  }

  template <typename VertexType>
  bool operator==(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
    return (a.comp_tuple() == b.comp_tuple());
  }

  template <typename VertexType>
  bool operator<(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
      return (a.comp_tuple() < b.comp_tuple());
  }

  template <typename VertexType>
  bool ::dag::effect_lt(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
      return (a.comp_tuple() < b.comp_tuple());
  }

  template <typename VertexType>
  bool adjacent(
      const undirected_edge<VertexType>& a,
      const undirected_edge<VertexType>& b) {
    return (a.v1 == b.v1 ||
            a.v1 == b.v2 ||
            a.v2 == b.v1 ||
            a.v2 == b.v2);
  }

  template <typename VertexType>
  std::ostream& operator<<(
      std::ostream &os,
      const undirected_edge<VertexType>& e) {
    return os << e.v1 << " " << e.v2;
  }

  template <typename VertexType>
  std::istream& operator>>(std::istream &is,
      undirected_edge<VertexType>& e) {
    return is >> e.v1 >> e.v2;
  }

  template <typename VertexType>
  inline std::tuple<VertexType, VertexType>
  undirected_edge<VertexType>::comp_tuple() const {
    return std::make_tuple(std::min(v1, v2), std::max(v1, v2));
  }
}  // namespace dag
