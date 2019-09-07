namespace dag {
  template <class EdgeT,
          template<typename> class EstimatorT, //  = template <typename T> hll_estimator<T, HyperLogLog<18, 19>>
          template<typename> class ReadOnlyEstimatorT> // = template <typename T> hll_estimator_readonly<T , HyperLogLog<18, 19>>
  std::vector<std::pair<EdgeT,
    component_size_counter<EdgeT, ReadOnlyEstimatorT>>>
  out_component_size_estimate(
      const implicit_event_graph<EdgeT>& eg,
      uint32_t seed,
      bool only_roots=false,
      bool revert_event_graph=false) {

    std::unordered_map<EdgeT, component_size_counter<EdgeT, EstimatorT>>
      out_components;
    std::vector<std::pair<EdgeT, component_size_counter<EdgeT, ReadOnlyEstimatorT>>>
      out_component_ests;
    out_component_ests.reserve(eg.topo().size());

    std::unordered_map<EdgeT, size_t> in_degrees;

    auto temp_edge_iter = eg.topo()rbegin();

    // todo: replace with proper logging
    size_t log_increment = eg.topo().size()/20;
    while (temp_edge_iter < eg.topo().rend()) {
      if (log_increment > 10'000 &&
          std::distance(eg.topo().rbegin(),
            temp_edge_iter) % log_increment == 0)
        std::cerr <<
          std::distance(
              eg.topo().rbegin(),
              temp_edge_iter)*100/eg.topo().size() <<
          "\% processed" << std::endl;

      out_components.emplace(*temp_edge_iter, seed);

      std::vector<EdgeT> successors = eg.successors(*temp_edge_iter);
      std::vector<EdgeT> predecessors = eg.predecessors(*temp_edge_iter);

      in_degrees[*temp_edge_iter] =
        (revert_event_graph ? successors : predecessors).size();

      for (const auto& other:
          (revert_event_graph ? predecessors : successors)) {

        out_components.at(*temp_edge_iter).merge(out_components.at(other));

        if (--in_degrees.at(other) == 0) {
          if (!only_roots)
            out_component_ests.emplace_back(other,
                out_components.at(other));
          out_components.erase(other);
          in_degrees.erase(other);
        }
      }

      out_components.at(*temp_edge_iter).insert(*temp_edge_iter);

      if (in_degrees.at(*temp_edge_iter) == 0) {
        out_component_ests.emplace_back(*temp_edge_iter,
          out_components.at(*temp_edge_iter));
        out_components.erase(*temp_edge_iter);
        in_degrees.erase(*temp_edge_iter);
      }
      temp_edge_iter++;
    }

    if (only_roots)
      out_component_ests.shrink_to_fit();

    return out_component_ests;
  }


  template <class EdgeT,
          template<typename> class EstimatorT,
          template<typename> class ReadOnlyEstimatorT>
  std::vector<std::pair<EdgeT,
    component_size_counter<EdgeT, ReadOnlyEstimatorT>>>
  in_component_size_estimate(
      const implicit_event_graph<EdgeT>& eg,
      uint32_t seed,
      bool only_roots=false) {
    return out_component_size_estimate<>(eg, seed, only_roots, true);
  }
}
