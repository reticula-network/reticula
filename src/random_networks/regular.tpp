namespace dag {
  template <integer_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT> random_regular_graph(
      VertT size, VertT degree,
      Gen& gen) {
    std::optional<undirected_network<VertT>> maybe_g = std::nullopt;
    while (!maybe_g) maybe_g = try_random_regular_graph(
        size, degree, gen, 1000);
    return *maybe_g;
  }

  template <integer_vertex VertT, std::uniform_random_bit_generator Gen>
  std::optional<undirected_network<VertT>>
  try_random_regular_graph(
      VertT size, VertT degree,
      Gen& gen,
      std::size_t max_tries) {
    if (size*degree % 2 != 0)
      throw std::domain_error("size or degree must be even");

    if (size <= 0)
      throw std::domain_error("size must be positive");

    if (degree >= size)
      throw std::domain_error("degree must be less than size");

    auto has_hanging_edge = [](
        const std::unordered_set<
          undirected_edge<VertT>,
          hash<undirected_edge<VertT>>>& edges,
        const std::unordered_map<
          VertT, std::size_t, hash<VertT>>& hanging_stubs) {
      if (hanging_stubs.empty()) {
        return false;
      } else {
        for (auto& [node1, count1]: hanging_stubs)
          for (auto& [node2, count1]: hanging_stubs)
            if (node1 == node2)
              break;
            else if (edges.find({node1, node2}) == edges.end())
              return true;
      }
      return false;
    };

    for (std::size_t t = 0; t < max_tries; t++) {
      std::unordered_set<
        undirected_edge<VertT>,
        hash<undirected_edge<VertT>>> edges;
      std::vector<VertT> stubs;
      stubs.reserve(static_cast<std::size_t>(size*degree));
      for (VertT i = 0; i < size; i++)
        for (VertT d = 0; d < degree; d++)
          stubs.emplace_back(i);

      bool suitable = true;
      while (suitable) {
        std::shuffle(stubs.begin(), stubs.end(), gen);
        std::unordered_map<VertT, std::size_t, hash<VertT>> hanging_stubs;
        for (auto iter = stubs.begin();
            iter < stubs.end();
            std::advance(iter, 2)) {
          if (*iter != *(iter+1) &&
              edges.find({*iter, *(iter+1)}) == edges.end()) {
            edges.emplace(*iter, *(iter+1));
          } else {
            hanging_stubs[*iter]++;
            hanging_stubs[*(iter+1)]++;
          }
        }

        stubs.clear();
        for (auto& [node, count]: hanging_stubs)
          for (std::size_t i = 0; i < count; i++)
             stubs.emplace_back(node);

        suitable = has_hanging_edge(edges, hanging_stubs);
      }

      if (edges.size() == static_cast<std::size_t>(size*degree)/2)
        return undirected_network<VertT>(
            std::vector<dag::undirected_edge<VertT>>(
              edges.begin(), edges.end()));
    }

    return std::nullopt;
  }
}  // namespace dag
