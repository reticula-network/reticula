#ifndef INCLUDE_DAG_RANDOM_NETWORKS_HPP_
#define INCLUDE_DAG_RANDOM_NETWORKS_HPP_

#include <random>
#include <concepts>
#include <numeric>

#include "network_concepts.hpp"

namespace dag {
  template <typename T>
  concept random_number_distribution =
    std::is_arithmetic_v<typename T::result_type> &&
    requires(T dist, std::mt19937_64& gen) {
      { dist(gen) } -> std::convertible_to<typename T::result_type>;
    };  // NOLINT(readability/braces)

  template <network_vertex VertT, std::uniform_random_bit_generator Gen>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> gnp_random_graph(
      VertT n, double p,
      Gen& generator);

  template <network_vertex VertT, std::uniform_random_bit_generator Gen>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> ba_random_graph(
      VertT n, VertT m,
      Gen& generator);

  template <network_vertex VertT, std::uniform_random_bit_generator Gen>
  requires std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> random_regular_graph(
      VertT size, VertT degree,
      Gen& generator);


  template <
    network_vertex VertT,
    std::ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    std::convertible_to<std::ranges::range_value_t<Range>, VertT> &&
    std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> random_degree_sequence_graph(
      const Range& degree_sequence,
      Gen& generator);

  template <
    network_vertex VertT,
    std::ranges::forward_range Range1,
    std::ranges::forward_range Range2,
    std::uniform_random_bit_generator Gen>
  requires
    std::convertible_to<std::ranges::range_value_t<Range1>, VertT> &&
    std::convertible_to<std::ranges::range_value_t<Range2>, VertT> &&
    std::numeric_limits<VertT>::is_integer
  directed_network<VertT> directed_random_degree_sequence_graph(
      const Range1& in_degree_sequence,
      const Range2& out_degree_sequence,
      Gen& generator);


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
    network_vertex VertT,
    std::ranges::forward_range Range,
    std::uniform_random_bit_generator Gen>
  requires
    std::is_arithmetic_v<std::ranges::range_value_t<Range>> &&
    std::numeric_limits<VertT>::is_integer
  undirected_network<VertT> random_expected_degree_sequence_graph(
      const Range& degree_sequence,
      Gen& generator,
      bool self_loops=false);

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
    `dag::random_expected_degree_sequence_graph`) with link probabilities taking
    into account out-degree weight of the tail node and in-degree weight of the
    possible head node.

    Chung, Fan, and Linyuan Lu. "Connected components in random graphs with
    given expected degree sequences." Annals of combinatorics 6.2 (2002):
    125-145.

    Miller, Joel C., and Aric Hagberg. "Efficient generation of networks with
    given expected degrees." International Workshop on Algorithms and Models for
    the Web-Graph. Springer, Berlin, Heidelberg, 2011.
  */
  template <
    network_vertex VertT,
    std::ranges::forward_range Range1,
    std::ranges::forward_range Range2,
    std::uniform_random_bit_generator Gen>
  requires
    std::is_arithmetic_v<std::ranges::range_value_t<Range1>> &&
    std::is_arithmetic_v<std::ranges::range_value_t<Range2>> &&
    std::numeric_limits<VertT>::is_integer
  directed_network<VertT> random_directed_expected_degree_sequence_graph(
      const Range1& in_degree_sequence,
      const Range2& out_degree_sequence,
      Gen& generator,
      bool self_loops=false);

  /**
    Random events with the given `inter_event_time_dist` and
    `residual_time_dist` for generating time to first event.
    */
  template <
    temporal_edge EdgeT,
    random_number_distribution Distribution,
    random_number_distribution ResDistribution,
    std::uniform_random_bit_generator Gen>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      ResDistribution residual_time_dist,
      Gen& generator,
      std::size_t size_hint = 0);

  /**
    Random events with the given inter-event time distribution and `max_t`
    units of time burn-in before being recorded.
    */
  template <
    temporal_edge EdgeT,
    random_number_distribution Distribution,
    std::uniform_random_bit_generator Gen>
  std::vector<EdgeT>
  random_events(
      const undirected_network<typename EdgeT::VertexType>& base_net,
      typename EdgeT::TimeType max_t,
      Distribution inter_event_time_dist,
      Gen& generator,
      std::size_t size_hint = 0);


  template <class RealType = double>
  class power_law_with_specified_mean {
  public:
    using result_type = RealType;

    power_law_with_specified_mean(RealType exponent, RealType mean);

    template <class Generator>
    RealType operator()(Generator& g) const;

    RealType x_min() const;
    RealType exponent() const;
    RealType mean() const;
  private:
    RealType _x_min, _exponent, _mean;
  };

  template <class RealType = double>
  class residual_power_law_with_specified_mean {
  public:
    using result_type = RealType;

    residual_power_law_with_specified_mean(RealType exponent, RealType mean);

    template <class Generator>
    RealType operator()(Generator& g) const;

    RealType x_min() const;
    RealType exponent() const;
    RealType mean() const;
  private:
    RealType _x_min, _exponent, _mean;
  };

  template <class RealType = double>
  class hawkes_univariate_exponential {
  public:
    using result_type = RealType;

    hawkes_univariate_exponential(
        RealType mu, RealType alpha, RealType theta, RealType phi = {});

    template <class Generator>
    RealType operator()(Generator& g);

    RealType mu() const;
    RealType alpha() const;
    RealType theta() const;
    RealType phi() const;
  private:
    RealType _mu, _alpha, _theta, _phi;
  };
}  // namespace dag

#include "../../src/random_networks.tpp"

#endif  // INCLUDE_DAG_RANDOM_NETWORKS_HPP_
