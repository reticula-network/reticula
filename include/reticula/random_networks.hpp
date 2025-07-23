#ifndef INCLUDE_RETICULA_RANDOM_NETWORKS_HPP_
#define INCLUDE_RETICULA_RANDOM_NETWORKS_HPP_

#include <concepts>
#include <optional>
#include <numeric>

#include "ranges.hpp"
#include "distributions.hpp"
#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT> random_gnp_graph(
      VertT n, double p,
      Gen& generator);

  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  directed_network<VertT> random_directed_gnp_graph(
      VertT n, double p,
      Gen& generator);


  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT> random_barabasi_albert_graph(
      VertT n, VertT m,
      Gen& generator);


  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT>
  random_watts_strogatz_graph(VertT n, VertT k, double p, Gen& generator);


  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT>
  random_regular_graph(
      VertT size, VertT degree,
      Gen& generator);

  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  std::optional<undirected_network<VertT>>
  try_random_regular_graph(
      VertT size, VertT degree,
      Gen& generator,
      std::size_t max_tries);


  template <
    integer_network_vertex VertT,
    ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    degree_range<Range> &&
    std::convertible_to<ranges::range_value_t<Range>, VertT>
  undirected_network<VertT>
  random_degree_sequence_graph(
      Range&& degree_sequence,
      Gen& generator);

  template <
    integer_network_vertex VertT,
    ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    degree_range<Range> &&
    std::convertible_to<ranges::range_value_t<Range>, VertT>
  std::optional<undirected_network<VertT>>
  try_random_degree_sequence_graph(
      Range&& degree_sequence,
      Gen& generator,
      std::size_t max_tries);


  template <
    integer_network_vertex VertT,
    ranges::forward_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires
    degree_pair_range<PairRange> &&
    is_pairlike_of<ranges::range_value_t<PairRange>, VertT, VertT>
  directed_network<VertT>
  random_directed_degree_sequence_graph(
      PairRange&& in_out_degree_sequence,
      Gen& generator);

  template <
    integer_network_vertex VertT,
    ranges::forward_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires
    degree_pair_range<PairRange> &&
    is_pairlike_of<ranges::range_value_t<PairRange>, VertT, VertT>
  std::optional<directed_network<VertT>>
  try_random_directed_degree_sequence_graph(
      PairRange&& in_out_degree_sequence,
      Gen& generator,
      std::size_t max_tries);


  /**
    Random undirected graph generated with the given weight sequence, more
    specifically known as ``Chung-Lu graphs''. As the size of the network grows,
    the expected degrees sequence of the generated network approaches the given
    weight sequence, which for this reason is more commonly known as the degree
    sequence.

    Additionally, existence of each two edges in the generated graph is
    independent of each other.

    Chung, Fan, and Linyuan Lu. "Connected components in random graphs with
    given expected degree sequences." Annals of combinatorics 6.2 (2002):
    125-145.

    Miller, Joel C., and Aric Hagberg. "Efficient generation of networks with
    given expected degrees." International Workshop on Algorithms and Models for
    the Web-Graph. Springer, Berlin, Heidelberg, 2011.
  */
  template <
    integer_network_vertex VertT,
    ranges::input_range Range,
    std::uniform_random_bit_generator Gen>
  requires weight_range<Range>
  undirected_network<VertT>
  random_expected_degree_sequence_graph(
      Range&& weight_sequence,
      Gen& generator,
      bool self_loops = false);

  /**
    Random directed graph generated with the two given degree (weight)
    sequences, corresponding to in- and out-degree, more specifically, this is
    based on directed extention of "Chung-Lu" algorithm. As the size of the
    network grows, the expected in- and out-degrees (weight) sequence of the
    generated graph approaches the given degree_sequence. For example, if the
    in-degree sequence provided is denoted by $i_1, i_2, \ldots, i_n$ and the
    out-degree sequence by $o_1, o_2, \ldots, o_n$, in the generated graph, node
    $u$ will have an expected in-degree of $i_u$ and expected out-degree of
    $o_u$.

    Additionally, existence of each two edges in the generated graph is
    independent of each other.

    The implementation is based on Chung-Lu algorithm (i.e. that of
    `reticula::random_expected_degree_sequence_graph`) with link probabilities
    taking into account out-degree weight of the tail node and in-degree weight
    of the possible head node.

    Chung, Fan, and Linyuan Lu. "Connected components in random graphs with
    given expected degree sequences." Annals of combinatorics 6.2 (2002):
    125-145.

    Miller, Joel C., and Aric Hagberg. "Efficient generation of networks with
    given expected degrees." International Workshop on Algorithms and Models for
    the Web-Graph. Springer, Berlin, Heidelberg, 2011.
  */
  template <
    integer_network_vertex VertT,
    ranges::input_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires weight_pair_range<PairRange>
  directed_network<VertT>
  random_directed_expected_degree_sequence_graph(
      PairRange&& in_out_weight_sequence,
      Gen& generator,
      bool self_loops = false);


  template <
    integer_network_vertex VertT,
    ranges::input_range VertRange,
    ranges::input_range EdgeRange,
    std::uniform_random_bit_generator Gen>
  requires weight_range<VertRange> && weight_range<EdgeRange>
  undirected_hypernetwork<VertT>
  random_expected_degree_sequence_hypergraph(
      VertRange&& vertex_weight_sequence,
      EdgeRange&& edge_weight_sequence,
      Gen& generator);

  template <
    integer_network_vertex VertT,
    ranges::input_range VertPairRange,
    ranges::input_range EdgePairRange,
    std::uniform_random_bit_generator Gen>
  requires weight_pair_range<VertPairRange> &&
    weight_pair_range<EdgePairRange>
  directed_hypernetwork<VertT>
  random_directed_expected_degree_sequence_hypergraph(
      VertPairRange&& vertex_in_out_weight_sequence,
      EdgePairRange&& edge_in_out_weight_sequence,
      Gen& generator);


  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_hypernetwork<VertT>
  random_uniform_hypergraph(
      VertT size, VertT edge_degree, double edge_prob, Gen& generator);

  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  directed_hypernetwork<VertT>
  random_directed_uniform_hypergraph(
      VertT size, VertT edge_in_degree,
      VertT edge_out_degree, double edge_prob,
      Gen& generator);


  /**
    Random undirected temporal network, with nodes labeled from 0 to `size` and
    where and undirected connection between any two nodes can be activated
    independently as Poisson point processes with rate `rate`. The activations
    are recorded in range of time [0, `max_t`).

    @param size The number of nodes
    @param rate Per-edge Poisson point process rate
    @param max_t The end of observation window.
  */
  template <
    integer_network_vertex VertT,
    std::uniform_random_bit_generator Gen>
  undirected_temporal_network<VertT, double>
  random_fully_mixed_temporal_network(
      VertT size, double rate, double max_t, Gen& generator);

  /**
    Random directed temporal network, with nodes labeled from 0 to `size` and
    where a directed connection between any two nodes can be activated
    independently as Poisson point processes with rate `rate`. The activations
    are recorded in range of time [0, `max_t`).

    @param size The number of nodes
    @param rate Per-edge Poisson point process rate
    @param max_t The end of observation window.
    @param generator A uniform random bit generator
  */
  template <
    integer_network_vertex VertT,
    std::uniform_random_bit_generator Gen>
  directed_temporal_network<VertT, double>
  random_directed_fully_mixed_temporal_network(
      VertT size, double rate, double max_t, Gen& generator);


  /**
    Random temporal network constructed from a base (aggregate) static network
    where each link is activated according to the inter-event time distribution
    provided by `iet_dist` observed in time range [0, `max_t`). The time of the
    first event for each edge is derived from the distribution `res_dist`.

    @param base_net Base (aggregate) static network
    @param max_t The end of observation window
    @param iet_dist random_number_distribution that generates inter-event times
    for activations of each link
    @param res_dist random_number_distribution that generates time of the first
    event for each link
    @param generator A uniform random bit generator
    @param size_hint Estimated number of edges in the final temporal network.
  */
  template <
    temporal_network_edge EdgeT,
    random_number_distribution IETDist,
    random_number_distribution ResDist,
    std::uniform_random_bit_generator Gen>
  requires
    std::same_as<
      typename IETDist::result_type, typename ResDist::result_type> &&
    std::constructible_from<EdgeT,
      typename EdgeT::StaticProjectionType, typename IETDist::result_type>
  network<EdgeT>
  random_link_activation_temporal_network(
      const network<typename EdgeT::StaticProjectionType>& base_net,
      typename EdgeT::TimeType max_t,
      IETDist iet_dist,
      ResDist res_dist,
      Gen& generator,
      std::size_t size_hint = 0);


  /**
    Random temporal network constructed from a base (aggregate) static network
    where each link is activated according to the inter-event time distribution
    provided by `iet_dist` observed in time range [0, `max_t`). The time of the
    first event for each edge is derived from a burn-in process by running the
    inter-event time distribution for time period equal to `max_t`.

    @param base_net Base (aggregate) static network
    @param max_t The end of observation window
    @param iet_dist random_number_distribution that generates inter-event times
    for activations of each link
    @param generator A uniform random bit generator
    @param size_hint Estimated number of edges in the final temporal network.
  */
  template <
    temporal_network_edge EdgeT,
    random_number_distribution IETDist,
    std::uniform_random_bit_generator Gen>
  requires
    std::constructible_from<EdgeT,
      typename EdgeT::StaticProjectionType, typename IETDist::result_type>
  network<EdgeT>
  random_link_activation_temporal_network(
      const network<typename EdgeT::StaticProjectionType>& base_net,
      typename EdgeT::TimeType max_t,
      IETDist iet_dist,
      Gen& generator,
      std::size_t size_hint = 0);


  /**
    Random temporal network constructed from a base (aggregate) static network
    where each node is activated according to the inter-activation time
    distribution provided by `iet_dist` observed in time range [0, `max_t`).
    The time of the first activation for each node is derived from the
    distribution `res_dist`.

    At each activation of a node, a random outgoing link from that node is
    selected an event is added for that link.

    Note that for undirected networks, the activation times of each link are
    the result of independent activation of the nodes incident to the link. For
    example, for a network consisting of a single link and two nodes, the link
    is activated every time either of the two nodes are activated.

    Also note that if a non-renewal process is used for generating node
    activation times, the exogenous activations of links connected to that node
    do not affect the future activations of that node in any way. This is,
    therefore not a suitable method for simulating, e.g., activation cascades.

    @param base_net Base (aggregate) static network
    @param max_t The end of observation window
    @param iet_dist random_number_distribution that generates inter-activation
    times of each node
    @param res_dist random_number_distribution that generates time of the first
    activation for each node
    @param generator A uniform random bit generator
    @param size_hint Estimated number of edges in the final temporal network.
  */
  template <
    temporal_network_edge EdgeT,
    random_number_distribution IETDist,
    random_number_distribution ResDist,
    std::uniform_random_bit_generator Gen>
  requires
    std::same_as<
      typename IETDist::result_type, typename ResDist::result_type> &&
    std::constructible_from<EdgeT,
      typename EdgeT::StaticProjectionType, typename IETDist::result_type>
  network<EdgeT>
  random_node_activation_temporal_network(
      const network<typename EdgeT::StaticProjectionType>& base_net,
      typename EdgeT::TimeType max_t,
      IETDist iet_dist,
      ResDist res_dist,
      Gen& generator,
      std::size_t size_hint = 0);


  /**
    Random temporal network constructed from a base (aggregate) static network
    where each node is activated according to the inter-activation time
    distribution provided by `iet_dist` observed in time range [0, `max_t`).
    The time of the first activation for each edge is derived from a burn-in
    process by running the inter-event time distribution for time period equal
    to `max_t`.

    At each activation of a node, a random outgoing link from that node is
    selected an an event is added for that link.

    Note that for undirected networks, the activation times of each link are
    the result of indipendent activation of the nodes incident to the link. For
    example, for a network consisting of a single link and two nodes, the link
    is activated every time either of the two nodes are activated.

    Also note that if a non-renewal process is used for generating node
    activation times, the exogenous activations of links connected to that node
    do not affect the future activations of that node in any way. This is,
    therefore not a suitable method for simulating, e.g., activation cascades.

    @param base_net Base (aggregate) static network
    @param max_t The end of observation window
    @param iet_dist random_number_distribution that generates inter-activation
    times of the nodes
    @param generator A uniform random bit generator
    @param size_hint Estimated number of edges in the final temporal network
  */
  template <
    temporal_network_edge EdgeT,
    random_number_distribution IETDist,
    std::uniform_random_bit_generator Gen>
  requires
    std::constructible_from<EdgeT,
      typename EdgeT::StaticProjectionType, typename IETDist::result_type>
  network<EdgeT>
  random_node_activation_temporal_network(
      const network<typename EdgeT::StaticProjectionType>& base_net,
      typename EdgeT::TimeType max_t,
      IETDist iet_dist,
      Gen& generator,
      std::size_t size_hint = 0);
}  // namespace reticula

// Implementation
#include <cstddef>
#include <random>

#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  template <
    integer_network_vertex VertT,
    std::uniform_random_bit_generator Gen>
  undirected_network<VertT>
  random_barabasi_albert_graph(
      VertT n, VertT m,
      Gen& generator) {
    if (n <= m || m < 1)
      throw std::invalid_argument(
          "BA network must have m >= 1 and n > m");

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(m*(n-m)));

    std::vector<VertT> repeated_nodes;
    repeated_nodes.reserve(static_cast<std::size_t>(2 * m * (n - m)));


    for (VertT i = 0; i < m; i++) {
      edges.emplace_back(i, m);
      repeated_nodes.push_back(i);
      repeated_nodes.push_back(m);
    }

    for (VertT current_node = m+1; current_node < n; current_node++) {
      std::uniform_int_distribution<std::size_t> dist(
        0, repeated_nodes.size()-1);

      std::unordered_set<VertT> targets;
      targets.reserve(static_cast<std::size_t>(m));

      while (targets.size() < static_cast<std::size_t>(m))
        targets.insert(repeated_nodes[dist(generator)]);

      for (const auto& i: targets) {
        edges.emplace_back(current_node, i);
        repeated_nodes.push_back(i);
        repeated_nodes.push_back(current_node);
      }
    }

    return undirected_network<VertT>(edges, views::iota(VertT{}, n));
  }

  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT>
  random_watts_strogatz_graph(VertT n, VertT k, double p, Gen& generator) {
    if (n < 0)
      throw std::domain_error("n must be non-negative");

    if (k >= n)
      throw std::invalid_argument("k must be less than n");

    if (k % 2 != 0)
      throw std::invalid_argument("k must be divisible by two");

    if (p < 0.0 || p > 1.0)
      throw std::invalid_argument("rewiring probability p should be in [0,1]");

    if (n == 0)
        return undirected_network<VertT>();

    std::vector<std::unordered_set<VertT, hash<VertT>>> adj(
      static_cast<std::size_t>(n));

    std::uniform_real_distribution<> real_dist(0.0, 1.0);
    std::uniform_int_distribution<VertT> vert_dist(0, n - 1);

    for (VertT i = {}; i < n; i++) {
      auto& ai = adj[static_cast<std::size_t>(i)];
      for (VertT d = 1; d <= k / 2; d++) {
        VertT j = (i + d) % n;
        ai.insert(j);
        adj[static_cast<std::size_t>(j)].insert(i);
      }
    }

    if (p > 0.0) {
      for (VertT i = {}; i < n; i++) {
        auto& ai = adj[static_cast<std::size_t>(i)];
        for (VertT d = 1; d <= k / 2; d++) {
          VertT j = (i + d) % n;
          auto& aj = adj[static_cast<std::size_t>(j)];
          if (real_dist(generator) < p) {
            VertT r = vert_dist(generator);
            while (ai.contains(r) || r == i)
              r = vert_dist(generator);
            ai.erase(j);
            aj.erase(i);
            ai.insert(r);
            adj[static_cast<std::size_t>(r)].insert(i);
          }
        }
      }
    }

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(n * k / 2));
    for (VertT i = {}; i < n; i++)
      for (const auto& j: adj[static_cast<std::size_t>(i)])
        if (i < j) edges.emplace_back(i, j);

    return undirected_network<VertT>(edges, views::iota(VertT{}, n));
  }
}  // namespace reticula

#include <random>

#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT>
  random_gnp_graph(VertT n, double p, Gen& generator) {
    if (n < 0)
      throw std::domain_error("n must be non-negative");

    if (p > 1.0 || p < 0.0)
      throw std::invalid_argument(
          "edge probability p should be in [0,1] interval");

    if (n == 0 || p == 0)
      return undirected_network<VertT>(
          ranges::empty_view<undirected_edge<VertT>>(),
          views::iota(VertT{}, n));

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(
          static_cast<double>(n)*static_cast<double>(n/2)*p));

    std::uniform_real_distribution<> rd;

    double lp = std::log(1.0 - p);
    VertT v = 1;
    auto w = static_cast<VertT>(
        std::floor(std::log(1.0 - rd(generator))/lp));

    while (v < n) {
      while (w >= v && v < n) {
        w = w - v;
        v = v + 1;
      }

      if (v < n)
        edges.emplace_back(v, w);

      double lr = std::log(1.0 - rd(generator));
      w = w + static_cast<VertT>(1 + std::floor(lr/lp));
    }

    return undirected_network<VertT>(edges, views::iota(VertT{}, n));
  }

  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  directed_network<VertT>
  random_directed_gnp_graph(VertT n, double p, Gen& generator) {
    if (n < 0)
      throw std::domain_error("n must be non-negative");

    if (p > 1.0 || p < 0.0)
      throw std::invalid_argument(
          "edge probability p should be in [0,1] interval");

    if (n == 0 || p == 0)
      return directed_network<VertT>(
          ranges::empty_view<directed_edge<VertT>>(),
          views::iota(VertT{}, n));

    std::vector<directed_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(
          static_cast<double>(n)*static_cast<double>(n)*p));

    std::uniform_real_distribution<> rd;

    double lp = std::log(1.0 - p);
    VertT v = 1;
    auto w = static_cast<VertT>(
        std::floor(std::log(1.0 - rd(generator))/lp));

    while (v < n) {
      while (w >= (n - 1) && v < n) {
        w = w - (n - 1);
        v = v + 1;
      }

      if (v < n)
        edges.emplace_back(v, w < v ? w : w + 1);

      double lr = std::log(1.0 - rd(generator));
      w = w + static_cast<VertT>(1 + std::floor(lr/lp));
    }

    return directed_network<VertT>(edges, views::iota(VertT{}, n));
  }
}  // namespace reticula

#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT> random_regular_graph(
      VertT size, VertT degree,
      Gen& gen) {
    std::optional<undirected_network<VertT>> maybe_g = std::nullopt;
    while (!maybe_g) maybe_g = try_random_regular_graph(
        size, degree, gen, 1000);
    return *maybe_g;
  }

  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
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
        return undirected_network<VertT>(edges, views::iota(VertT{}, size));
    }

    return std::nullopt;
  }
}  // namespace reticula

#include <random>

#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  template <
    integer_network_vertex VertT,
    ranges::input_range Range,
    std::uniform_random_bit_generator Gen>
  requires weight_range<Range>
  undirected_network<VertT>
  random_expected_degree_sequence_graph(
      Range&& weight_sequence,
      Gen& generator,
      bool self_loops) {
    std::vector<std::pair<double, VertT>> weight_node_pairs;
    if constexpr (ranges::sized_range<Range>)
      weight_node_pairs.reserve(std::size(weight_sequence));

    for (VertT i{}; auto&& w: weight_sequence)
      weight_node_pairs.emplace_back(static_cast<double>(w), i++);

    ranges::sort(weight_node_pairs, ranges::greater());

    if (weight_node_pairs.empty())
      return undirected_network<VertT>();

    if (weight_node_pairs.front().first == 0.0)
      return undirected_network<VertT>(
          ranges::empty_view<undirected_edge<VertT>>{},
          views::iota(
            VertT{}, static_cast<VertT>(weight_node_pairs.size())));

    double s = std::accumulate(
        weight_node_pairs.begin(),
        weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    std::vector<undirected_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(s + 4.0*std::sqrt(s)));

    std::size_t last = weight_node_pairs.size();
    if (!self_loops)
      last--;

    for (std::size_t u = 0; u < last; u++) {
      std::size_t v = u;
      if (!self_loops)
        v++;

      auto [wu, i] = weight_node_pairs[u];
      auto [wv, j] = weight_node_pairs[v];

      double p = std::min(wu*wv/s, 1.0);
      while (v < weight_node_pairs.size() && p > 0) {
        if (p != 1.0)
          v += static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));
        if (v < weight_node_pairs.size()) {
          std::tie(wv, j) = weight_node_pairs[v];
          double q = std::min(wu*wv/s, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p)
            edges.emplace_back(i, j);
          p = q;
          v++;
        }
      }
    }

    return undirected_network<VertT>(edges,
        views::iota(
          VertT{}, static_cast<VertT>(weight_node_pairs.size())));
  }

  template <
    integer_network_vertex VertT,
    ranges::input_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires weight_pair_range<PairRange>
  directed_network<VertT> random_directed_expected_degree_sequence_graph(
      PairRange&& in_out_weight_sequence,
      Gen& generator,
      bool self_loops) {
    std::vector<std::pair<double, VertT>>
      in_weight_node_pairs, out_weight_node_pairs;

    if constexpr (ranges::sized_range<PairRange>) {
      in_weight_node_pairs.reserve(ranges::size(in_out_weight_sequence));
      out_weight_node_pairs.reserve(ranges::size(in_out_weight_sequence));
    }

    for (VertT i{}; auto& [w_in, w_out]: in_out_weight_sequence) {
      in_weight_node_pairs.emplace_back(static_cast<double>(w_in), i);
      out_weight_node_pairs.emplace_back(static_cast<double>(w_out), i++);
    }

    ranges::sort(in_weight_node_pairs, ranges::greater());
    ranges::sort(out_weight_node_pairs, ranges::greater());

    std::vector<std::size_t> node_index_in_in_weight_pair(
        in_weight_node_pairs.size(), 0);
    for (std::size_t j = 0; auto& [w, i]: in_weight_node_pairs)
      node_index_in_in_weight_pair[static_cast<std::size_t>(i)] = j++;

    if (in_weight_node_pairs.empty())
      return directed_network<VertT>();

    if (in_weight_node_pairs.front().first == 0.0 &&
        out_weight_node_pairs.front().first == 0.0)
      return directed_network<VertT>(
          ranges::empty_view<directed_edge<VertT>>{},
          views::iota(
            VertT{}, static_cast<VertT>(in_weight_node_pairs.size())));

    double s_in = std::accumulate(
        in_weight_node_pairs.begin(),
        in_weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    double s_out = std::accumulate(
        out_weight_node_pairs.begin(),
        out_weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    if (s_out > s_in*1.05 || s_out < s_in*0.95)
      throw std::invalid_argument(
          "in- and out-weight sequences should have (almost) equal sums");

    std::vector<directed_edge<VertT>> edges;
    edges.reserve(static_cast<std::size_t>(s_in + 4.0*std::sqrt(s_in)));

    for (std::size_t u = 0; u < out_weight_node_pairs.size(); u++) {
      std::size_t v = 0;

      auto [wu, i] = out_weight_node_pairs[u];
      auto [wv, j] = in_weight_node_pairs[v];

      double p = std::min(wu*wv/s_in, 1.0);
      while (v < in_weight_node_pairs.size() && p > 0) {
        if (p != 1.0) {
          auto delta = static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));

          if (!self_loops)
            if (node_index_in_in_weight_pair[
                  static_cast<std::size_t>(i)] >= v &&
                node_index_in_in_weight_pair[
                  static_cast<std::size_t>(i)] <= v+delta)
              delta++;
          v += delta;
        }

        if (v < in_weight_node_pairs.size()) {
          std::tie(wv, j) = in_weight_node_pairs[v];
          double q = std::min(wu*wv/s_in, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p)
            edges.emplace_back(i, j);
          p = q;
          v++;
        }
      }
    }

    return directed_network<VertT>(edges,
        views::iota(
          VertT{}, static_cast<VertT>(in_weight_node_pairs.size())));
  }

  template <
    integer_network_vertex VertT,
    ranges::input_range VertRange,
    ranges::input_range EdgeRange,
    std::uniform_random_bit_generator Gen>
  requires weight_range<VertRange> && weight_range<EdgeRange>
  undirected_hypernetwork<VertT>
  random_expected_degree_sequence_hypergraph(
      VertRange&& vertex_weight_sequence,
      EdgeRange&& edge_weight_sequence,
      Gen& generator) {
    std::vector<std::pair<double, VertT>> weight_node_pairs;
    if constexpr (ranges::sized_range<VertRange>)
      weight_node_pairs.reserve(std::size(vertex_weight_sequence));

    for (VertT i{}; auto&& w: vertex_weight_sequence)
      weight_node_pairs.emplace_back(static_cast<double>(w), i++);

    ranges::sort(weight_node_pairs, ranges::greater());

    double s = std::accumulate(
        weight_node_pairs.begin(),
        weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });


    std::vector<std::pair<double, std::size_t>> weight_edge_pairs;
    if constexpr (ranges::sized_range<EdgeRange>)
      weight_edge_pairs.reserve(std::size(edge_weight_sequence));

    for (std::size_t i{}; auto&& w: edge_weight_sequence)
      weight_edge_pairs.emplace_back(static_cast<double>(w), i++);

    ranges::sort(weight_node_pairs, ranges::greater());

    double s_edge = std::accumulate(
        weight_edge_pairs.begin(),
        weight_edge_pairs.end(),
        0.0, [](double total, std::pair<double, std::size_t> p){
          return total+p.first;
        });

    if (s > s_edge*1.05 || s < s_edge*0.95)
      throw std::invalid_argument(
          "vertex and edge weight sequences should have (almost) equal sums");

    if (weight_node_pairs.empty())
      return undirected_hypernetwork<VertT>();

    if (weight_node_pairs.front().first == 0.0)
      return undirected_hypernetwork<VertT>(
          ranges::empty_view<undirected_hyperedge<VertT>>{},
          views::iota(
            VertT{}, static_cast<VertT>(weight_node_pairs.size())));

    std::vector<undirected_hyperedge<VertT>> edges;
    edges.reserve(weight_edge_pairs.size());

    for (auto [wu, i] : weight_edge_pairs) {
      std::size_t v = 0;
      auto [wv, j] = weight_node_pairs[v];

      std::vector<VertT> incidents;
      incidents.reserve(static_cast<std::size_t>(wu + 4.0*std::sqrt(wu)));

      double p = std::min(wu*wv/s, 1.0);
      while (v < weight_node_pairs.size() && p > 0) {
        if (p != 1.0)
          v += static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));
        if (v < weight_node_pairs.size()) {
          std::tie(wv, j) = weight_node_pairs[v];
          double q = std::min(wu*wv/s, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p)
            incidents.push_back(j);
          p = q;
          v++;
        }
      }

      edges.emplace_back(incidents);
    }

    return undirected_hypernetwork<VertT>(edges,
        views::iota(
          VertT{}, static_cast<VertT>(weight_node_pairs.size())));
  }

  template <
    integer_network_vertex VertT,
    ranges::input_range VertPairRange,
    ranges::input_range EdgePairRange,
    std::uniform_random_bit_generator Gen>
  requires weight_pair_range<VertPairRange> &&
    weight_pair_range<EdgePairRange>
  directed_hypernetwork<VertT>
  random_directed_expected_degree_sequence_hypergraph(
      VertPairRange&& vertex_in_out_weight_sequence,
      EdgePairRange&& edge_in_out_weight_sequence,
      Gen& generator) {
    std::vector<std::pair<double, VertT>>
      in_weight_node_pairs, out_weight_node_pairs;

    if constexpr (ranges::sized_range<VertPairRange>) {
      in_weight_node_pairs.reserve(
          ranges::size(vertex_in_out_weight_sequence));
      out_weight_node_pairs.reserve(
          ranges::size(vertex_in_out_weight_sequence));
    }

    for (VertT i{}; auto& [w_in, w_out]: vertex_in_out_weight_sequence) {
      in_weight_node_pairs.emplace_back(static_cast<double>(w_in), i);
      out_weight_node_pairs.emplace_back(static_cast<double>(w_out), i++);
    }

    if (in_weight_node_pairs.empty())
      return directed_hypernetwork<VertT>();

    double s_in = std::accumulate(
        in_weight_node_pairs.begin(),
        in_weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    double s_out = std::accumulate(
        out_weight_node_pairs.begin(),
        out_weight_node_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    if (s_out > s_in*1.05 || s_out < s_in*0.95)
      throw std::invalid_argument(
          "vertex and edge in- and out-weight "
          "sequences should have (almost) equal sums");

    ranges::sort(in_weight_node_pairs, ranges::greater());
    ranges::sort(out_weight_node_pairs, ranges::greater());

    std::vector<std::pair<double, std::size_t>>
      in_weight_edge_pairs, out_weight_edge_pairs;

    if constexpr (ranges::sized_range<EdgePairRange>) {
      in_weight_node_pairs.reserve(
          ranges::size(edge_in_out_weight_sequence));
      out_weight_node_pairs.reserve(
          ranges::size(edge_in_out_weight_sequence));
    }

    for (std::size_t i{}; auto& [w_in, w_out]: edge_in_out_weight_sequence) {
      in_weight_edge_pairs.emplace_back(static_cast<double>(w_in), i);
      out_weight_edge_pairs.emplace_back(static_cast<double>(w_out), i++);
    }

    ranges::sort(in_weight_edge_pairs, ranges::greater());
    ranges::sort(out_weight_edge_pairs, ranges::greater());

    double s_edge_in = std::accumulate(
        in_weight_edge_pairs.begin(),
        in_weight_edge_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    double s_edge_out = std::accumulate(
        out_weight_edge_pairs.begin(),
        out_weight_edge_pairs.end(),
        0.0, [](double total, std::pair<double, VertT> p){
          return total+p.first;
        });

    if (s_in > s_edge_in*1.05 || s_in < s_edge_in*0.95
        || s_in > s_edge_out*1.05 || s_out < s_edge_out*0.95)
      throw std::invalid_argument(
          "vertex and edge in- and out-weight "
          "sequences should have (almost) equal sums");

    if (in_weight_node_pairs.front().first == 0.0)
      return directed_hypernetwork<VertT>(
          ranges::empty_view<directed_hyperedge<VertT>>{},
          views::iota(
            VertT{}, static_cast<VertT>(in_weight_node_pairs.size())));

    std::vector<std::pair<std::size_t, std::vector<VertT>>> edges_out_inc;
    edges_out_inc.reserve(out_weight_edge_pairs.size());

    for (auto [wu, i] : out_weight_edge_pairs) {
      std::size_t v = 0;
      auto [wv, j] = in_weight_node_pairs[v];

      std::vector<VertT> out_incidents;
      out_incidents.reserve(static_cast<std::size_t>(wu + 4.0*std::sqrt(wu)));

      double p = std::min(wu*wv/s_in, 1.0);
      while (v < in_weight_node_pairs.size() && p > 0) {
        if (p != 1.0) {
          auto delta = static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));
          v += delta;
        }

        if (v < in_weight_node_pairs.size()) {
          std::tie(wv, j) = in_weight_node_pairs[v];
          double q = std::min(wu*wv/s_in, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p)
            out_incidents.emplace_back(j);
          p = q;
          v++;
        }
      }

      edges_out_inc.emplace_back(i, out_incidents);
    }

    std::vector<std::pair<std::size_t, std::vector<VertT>>> edges_in_inc;
    edges_in_inc.reserve(in_weight_edge_pairs.size());

    for (auto [wu, i] : in_weight_edge_pairs) {
      std::size_t v = 0;
      auto [wv, j] = out_weight_node_pairs[v];

      std::vector<VertT> in_incidents;
      in_incidents.reserve(static_cast<std::size_t>(wu + 4.0*std::sqrt(wu)));

      double p = std::min(wu*wv/s_in, 1.0);
      while (v < out_weight_node_pairs.size() && p > 0) {
        if (p != 1.0) {
          auto delta = static_cast<std::size_t>(
              std::log(
                std::uniform_real_distribution{}(generator))/std::log(1-p));
          v += delta;
        }

        if (v < in_weight_node_pairs.size()) {
          std::tie(wv, j) = out_weight_node_pairs[v];
          double q = std::min(wu*wv/s_in, 1.0);
          if (std::uniform_real_distribution{}(generator) < q/p)
            in_incidents.emplace_back(j);
          p = q;
          v++;
        }
      }

      edges_in_inc.emplace_back(i, in_incidents);
    }

    ranges::sort(edges_in_inc);
    ranges::sort(edges_out_inc);

    std::vector<directed_hyperedge<VertT>> edges;
    edges.reserve(edges_in_inc.size());
    for (std::size_t i = 0; i < edges_in_inc.size(); i++)
      edges.emplace_back(edges_in_inc[i].second, edges_out_inc[i].second);

    return directed_hypernetwork<VertT>(edges,
        views::iota(
          VertT{}, static_cast<VertT>(in_weight_node_pairs.size())));
  }

}  // namespace reticula

#include <cmath>
#include <functional>
#include <random>
#include <unordered_set>

#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_hypernetwork<VertT>
  random_uniform_hypergraph(
      VertT size, VertT edge_degree, double edge_prob, Gen& generator) {
    if (size < 0)
      throw std::domain_error("size must be non-negative");

    if (edge_prob > 1.0 || edge_prob < 0.0)
      throw std::invalid_argument(
          "edge probability should be in [0,1] interval");

    if (edge_degree > size)
      throw std::invalid_argument(
          "edge_degree must be less than or equal to size");

    if (size == 0 || edge_prob == 0)
      return undirected_hypernetwork<VertT>(
          ranges::empty_view<undirected_hyperedge<VertT>>(),
          views::iota(VertT{}, size));

    std::vector<undirected_hyperedge<VertT>> edges;
    double logncm = std::lgamma(size + 1) -
                      std::lgamma(edge_degree + 1) -
                      std::lgamma(size - edge_degree + 1);
    const double ncm_estimate = std::exp(logncm + std::log(edge_prob));

    if (ncm_estimate > static_cast<double>(
        std::numeric_limits<std::size_t>::max()))
      throw std::invalid_argument(
          "cannot generate a hypergraph with that many edges");

    edges.reserve(static_cast<std::size_t>(ncm_estimate*1.1));

    std::uniform_real_distribution<> rd;

    const double lp = std::log(1.0 - edge_prob);
    double lr = std::log(1.0 - rd(generator));
    auto carry = static_cast<std::size_t>(std::floor(lr/lp));

    std::vector<VertT> current(static_cast<std::size_t>(edge_degree));
    bool finished = false;

    while (!finished) {
      for (std::size_t i{}; i < static_cast<std::size_t>(edge_degree); i++) {
        std::size_t temp_sum = static_cast<std::size_t>(current[i]) + carry;
        current[i] = static_cast<VertT>(
          temp_sum % static_cast<std::size_t>(size));
        carry = temp_sum/static_cast<std::size_t>(size);
      }

      if (carry > 0) {
        finished = true;
      } else {
        if (std::ranges::adjacent_find(
            current, std::greater_equal<VertT>{}) == current.end())
          edges.emplace_back(current);
      }

      lr = std::log(1.0 - rd(generator));
      carry = static_cast<std::size_t>(1 + std::floor(lr/lp));
    }

    return undirected_hypernetwork<VertT>(edges,
        views::iota(VertT{}, size));
  }

  template <integer_network_vertex VertT, std::uniform_random_bit_generator Gen>
  directed_hypernetwork<VertT>
  random_directed_uniform_hypergraph(
      VertT size, VertT edge_in_degree,
      VertT edge_out_degree, double edge_prob,
      Gen& generator) {
    if (size < 0)
      throw std::domain_error("size must be non-negative");

    if (edge_prob > 1.0 || edge_prob < 0.0)
      throw std::invalid_argument(
          "edge probability should be in [0,1] interval");

    if (edge_in_degree > size)
      throw std::invalid_argument(
          "edge_in_degree must be less than or equal to size");

    if (edge_out_degree > size)
      throw std::invalid_argument(
          "edge_out_degree must be less than or equal to size");

    if (size == 0 || edge_prob == 0)
      return directed_hypernetwork<VertT>(
          ranges::empty_view<directed_hyperedge<VertT>>(),
          views::iota(VertT{}, size));

    VertT edge_degree = edge_in_degree + edge_out_degree;

    std::vector<directed_hyperedge<VertT>> edges;
    double logncm = std::lgamma(size + 1) -
                      std::lgamma(edge_in_degree + 1) -
                      std::lgamma(size - edge_in_degree + 1);
    logncm += std::lgamma(size + 1) -
                std::lgamma(edge_out_degree + 1) -
                std::lgamma(size - edge_out_degree + 1);

    double ncm_estimate = std::exp(logncm + std::log(edge_prob));

    if (ncm_estimate > static_cast<double>(
        std::numeric_limits<std::size_t>::max()))
      throw std::invalid_argument(
          "cannot generate a hypergraph with that many edges");

    edges.reserve(static_cast<std::size_t>(ncm_estimate*1.1));

    std::uniform_real_distribution<> rd;

    const double lp = std::log(1.0 - edge_prob);
    double lr = std::log(1.0 - rd(generator));
    auto carry = static_cast<std::size_t>(std::floor(lr/lp));

    std::vector<VertT> current(static_cast<std::size_t>(edge_degree));
    bool finished = false;

    while (!finished) {
      for (std::size_t i{}; i < static_cast<std::size_t>(edge_degree); i++) {
        std::size_t temp_sum = static_cast<std::size_t>(current[i]) + carry;
        current[i] = static_cast<VertT>(
          temp_sum % static_cast<std::size_t>(size));
        carry = temp_sum/static_cast<std::size_t>(size);
      }

      if (carry > 0) {
        finished = true;
      } else {
        auto split = current.begin() +
          static_cast<std::ptrdiff_t>(edge_in_degree);
        std::ranges::subrange
          in{current.begin(), split}, out{split, current.end()};
        if (std::ranges::adjacent_find(
              in, std::greater_equal<VertT>{}) == in.end() &&
            std::ranges::adjacent_find(
              out, std::greater_equal<VertT>{}) == out.end())
          edges.emplace_back(in, out);
      }

      lr = std::log(1.0 - rd(generator));
      carry = static_cast<std::size_t>(1 + std::floor(lr/lp));
    }

    return directed_hypernetwork<VertT>(edges,
        views::iota(VertT{}, size));
  }
}  // namespace reticula

#include "algorithms.hpp"
#include <cstddef>
#include <utility>

namespace reticula {
  template <
    integer_network_vertex VertT,
    ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    degree_range<Range> &&
    std::convertible_to<ranges::range_value_t<Range>, VertT>
  undirected_network<VertT>
  random_degree_sequence_graph(
      Range&& degree_sequence,
      Gen& gen) {
    std::optional<undirected_network<VertT>> maybe_g = std::nullopt;
    constexpr std::size_t tries = 1000;
    while (!maybe_g) maybe_g = try_random_degree_sequence_graph<VertT>(
        std::forward<Range>(degree_sequence), gen, tries);
    return *maybe_g;
  }

  template <
    integer_network_vertex VertT,
    ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    degree_range<Range> &&
    std::convertible_to<ranges::range_value_t<Range>, VertT>
  std::optional<undirected_network<VertT>>
  try_random_degree_sequence_graph(
      Range&& degree_sequence,
      Gen& gen,
      std::size_t max_tries) {
    if (!is_graphic(degree_sequence))
      throw std::invalid_argument("degree_sequence is not graphical");

    std::vector<VertT> degrees;
    if constexpr (ranges::sized_range<Range>)
      degrees.reserve(std::size(degree_sequence));

    for (auto d: degree_sequence)
      degrees.push_back(d);

    if (degrees.empty())
      return undirected_network<VertT>();

    VertT degree_sum = std::accumulate(degrees.begin(), degrees.end(), VertT{});

    for (std::size_t t = 0; t < max_tries; t++) {
      std::unordered_set<
        undirected_edge<VertT>,
        hash<undirected_edge<VertT>>> edges;
      edges.reserve(static_cast<std::size_t>(degree_sum)/2);

      std::vector<VertT> stubs;
      if (!degrees.empty())
        stubs = degrees;
      std::vector<VertT> repeated_stubs;
      repeated_stubs.reserve(static_cast<std::size_t>(degree_sum));

      // Initialize repeated_stubs
      for (VertT i = 0; i < static_cast<VertT>(degrees.size()); i++)
        for (VertT j = 0; j < degrees[static_cast<std::size_t>(i)]; j++)
          repeated_stubs.push_back(i);

      auto has_possible_edge = [&edges](const std::vector<VertT>& stubs) {
        std::size_t size = stubs.size();

        if (size < 2)
          return false;

        for (std::size_t i = 0; i < size; i++)
          for (std::size_t j = i + 1; j < size; j++)
            if (stubs[i] != stubs[j] &&
                !edges.contains({stubs[i], stubs[j]}))
              return true;

        return false;
      };

      auto suitable_edges_remaining = has_possible_edge(repeated_stubs);
      while (suitable_edges_remaining) {
        std::size_t current_size = repeated_stubs.size();
        std::uniform_int_distribution<std::size_t> dist_u{0, current_size - 1};
        std::size_t idx_u = dist_u(gen);
        VertT u = repeated_stubs[idx_u];
        std::swap(repeated_stubs[idx_u], repeated_stubs.at(current_size - 1));

        std::uniform_int_distribution<std::size_t> dist_v{0, current_size - 2};
        std::size_t idx_v = dist_v(gen);
        while (repeated_stubs[idx_v] == u) idx_v = dist_v(gen);
        VertT v = repeated_stubs[idx_v];
        std::swap(repeated_stubs[idx_v], repeated_stubs.at(current_size - 2));

        double puv =
          static_cast<double>(
              degrees[static_cast<std::size_t>(u)]*
              degrees[static_cast<std::size_t>(v)])/
            (2.0*static_cast<double>(degree_sum));
        undirected_edge<VertT> new_edge{u, v};

        // Bayati et al. 2009 suggests that the probability of accepting u, v is
        // proportional to remaining degrees of u and v and (1-d[u]d[v]/4m). We
        // have chosen u and v at random with weights proportional to remaining
        // degree, to get the complement out of the way we accept if x > puv
        // where x is randomly drawn in [0, 1).
        if (!edges.contains(new_edge) &&
            std::uniform_real_distribution{}(gen) > puv) {
          edges.insert(new_edge);

          repeated_stubs.pop_back();
          repeated_stubs.pop_back();

          stubs[static_cast<std::size_t>(u)]--;
          stubs[static_cast<std::size_t>(v)]--;

          if (stubs[static_cast<std::size_t>(u)] == 0 ||
              stubs[static_cast<std::size_t>(v)] == 0)
            suitable_edges_remaining = has_possible_edge(repeated_stubs);
        }
      }

      if (repeated_stubs.empty())
        return undirected_network<VertT>(edges,
            views::iota(
              VertT{}, static_cast<VertT>(degrees.size())));
    }
    return std::nullopt;
  }


  template <
    integer_network_vertex VertT,
    ranges::forward_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires
    degree_pair_range<PairRange> &&
    is_pairlike_of<ranges::range_value_t<PairRange>, VertT, VertT>
  directed_network<VertT>
  random_directed_degree_sequence_graph(
      PairRange&& in_out_degree_sequence,
      Gen& gen) {
    std::optional<directed_network<VertT>> maybe_g = std::nullopt;
    constexpr std::size_t tries = 1000;
    while (!maybe_g) maybe_g = try_random_directed_degree_sequence_graph<VertT>(
        std::forward<PairRange>(in_out_degree_sequence), gen, tries);
    return *maybe_g;
  }

  template <
    integer_network_vertex VertT,
    ranges::forward_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires
    degree_pair_range<PairRange> &&
    is_pairlike_of<ranges::range_value_t<PairRange>, VertT, VertT>
  std::optional<directed_network<VertT>>
  try_random_directed_degree_sequence_graph(
      PairRange&& in_out_degree_sequence,
      Gen& gen,
      std::size_t max_tries) {
    // TODO: pass a lazy zip_view of in_degrees and out_degrees instead of
    // reading the sequence twice. This way we can change the forward_range to
    // an input_range.
    if (!is_digraphic(in_out_degree_sequence))
      throw std::invalid_argument("degree_sequence is not digraphical");

    std::vector<VertT> in_degrees, out_degrees;
    if constexpr (ranges::sized_range<PairRange>) {
      in_degrees.reserve(ranges::size(in_out_degree_sequence));
      out_degrees.reserve(ranges::size(in_out_degree_sequence));
    }

    for (auto [in, out]: in_out_degree_sequence) {
      in_degrees.push_back(in);
      out_degrees.push_back(out);
    }

    if (in_degrees.empty())
      return directed_network<VertT>();

    VertT degree_sum =
        std::accumulate(in_degrees.begin(), in_degrees.end(), VertT{});

    if (degree_sum !=
        std::accumulate(out_degrees.begin(), out_degrees.end(), VertT{}))
      throw std::invalid_argument(
          "in- and out-degree sequences should have equal sums");

    for (std::size_t t = 0; t < max_tries; t++) {
      std::unordered_set<
        directed_edge<VertT>,
        hash<directed_edge<VertT>>> edges;
      edges.reserve(static_cast<std::size_t>(degree_sum));

      std::vector<VertT> in_stubs;
      std::vector<VertT> out_stubs;
      if (!in_degrees.empty() && !out_degrees.empty()) {
        in_stubs = in_degrees;
        out_stubs = out_degrees;
      }

      std::vector<VertT> in_repeated_stubs;
      in_repeated_stubs.reserve(static_cast<std::size_t>(degree_sum));
      std::vector<VertT> out_repeated_stubs;
      out_repeated_stubs.reserve(static_cast<std::size_t>(degree_sum));

      for (VertT i = 0; i < static_cast<VertT>(in_degrees.size()); i++) {
        for (VertT j = 0; j < in_degrees[static_cast<std::size_t>(i)]; j++)
          in_repeated_stubs.push_back(i);
        for (VertT j = 0; j < out_degrees[static_cast<std::size_t>(i)]; j++)
          out_repeated_stubs.push_back(i);
      }

      auto has_possible_directed_edge =
          [&edges](const std::vector<VertT>& in_stubs,
                   const std::vector<VertT>& out_stubs) {
            if (out_stubs.empty() || in_stubs.empty())
              return false;

            for (const auto& u : out_stubs)
              for (const auto& v : in_stubs)
                if (u != v && !edges.contains({u, v}))
                  return true;
            return false;
          };

      bool suitable_edges_remaining = has_possible_directed_edge(
          in_repeated_stubs, out_repeated_stubs);
      while (suitable_edges_remaining) {
        std::size_t out_size = out_repeated_stubs.size();
        std::uniform_int_distribution<std::size_t> dist{0, out_size - 1};

        std::size_t idx_u = dist(gen);
        std::size_t idx_v = dist(gen);
        while (out_repeated_stubs[idx_u] == in_repeated_stubs[idx_v]) {
          idx_u = dist(gen);
          idx_v = dist(gen);
        }

        VertT u = out_repeated_stubs[idx_u];
        std::swap(out_repeated_stubs[idx_u], out_repeated_stubs[out_size - 1]);
        VertT v = in_repeated_stubs[idx_v];
        std::swap(in_repeated_stubs[idx_v], in_repeated_stubs[out_size - 1]);

        double puv =
          static_cast<double>(
              in_degrees[static_cast<std::size_t>(v)]*
              out_degrees[static_cast<std::size_t>(u)])/
            (2.0*static_cast<double>(degree_sum));
        directed_edge<VertT> new_edge{u, v};

        // Bayati et al. 2009 suggests that the probability of accepting u, v is
        // proportional to remaining degrees of u and v and (1-d[u]d[v]/4m). We
        // have chosen u and v at random with weights proportional to remaining
        // degree, to get the complement out of the way we accept if x > puv
        // where x is randomly drawn in [0, 1).
        if (!edges.contains(new_edge) &&
            std::uniform_real_distribution{}(gen) > puv) {
          edges.insert(new_edge);

          out_repeated_stubs.pop_back();
          in_repeated_stubs.pop_back();

          out_stubs[static_cast<std::size_t>(u)]--;
          in_stubs[static_cast<std::size_t>(v)]--;

          if (out_stubs[static_cast<std::size_t>(u)] == 0 ||
              in_stubs[static_cast<std::size_t>(v)] == 0)
            suitable_edges_remaining = has_possible_directed_edge(
                in_repeated_stubs, out_repeated_stubs);
        }
      }

      if (in_repeated_stubs.empty() && out_repeated_stubs.empty())
        return directed_network<VertT>(edges,
            views::iota(VertT{}, static_cast<VertT>(in_degrees.size())));
    }
    return std::nullopt;
  }
}  // namespace reticula

#include "generators.hpp"
#include "distributions.hpp"
#include <random>

namespace reticula {
  template <
    integer_network_vertex VertT,
    std::uniform_random_bit_generator Gen>
  undirected_temporal_network<VertT, double>
  random_fully_mixed_temporal_network(
      VertT size, double rate, double max_t, Gen& generator) {
    return random_link_activation_temporal_network<
      undirected_temporal_edge<VertT, double>>(
        complete_graph(size),
        max_t,
        std::exponential_distribution<double>(rate),
        std::exponential_distribution<double>(rate),
        generator,
        static_cast<std::size_t>(max_t*rate + 6*std::sqrt(max_t*rate)));
  }

  template <
    integer_network_vertex VertT,
    std::uniform_random_bit_generator Gen>
  directed_temporal_network<VertT, double>
  random_directed_fully_mixed_temporal_network(
      VertT size, double rate, double max_t, Gen& generator) {
    return random_link_activation_temporal_network<
      directed_temporal_edge<VertT, double>>(
        complete_directed_graph(size),
        max_t,
        std::exponential_distribution<double>(rate),
        std::exponential_distribution<double>(rate),
        generator,
        static_cast<std::size_t>(max_t*rate + 6*std::sqrt(max_t*rate)));
  }

  template <
    temporal_network_edge EdgeT,
    random_number_distribution IETDist,
    random_number_distribution ResDist,
    std::uniform_random_bit_generator Gen>
  requires
    std::same_as<
      typename IETDist::result_type, typename ResDist::result_type> &&
    std::constructible_from<EdgeT,
      typename EdgeT::StaticProjectionType, typename IETDist::result_type>
  network<EdgeT>
  random_link_activation_temporal_network(
      const network<typename EdgeT::StaticProjectionType>& base_net,
      typename EdgeT::TimeType max_t,
      IETDist iet_dist,
      ResDist res_dist,
      Gen& generator,
      std::size_t size_hint) {
    std::vector<EdgeT> edges;
    if (size_hint > 0)
      edges.reserve(size_hint);

    for (const auto& e: base_net.edges())
      for (auto t = res_dist(generator); t < max_t; t += iet_dist(generator))
        edges.emplace_back(e, t);

    return network<EdgeT>(edges, base_net.vertices());
  }

  template <
    temporal_network_edge EdgeT,
    random_number_distribution IETDist,
    std::uniform_random_bit_generator Gen>
  requires
    std::constructible_from<EdgeT,
      typename EdgeT::StaticProjectionType, typename IETDist::result_type>
  network<EdgeT>
  random_link_activation_temporal_network(
      const network<typename EdgeT::StaticProjectionType>& base_net,
      typename EdgeT::TimeType max_t,
      IETDist iet_dist,
      Gen& generator,
      std::size_t size_hint) {
    std::vector<EdgeT> edges;
    if (size_hint > 0)
      edges.reserve(size_hint);

    for (const auto& e: base_net.edges())
      for (typename EdgeT::TimeType t{}; t < max_t*2; t += iet_dist(generator))
        if (t >= max_t) edges.emplace_back(e, t - max_t);

    return network<EdgeT>(edges, base_net.vertices());
  }

  template <
    temporal_network_edge EdgeT,
    random_number_distribution IETDist,
    random_number_distribution ResDist,
    std::uniform_random_bit_generator Gen>
  requires
    std::same_as<
      typename IETDist::result_type, typename ResDist::result_type> &&
    std::constructible_from<EdgeT,
      typename EdgeT::StaticProjectionType, typename IETDist::result_type>
  network<EdgeT>
  random_node_activation_temporal_network(
      const network<typename EdgeT::StaticProjectionType>& base_net,
      typename EdgeT::TimeType max_t,
      IETDist iet_dist,
      ResDist res_dist,
      Gen& generator,
      std::size_t size_hint) {
    std::vector<EdgeT> edges;
    if (size_hint > 0)
      edges.reserve(size_hint);

    for (const auto& v: base_net.vertices()) {
      auto links = base_net.out_edges(v);
      if (links.empty()) continue;

      std::uniform_int_distribution<std::size_t> idx(0, links.size() - 1);
      for (auto t = res_dist(generator); t < max_t; t += iet_dist(generator))
        edges.emplace_back(links[idx(generator)], t);
    }

    return network<EdgeT>(edges, base_net.vertices());
  }

  template <
    temporal_network_edge EdgeT,
    random_number_distribution IETDist,
    std::uniform_random_bit_generator Gen>
  requires
    std::constructible_from<EdgeT,
      typename EdgeT::StaticProjectionType, typename IETDist::result_type>
  network<EdgeT>
  random_node_activation_temporal_network(
      const network<typename EdgeT::StaticProjectionType>& base_net,
      typename EdgeT::TimeType max_t,
      IETDist iet_dist,
      Gen& generator,
      std::size_t size_hint) {
    std::vector<EdgeT> edges;
    if (size_hint > 0)
      edges.reserve(size_hint);

    for (const auto& v: base_net.vertices()) {
      auto links = base_net.out_edges(v);
      if (links.empty()) continue;

      std::uniform_int_distribution<std::size_t> idx(0, links.size() - 1);
      for (typename EdgeT::TimeType t{}; t < max_t*2; t += iet_dist(generator))
        if (t >= max_t) edges.emplace_back(links[idx(generator)], t - max_t);
    }

    return network<EdgeT>(edges, base_net.vertices());
  }
}  // namespace reticula



#endif  // INCLUDE_RETICULA_RANDOM_NETWORKS_HPP_
