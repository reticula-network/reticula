#include <functional>
#include <istream>
#include <ostream>

#include "../include/dag/utils.hpp"

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
