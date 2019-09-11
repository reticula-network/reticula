#ifndef INCLUDE_DAG_ALGORITHMS_HPP_
#define INCLUDE_DAG_ALGORITHMS_HPP_

#include <vector>
#include <unordered_set>

namespace dag {
  template <typename EdgeT>
  directed_network<EdgeT>
  event_graph(const network<EdgeT>& temp, typename EdgeT::TimeType max_delta_t);


  template <typename VertT>
  std::vector<VertT>
  topological_order(const directed_network<VertT>& dir);

  template <typename VertT>
  std::unordered_set<VertT> out_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint = 0,
      bool revert_graph = false);

  template <typename VertT>
  std::unordered_set<VertT> in_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint = 0);
}  // namespace dag

#include "../../src/algorithms.tpp"

#endif  // INCLUDE_DAG_ALGORITHMS_HPP_
