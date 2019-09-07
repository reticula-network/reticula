#include <functional>
#include <istream>
#include <ostream>

#include "../../../include/utils.hpp"

// Hashing rules for edges
namespace std {
  template<typename VertT>
  struct hash<dag::directed_edge<VertT>> {
    size_t
    operator()(const dag::directed_edge<VertT>& e) const {
      return dag::utils::combine_hash(std::hash<VertT>{}(e.v1), e.v2);
    }
  };

  template<typename VertT>
  struct hash<dag::undirected_edge<VertT>> {
    size_t operator()(const dag::undirected_edge<VertT>& e) const {
      return dag::utils::unordered_hash(e.v1, e.v2);
    }
  };
}

namespace dag {
  // directed edge IO:
  template <typename VertT>
  std::ostream& operator<<(std::ostream& os,
      const directed_edge<VertT>& e) {
    return os << e.tail_vert() << " " << e.head_vert();
  }

  template <typename VertT>
  std::istream& operator>>(std::istream& is,
      directed_edge<VertT>& e) {
    return is >> e.v1 >> e.v2;
  }


  // undirected edge IO:
  template <typename VertT>
  std::ostream& operator<<(std::ostream &os,
      const undirected_edge<VertT>& e) {
    return os << e.v1 << " " << e.v2;
  }

  template <typename VertT>
  std::istream& operator>>(std::istream& is,
      undirected_edge<VertT>& e) {
    return is >> e.v1 >> e.v2;
  }
}
