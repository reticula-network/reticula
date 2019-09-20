namespace dag {
  template <class EdgeT,
           class AdjacencyProbT,
           template<typename> class EstimatorT,
           template<typename> class ReadOnlyEstimatorT>
  std::vector<std::pair<EdgeT,
    temporal_component<EdgeT, ReadOnlyEstimatorT>>>
  out_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      size_t seed,
      bool only_roots) {
    std::unordered_map<EdgeT, temporal_component<EdgeT, EstimatorT>>
      out_components;
    std::vector<
      std::pair<EdgeT, temporal_component<EdgeT, ReadOnlyEstimatorT>>>
        out_component_ests;
    out_component_ests.reserve(eg.events_cause().size());

    std::unordered_map<EdgeT, size_t> in_degrees;

    auto temp_edge_iter = eg.events_cause().rbegin();
    auto end = eg.events_cause().rend();

    while (temp_edge_iter < end) {
      out_components.emplace(*temp_edge_iter, seed);

      std::vector<EdgeT> successors = eg.successors(*temp_edge_iter);
      std::vector<EdgeT> predecessors = eg.predecessors(*temp_edge_iter);

      in_degrees[*temp_edge_iter] = predecessors.size();

      for (const auto& other: successors) {
        out_components.at(*temp_edge_iter).merge(out_components.at(other));

        if (--in_degrees.at(other) == 0) {
          if (!only_roots)
            out_component_ests.emplace_back(other,
                out_components.at(other));
          out_components.erase(other);
          in_degrees.erase(other);
        }
      }

      auto nodes = temp_edge_iter->mutated_verts();
      out_components.at(*temp_edge_iter).insert(*temp_edge_iter, nodes);

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
           class AdjacencyProbT,
           template<typename> class EstimatorT,
           template<typename> class ReadOnlyEstimatorT>
  std::vector<std::pair<EdgeT,
    temporal_component<EdgeT, ReadOnlyEstimatorT>>>
  in_components(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      size_t seed,
      bool only_roots) {
    std::unordered_map<EdgeT, temporal_component<EdgeT, EstimatorT>>
      out_components;
    std::vector<
      std::pair<EdgeT, temporal_component<EdgeT, ReadOnlyEstimatorT>>>
        out_component_ests;
    out_component_ests.reserve(eg.events_cause().size());

    std::unordered_map<EdgeT, size_t> in_degrees;

    auto temp_edge_iter = eg.events_effect().begin();
    auto end = eg.events_effect().end();

    while (temp_edge_iter < end) {
      out_components.emplace(*temp_edge_iter, seed);

      std::vector<EdgeT> successors = eg.successors(*temp_edge_iter);
      std::vector<EdgeT> predecessors = eg.predecessors(*temp_edge_iter);

      in_degrees[*temp_edge_iter] = successors.size();

      for (const auto& other: predecessors) {
        out_components.at(*temp_edge_iter).merge(out_components.at(other));

        if (--in_degrees.at(other) == 0) {
          if (!only_roots)
            out_component_ests.emplace_back(other,
                out_components.at(other));
          out_components.erase(other);
          in_degrees.erase(other);
        }
      }

      auto nodes = temp_edge_iter->mutator_verts();
      out_components.at(*temp_edge_iter).insert(*temp_edge_iter, nodes);

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
           class AdjacencyProbT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  _generic_out_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      size_t seed,
      size_t node_size_est,
      size_t edge_size_est,
      bool revert_graph) {
    std::queue<EdgeT> search({root});
    temporal_component<EdgeT, EstimatorT>
      out_component(seed, edge_size_est, node_size_est);
    auto nodes = (revert_graph ?
      root.mutator_verts() : root.mutated_verts());
    out_component.insert(root, nodes);

    while (!search.empty()) {
      EdgeT e = search.front();
      search.pop();
      auto next_events = (revert_graph ?
          eg.predecessors(e, reduced_event_graph(eg)) :
          eg.successors(e, reduced_event_graph(eg)));
      for (auto&& s: next_events)
        if (!out_component.edge_set().contains(s)) {
          search.push(s);
          auto nodes = (revert_graph ?
              s.mutator_verts() : s.mutated_verts());
          out_component.insert(s, nodes);
        }
    }

    return out_component;
  }


  template <class EdgeT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  _deterministic_out_component(
      const implicit_event_graph<EdgeT,
        adjacency_prob::deterministic<EdgeT>>& eg,
      const EdgeT& root,
      size_t seed,
      size_t node_size_est,
      size_t edge_size_est) {
    auto comp_function = [](const EdgeT& e1, const EdgeT& e2) {
      return (e1.effect_time()) > (e2.effect_time());
    };

    std::priority_queue<
      EdgeT, std::vector<EdgeT>,
      decltype(comp_function)> in_transition(comp_function);

    in_transition.push(root);

    using VertexType = typename EdgeT::VertexType;
    using TimeType = typename EdgeT::TimeType;

    adjacency_prob::deterministic<EdgeT> prob = eg.adjacency_prob();

    temporal_component<EdgeT, EstimatorT>
      out_component(seed, edge_size_est, node_size_est);
    out_component.insert(root, root.mutated_verts());

    std::unordered_map<VertexType, TimeType> last_infected;
    last_infected.reserve(eg.events_cause().size());

    for (auto && v: root.mutated_verts())
      last_infected[v] = root.effect_time();

    TimeType last_infect_time = root.effect_time();

    auto begin = eg.events_cause().begin();
    auto end = eg.events_cause().end();

    auto topo_it = std::upper_bound(begin, end, root);

    while (topo_it < end &&
        (topo_it->cause_time() < last_infect_time ||
        topo_it->cause_time() - last_infect_time < prob.cutoff_dt())) {
      while (!in_transition.empty() &&
          in_transition.top().effect_time() < topo_it->cause_time()) {
        for (auto && v: in_transition.top().mutated_verts()) {
          last_infected[v] = in_transition.top().effect_time();
        }

        auto event = in_transition.top();
        out_component.insert(event, event.mutated_verts());
        in_transition.pop();
      }


      bool is_infecting = false;

      for (auto && v: topo_it->mutator_verts()) {
        auto last_infected_it = last_infected.find(v);
        if (last_infected_it != last_infected.end() &&
            topo_it->cause_time() > last_infected_it->second &&
            topo_it->cause_time() - last_infected_it->second < prob.cutoff_dt())
          is_infecting = true;
      }

      if (is_infecting) {
        if (topo_it->cause_time() == topo_it->effect_time()) {
          out_component.insert(*topo_it, topo_it->mutated_verts());
          for (auto && v: topo_it->mutated_verts())
            last_infected[v] = topo_it->cause_time();
        } else {
          in_transition.push(*topo_it);
        }
        last_infect_time =
          std::max(topo_it->effect_time(), last_infect_time);
      }

      topo_it++;
    }

    while (!in_transition.empty()) {
      auto event = in_transition.top();
      out_component.insert(event, event.mutated_verts());
      in_transition.pop();
    }

    return out_component;
  }


  template <class EdgeT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  _deterministic_in_component(
      const implicit_event_graph<EdgeT,
          adjacency_prob::deterministic<EdgeT>>& eg,
      const EdgeT& root,
      size_t seed,
      size_t node_size_est,
      size_t edge_size_est) {
    auto comp_function = [](const EdgeT& e1, const EdgeT& e2) {
      return (e1.cause_time()) < (e2.cause_time());
    };

    std::priority_queue<
      EdgeT, std::vector<EdgeT>,
      decltype(comp_function)> in_transition(comp_function);

    in_transition.push(root);

    using VertexType = typename EdgeT::VertexType;
    using TimeType = typename EdgeT::TimeType;

    adjacency_prob::deterministic<EdgeT> prob = eg.adjacency_prob();

    temporal_component<EdgeT, EstimatorT>
      in_component(seed, edge_size_est, node_size_est);
    in_component.insert(root, root.mutator_verts());

    std::unordered_map<VertexType, TimeType> last_infected;
    last_infected.reserve(eg.events_effect().size());

    for (auto && v: root.mutator_verts())
      last_infected[v] = root.cause_time();

    TimeType last_infect_time = root.cause_time();

    auto begin = eg.events_effect().rbegin();
    auto end = eg.events_effect().rend();

    auto topo_it = std::upper_bound(begin, end, root,
        [](const EdgeT& a, const EdgeT& b){
          return a.effect_time() > b.effect_time();
        });

    while (topo_it < end &&
        (topo_it->effect_time() < last_infect_time ||
         topo_it->effect_time() - last_infect_time  < prob.cutoff_dt())) {
      while (!in_transition.empty() &&
          in_transition.top().cause_time() > topo_it->effect_time()) {
        for (auto && v: in_transition.top().mutator_verts()) {
          last_infected[v] = in_transition.top().cause_time();
        }

        auto event = in_transition.top();
        in_component.insert(event, event.mutator_verts());
        in_transition.pop();
      }


      bool is_infecting = false;

      for (auto && v: topo_it->mutated_verts()) {
        auto last_infected_it = last_infected.find(v);
        if (last_infected_it != last_infected.end() &&
            topo_it->effect_time() < last_infected_it->second &&
            last_infected_it->second - topo_it->effect_time() <
                prob.cutoff_dt())
          is_infecting = true;
      }

      if (is_infecting) {
        if (topo_it->cause_time() == topo_it->effect_time()) {
          in_component.insert(*topo_it, topo_it->mutator_verts());
          for (auto && v: topo_it->mutator_verts())
            last_infected[v] = topo_it->effect_time();
        } else {
          in_transition.push(*topo_it);
        }
        last_infect_time =
          std::min(topo_it->cause_time(), last_infect_time);
      }

      topo_it++;
    }

    while (!in_transition.empty()) {
      auto event = in_transition.top();
      in_component.insert(event, event.mutator_verts());
      in_transition.pop();
    }

    return in_component;
  }


  template <class EdgeT,
           class AdjacencyProbT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  _out_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      size_t seed,
      size_t node_size_est,
      size_t edge_size_est,
      bool revert_graph) {
    if constexpr (std::is_same_v<AdjacencyProbT,
                                    adjacency_prob::deterministic<EdgeT>>)
      if (revert_graph)
        return _deterministic_in_component<EdgeT, EstimatorT>(
            eg, root, seed, node_size_est, edge_size_est);
      else
        return _deterministic_out_component<EdgeT, EstimatorT>(
            eg, root, seed, node_size_est, edge_size_est);
    else
      return _generic_out_component<EdgeT, AdjacencyProbT, EstimatorT>(
          eg, root, seed, node_size_est, edge_size_est, revert_graph);
  }

  /**
    Only enable reduced event graph (i.e. arXiv 1908.11831 appendix B) for
    undirected temporal edges
   */
  template <class EdgeT, class AdjacencyProbT>
  constexpr bool reduced_event_graph(
      implicit_event_graph<EdgeT, AdjacencyProbT> /* eg */) {
    return
      std::is_same_v<EdgeT,
        dag::undirected_temporal_edge<typename EdgeT::VertexType,
                                      typename EdgeT::TimeType>> &&
      std::is_same_v<AdjacencyProbT,
        dag::adjacency_prob::deterministic<EdgeT>>;
  }

  template <class EdgeT,
           class AdjacencyProbT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  out_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      size_t seed,
      size_t node_size_hint,
      size_t edge_size_hint) {
    return _out_component<EdgeT, AdjacencyProbT, EstimatorT>(
        eg, root, seed,
        node_size_hint, edge_size_hint, false);
  }

  template <class EdgeT,
           class AdjacencyProbT,
           template<typename> class EstimatorT>
  temporal_component<EdgeT, EstimatorT>
  in_component(
      const implicit_event_graph<EdgeT, AdjacencyProbT>& eg,
      const EdgeT& root,
      size_t seed,
      size_t node_size_hint,
      size_t edge_size_hint) {
    return _out_component<EdgeT, AdjacencyProbT, EstimatorT>(
        eg, root, seed,
        node_size_hint, edge_size_hint, true);
  }

}  // namespace dag
