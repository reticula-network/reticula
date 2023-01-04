namespace reticula {
  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  implicit_event_graph<EdgeT, AdjT>::implicit_event_graph(
      const std::initializer_list<EdgeT>& events,
      const AdjT& adj) : _temp(events), _adj(adj) {}

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  implicit_event_graph<EdgeT, AdjT>::implicit_event_graph(
      const std::initializer_list<EdgeT>& events,
      const std::initializer_list<typename EdgeT::VertexType>& verts,
      const AdjT& adj) : _temp(events, verts), _adj(adj) {}

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  template <std::ranges::input_range Range>
  requires std::convertible_to<std::ranges::range_value_t<Range>, EdgeT>
  implicit_event_graph<EdgeT, AdjT>::implicit_event_graph(
      Range&& events,
      const AdjT& adj) : _temp(events), _adj(adj) {}

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  template <
    std::ranges::input_range EdgeRange,
    std::ranges::input_range VertRange>
  requires
    std::convertible_to<std::ranges::range_value_t<EdgeRange>, EdgeT> &&
    std::convertible_to<std::ranges::range_value_t<VertRange>,
      typename EdgeT::VertexType>
  implicit_event_graph<EdgeT, AdjT>::implicit_event_graph(
      EdgeRange&& events,
      VertRange&& verts,
      const AdjT& adj) : _temp(events, verts), _adj(adj) {}

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  implicit_event_graph<EdgeT, AdjT>::implicit_event_graph(
      const network<EdgeT>& temp,
      const AdjT& adj) : _temp(temp), _adj(adj) {}


  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  AdjT implicit_event_graph<EdgeT, AdjT>::temporal_adjacency() const {
    return _adj;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  const std::vector<EdgeT>&
  implicit_event_graph<EdgeT, AdjT>::events_cause() const {
    return _temp.edges_cause();
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  const std::vector<EdgeT>&
  implicit_event_graph<EdgeT, AdjT>::events_effect() const {
    return _temp.edges_effect();
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<typename EdgeT::VertexType>
  implicit_event_graph<EdgeT, AdjT>::temporal_net_vertices() const {
    return _temp.vertices();
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::pair<typename EdgeT::TimeType, typename EdgeT::TimeType>
  implicit_event_graph<EdgeT, AdjT>::time_window() const {
    if (_temp.edges().empty())
      return std::make_pair(0, 0);
    else
      return std::make_pair(
          _temp.edges().front().cause_time(),
          _temp.edges().back().cause_time());
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjT>::predecessors(
      const EdgeT& e, bool just_first) const {
    std::vector<EdgeT> pred;

    pred.reserve(e.mutator_verts().size());

    for (auto&& v : e.mutator_verts()) {
      std::size_t middle_offset = pred.size();
      auto res = predecessors_vert(e, v, just_first);
      pred.reserve(pred.size() + res.size());
      std::sort(res.begin(), res.end());
      std::copy(
          res.begin(), res.end(),
          std::back_inserter(pred));
      std::inplace_merge(pred.begin(),
          pred.begin() + static_cast<std::ptrdiff_t>(middle_offset),
          pred.end());
    }

    pred.erase(std::unique(pred.begin(), pred.end()), pred.end());

    return pred;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjT>::successors(
      const EdgeT& e, bool just_first) const {
    std::vector<EdgeT> succ;

    succ.reserve(e.mutated_verts().size());

    for (auto&& v : e.mutated_verts()) {
      std::size_t middle_offset = succ.size();
      auto res = successors_vert(e, v, just_first);
      succ.reserve(succ.size() + res.size());
      std::sort(res.begin(), res.end());
      std::copy(
          res.begin(), res.end(),
          std::back_inserter(succ));
      std::inplace_merge(succ.begin(),
          succ.begin() + static_cast<std::ptrdiff_t>(middle_offset),
          succ.end());
    }

    succ.erase(std::unique(succ.begin(), succ.end()), succ.end());

    return succ;
  }


  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjT>::neighbours(
      const EdgeT& e, bool just_first) const {
    std::vector<EdgeT> inc(successors(e, just_first));
    std::vector<EdgeT> pred(predecessors(e, just_first));
    inc.insert(inc.end(), pred.begin(), pred.end());

    std::sort(inc.begin(), inc.end());
    inc.erase(std::unique(inc.begin(), inc.end()), inc.end());

    return inc;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjT>::successors_vert(
      const EdgeT& e, VertexType v, bool just_first) const {
    std::vector<EdgeT> res;
    auto out_edges_it = _temp.out_edges().find(v);
    if (out_edges_it == _temp.out_edges().end())
      return res;

    auto other = std::lower_bound(
        out_edges_it->second.begin(), out_edges_it->second.end(), e,
        [](const EdgeT& e1, const EdgeT& e2) { return e1 < e2; });

    typename EdgeT::TimeType cutoff = _adj.linger(e, v);

    if (just_first)
      res.reserve(2);
    else
      res.reserve(std::min<std::size_t>(
            32, static_cast<std::size_t>(
              out_edges_it->second.end() - other)));
    while ((other < out_edges_it->second.end()) &&
        other->cause_time() - e.effect_time() <= cutoff) {
      if (adjacent(e, *other)) {
        if (just_first && !res.empty() &&
            res[0].cause_time() != other->cause_time())
          return res;
        else
          res.push_back(*other);
      }
      other++;
    }
    return res;
  }

  template <
    temporal_network_edge EdgeT,
    temporal_adjacency::temporal_adjacency AdjT>
  std::vector<EdgeT>
  implicit_event_graph<EdgeT, AdjT>::predecessors_vert(
      const EdgeT& e, VertexType v, bool just_first) const {
    std::vector<EdgeT> res;
    auto in_edges_it = _temp.in_edges().find(v);
    if (in_edges_it == _temp.in_edges().end())
      return res;

    auto other = std::lower_bound(
        in_edges_it->second.begin(), in_edges_it->second.end(), e,
        [](const EdgeT& e1, const EdgeT& e2) { return effect_lt(e1, e2); });

    if (other > in_edges_it->second.begin())
      other--;

    typename EdgeT::TimeType cutoff = _adj.maximum_linger(v);
    if (just_first)
      res.reserve(2);
    else
      res.reserve(std::min<std::size_t>(
            32, static_cast<std::size_t>(
            other - in_edges_it->second.begin())));
    while (other >= in_edges_it->second.begin() &&
        e.cause_time() - other->effect_time() <= cutoff) {
      if (adjacent(*other, e)) {
        if (just_first && !res.empty() &&
            res[0].effect_time() != other->effect_time())
          return res;
        else
          res.push_back(*other);
      }
      other--;
    }
    return res;
  }
}  // namespace reticula
