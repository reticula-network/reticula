namespace dag {
  template <class EdgeT>
  component_size_counter<EdgeT, exact_estimator> out_component(
      const event_graph<EdgeT>& eg,
      const EdgeT& root,
      size_t node_size_est,
      size_t edge_size_est) {

    using TimeT = typename EdgeT::TimeType;
    using VertT = typename EdgeT::VertexType;
    using delayed = dag::directed_delayed_temporal_edge<VertT, TimeT>;
    using directed = dag::directed_temporal_edge<VertT, TimeT>;
    using undirected = dag::undirected_temporal_edge<VertT, TimeT>;

    if (!eg.deterministic())
      return generic_out_component(
          eg, root, node_size_est, edge_size_est);
    else if constexpr (std::is_same<EdgeT, directed>::value ||
              std::is_same<EdgeT, undirected>::value ||
              std::is_same<EdgeT, delayed>::value)
      return deterministic_out_component(
          eg, root, node_size_est, edge_size_est);
    else
      return generic_out_component(
          eg, root, node_size_est, edge_size_est);
  }

  template <class EdgeT>
  component_size_counter<EdgeT, exact_estimator> generic_out_component(
      const event_graph<EdgeT>& eg,
      const EdgeT& root,
      size_t node_size_est,
      size_t edge_size_est) {

    std::queue<EdgeT> search({root});
    component_size_counter<EdgeT, exact_estimator>
      out_component(0, edge_size_est, node_size_est);
    out_component.insert(root);

    while (!search.empty()) {
      EdgeT e = search.front();
      search.pop();
      for (auto&& s: eg.successors(e))
        if (!out_component.edge_set().contains(s)) {
          search.push(s);
          out_component.insert(s);
        }
    }

    return out_component;
  }


  template <class EdgeT>
  component_size_counter<EdgeT, exact_estimator> deterministic_out_component(
      const event_graph<EdgeT>& eg,
      const EdgeT& root,
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

    component_size_counter<EdgeT, exact_estimator, exact_estimator>
      out_component(0, edge_size_est, node_size_est);
    out_component.insert(root);

    std::unordered_map<VertexType, TimeType> last_infected;
    last_infected.reserve(eg.topo().size());

    for (auto && v: root.mutated_verts())
      last_infected[v] = root.effect_time();

    TimeType last_infect_time = root.effect_time();

    auto topo_it = std::upper_bound(eg.topo().begin(), eg.topo().end(), root);

    while (topo_it < eg.topo().end() &&
        (topo_it->time < last_infect_time ||
        topo_it->time - last_infect_time < eg.expected_dt())) {

      while (!in_transition.empty() &&
          in_transition.top().effect_time() < topo_it->time) {
        for (auto && v: in_transition.top().mutated_verts()) {
          last_infected[v] = in_transition.top().effect_time();
        }
        out_component.insert(in_transition.top());
        in_transition.pop();
      }


      bool is_infecting = false;

      for (auto && v: topo_it->mutator_verts()) {
        auto last_infected_it = last_infected.find(v);
        if (last_infected_it != last_infected.end() &&
            topo_it->time > last_infected_it->second &&
            topo_it->time - last_infected_it->second < eg.expected_dt())
          is_infecting = true;
      }

      if (is_infecting) {
        if (topo_it->time == topo_it->effect_time()) {
          out_component.insert(*topo_it);
          for (auto && v: topo_it->mutated_verts())
            last_infected[v] = topo_it->time;
        } else in_transition.push(*topo_it);
        last_infect_time =
          std::max(topo_it->effect_time(), last_infect_time);
      }

      topo_it++;
    }

    while (!in_transition.empty()) {
        out_component.insert(in_transition.top());
        in_transition.pop();
    }

    return out_component;
  }
}
