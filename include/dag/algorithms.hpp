#ifndef INCLUDE_DAG_ALGORITHMS_HPP_
#define INCLUDE_DAG_ALGORITHMS_HPP_

#include <vector>
#include <unordered_set>

namespace dag {
  template <class EdgeT, class AdjacencyProbT>
  directed_network<EdgeT>
  event_graph(
      const network<EdgeT>& temp,
      const AdjacencyProbT& prob,
      size_t seed);


  template <typename VertT>
  std::vector<VertT>
  topological_order(const directed_network<VertT>& dir);

  template <typename VertT>
  std::vector<VertT> out_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint = 0,
      bool revert_graph = false);

  template <typename VertT>
  std::vector<VertT> in_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint = 0);
}  // namespace dag

#include "../../src/algorithms.tpp"

#endif  // INCLUDE_DAG_ALGORITHMS_HPP_
