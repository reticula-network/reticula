#ifndef DAG_ALGORITHMS_H
#define DAG_ALGORITHMS_H

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
      size_t size_hint=0,
      bool revert_graph=false);

  template <typename VertT>
  std::unordered_set<VertT> in_component(
      const directed_network<VertT>& dir,
      const  VertT& vert,
      size_t size_hint=0);
}

#include "../../src/algorithms.tpp"

#endif /* DAG_ALGORITHMS_H */
