#ifndef INCLUDE_RETICULA_RANDOM_NETWORKS_HPP_
#define INCLUDE_RETICULA_RANDOM_NETWORKS_HPP_

#include <concepts>
#include <optional>
#include <numeric>

#include "distributions.hpp"
#include "network_concepts.hpp"
#include "networks.hpp"

namespace reticula {
  template <integer_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT> random_gnp_graph(
      VertT n, double p,
      Gen& generator);


  template <integer_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT> random_barabasi_albert_graph(
      VertT n, VertT m,
      Gen& generator);


  template <integer_vertex VertT, std::uniform_random_bit_generator Gen>
  undirected_network<VertT>
  random_regular_graph(
      VertT size, VertT degree,
      Gen& generator);

  template <integer_vertex VertT, std::uniform_random_bit_generator Gen>
  std::optional<undirected_network<VertT>>
  try_random_regular_graph(
      VertT size, VertT degree,
      Gen& generator,
      std::size_t max_tries);


  template <
    integer_vertex VertT,
    std::ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    degree_range<Range> &&
    std::convertible_to<std::ranges::range_value_t<Range>, VertT>
  undirected_network<VertT>
  random_degree_sequence_graph(
      Range&& degree_sequence,
      Gen& generator);

  template <
    integer_vertex VertT,
    std::ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    degree_range<Range> &&
    std::convertible_to<std::ranges::range_value_t<Range>, VertT>
  std::optional<undirected_network<VertT>>
  try_random_degree_sequence_graph(
      Range&& degree_sequence,
      Gen& generator,
      std::size_t max_tries);


  template <
    integer_vertex VertT,
    std::ranges::forward_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires
    degree_pair_range<PairRange> &&
    is_pairlike_of<std::ranges::range_value_t<PairRange>, VertT, VertT>
  directed_network<VertT>
  random_directed_degree_sequence_graph(
      PairRange&& in_out_degree_sequence,
      Gen& generator);

  template <
    integer_vertex VertT,
    std::ranges::forward_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires
    degree_pair_range<PairRange> &&
    is_pairlike_of<std::ranges::range_value_t<PairRange>, VertT, VertT>
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

    Additionally, existance of each two edges in the generated graph is
    independent of each other.

    Chung, Fan, and Linyuan Lu. "Connected components in random graphs with
    given expected degree sequences." Annals of combinatorics 6.2 (2002):
    125-145.

    Miller, Joel C., and Aric Hagberg. "Efficient generation of networks with
    given expected degrees." International Workshop on Algorithms and Models for
    the Web-Graph. Springer, Berlin, Heidelberg, 2011.
  */
  template <
    integer_vertex VertT,
    std::ranges::input_range Range,
    std::uniform_random_bit_generator Gen>
  requires weight_range<Range>
  undirected_network<VertT>
  random_expected_degree_sequence_graph(
      Range&& degree_sequence,
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

    Additionally, existance of each two edges in the generated graph is
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
    integer_vertex VertT,
    std::ranges::input_range PairRange,
    std::uniform_random_bit_generator Gen>
  requires weight_pair_range<PairRange>
  directed_network<VertT>
  random_directed_expected_degree_sequence_graph(
      PairRange&& in_out_degree_sequence,
      Gen& generator,
      bool self_loops = false);


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
    integer_vertex VertT,
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
    integer_vertex VertT,
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
    @param iet_dist random_number_distribution that generates inter-event times.
    @param res_dist random_number_distribution that generates time of the first
    event for each link.
    @param generator A uniform random bit generator
    @param size_hint Estimated number of edges in the final temporal network.
  */
  template <
    temporal_edge EdgeT,
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
    @param iet_dist random_number_distribution that generates inter-event times.
    @param generator A uniform random bit generator
    @param size_hint Estimated number of edges in the final temporal network.
  */
  template <
    temporal_edge EdgeT,
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
}  // namespace reticula

#include "../../src/random_networks.tpp"

#endif  // INCLUDE_RETICULA_RANDOM_NETWORKS_HPP_
