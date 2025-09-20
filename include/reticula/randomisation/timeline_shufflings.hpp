#pragma once

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto timeline_shuffling(
  const NetT& net, Gen& generator, double t_start, double t_end,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links = {}) -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto timeline_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links = {}) -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto weight_constrained_timeline_shuffling(const NetT& net, Gen& generator)
  -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto weight_constrained_timeline_shuffling(
  const NetT& net, Gen& generator, double t_start, double t_end)
  -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto activity_constrained_timeline_shuffling(const NetT& net, Gen& generator)
  -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto interval_shuffling(const NetT& net, Gen& generator)
  -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto interval_shuffling(
  const NetT& net, Gen& generator, double t_start, double t_end)
  -> network<typename NetT::EdgeType>;

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto inter_event_shuffling(const NetT& net, Gen& generator)
  -> network<typename NetT::EdgeType>;
} // namespace reticula

#include <reticula/algorithms/properties.hpp>
#include <reticula/operations/addition.hpp>
#include <reticula/operations/projections.hpp>

namespace reticula {
namespace detail {
template <std::uniform_random_bit_generator Gen>
std::vector<double>
sample_timestamps(double t0, double t1, std::size_t k, Gen& generator) {
  std::unordered_set<double> s;
  s.reserve(k);

  // uniform_real_distribution is not end-inclusive.
  std::uniform_real_distribution<double> dist(
    t0, std::nextafter(t1, std::numeric_limits<double>::infinity()));
  while (s.size() < k)
    s.insert(dist(generator));

  return {s.begin(), s.end()};
}
} // namespace detail

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto timeline_shuffling(
  const NetT& net, Gen& generator,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links) -> network<typename NetT::EdgeType> {
  if (net.vertices().empty() || net.edges().empty())
    return net;

  auto [t0, t1] = cause_time_window(net);
  return timeline_shuffling(net, generator, t0, t1, unobserved_links);
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto timeline_shuffling(
  const NetT& net, Gen& generator, double t_start, double t_end,
  const std::vector<typename NetT::EdgeType::StaticProjectionType>&
    unobserved_links) -> network<typename NetT::EdgeType> {
  using EdgeT = typename NetT::EdgeType;
  if (net.vertices().empty() || net.edges().empty())
    return net;

  auto [t0, t1] = cause_time_window(net);
  if (t0 < t_start || t1 > t_end)
    throw std::invalid_argument(
      "observation window does not cover "
      "the cause time of all the events");

  std::vector<EdgeT> shuffled_edges;
  shuffled_edges.reserve(net.edges_cause().size());

  auto p = with_edges(static_projection(net), unobserved_links);
  auto links = p.edges();

  std::unordered_map<
    typename EdgeT::StaticProjectionType, std::vector<EdgeT>,
    hash<typename EdgeT::StaticProjectionType>>
    shuffled_link;
  std::uniform_int_distribution<std::size_t> dist(0, links.size() - 1);
  for (auto& l : links)
    shuffled_link[l].reserve(net.edges().size() * 2 / links.size());

  for (auto& e : net.edges_cause())
    shuffled_link[links[dist(generator)]].push_back(e);

  for (auto& [l, tl] : shuffled_link) {
    auto ts = detail::sample_timestamps(t_start, t_end, tl.size(), generator);
    for (const auto& [e, t] : std::views::zip(tl, ts))
      if constexpr (!is_instantaneous_v<EdgeT>)
        shuffled_edges.emplace_back(l, t, t + e.effect_time() - e.cause_time());
      else
        shuffled_edges.emplace_back(l, t);
  }

  return network<EdgeT>(shuffled_edges, net.vertices());
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto weight_constrained_timeline_shuffling(const NetT& net, Gen& generator)
  -> network<typename NetT::EdgeType> {
  if (net.vertices().empty() || net.edges().empty())
    return net;

  auto [t0, t1] = cause_time_window(net);
  return weight_constrained_timeline_shuffling(net, generator, t0, t1);
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto weight_constrained_timeline_shuffling(
  const NetT& net, Gen& generator, double t_start, double t_end)
  -> network<typename NetT::EdgeType> {
  using EdgeT = typename NetT::EdgeType;
  if (net.vertices().empty() || net.edges().empty())
    return net;

  auto [t0, t1] = cause_time_window(net);
  if (t0 < t_start || t1 > t_end)
    throw std::invalid_argument(
      "observation window does not cover "
      "the cause time of all the events");

  std::vector<EdgeT> shuffled_edges;
  shuffled_edges.reserve(net.edges_cause().size());

  for (auto sp = static_projection(net); auto& l : sp.edges()) {
    auto timeline = link_timeline(net, l);
    auto ts =
      detail::sample_timestamps(t_start, t_end, timeline.size(), generator);
    for (const auto& [e, t] : std::views::zip(timeline, ts))
      if constexpr (!is_instantaneous_v<EdgeT>)
        shuffled_edges.emplace_back(l, t, t + e.effect_time() - e.cause_time());
      else
        shuffled_edges.emplace_back(l, t);
  }

  return network<EdgeT>(shuffled_edges, net.vertices());
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto activity_constrained_timeline_shuffling(const NetT& net, Gen& generator)
  -> network<typename NetT::EdgeType> {
  using EdgeT = typename NetT::EdgeType;
  std::vector<EdgeT> shuffled_edges;
  shuffled_edges.reserve(net.edges_cause().size());

  for (auto sp = static_projection(net); const auto& link : sp.edges()) {
    auto timeline = link_timeline(net, link);
    auto ctimes = timeline | std::views::transform(
                               [](const auto& e) { return e.cause_time(); });
    auto [t_start, t_end] = std::ranges::minmax(ctimes);
    auto ts =
      detail::sample_timestamps(t_start, t_end, timeline.size(), generator);
    for (const auto& [e, t] : std::views::zip(timeline, ts))
      if constexpr (!is_instantaneous_v<EdgeT>)
        shuffled_edges.emplace_back(
          e.static_projection(), t, t + e.effect_time() - e.cause_time());
      else
        shuffled_edges.emplace_back(e.static_projection(), t);
  }

  return network<EdgeT>(shuffled_edges, net.vertices());
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto interval_shuffling(const NetT& net, Gen& generator)
  -> network<typename NetT::EdgeType> {
  if (net.vertices().empty() || net.edges().empty())
    return net;

  auto [t0, t1] = cause_time_window(net);
  return interval_shuffling(net, generator, t0, t1);
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto interval_shuffling(
  const NetT& net, Gen& generator, double t_start, double t_end)
  -> network<typename NetT::EdgeType> {
  using EdgeT = typename NetT::EdgeType;
  if (net.vertices().empty() || net.edges().empty())
    return net;

  auto [t0, t1] = cause_time_window(net);
  if (t0 < t_start || t1 > t_end)
    throw std::invalid_argument(
      "observation window does not cover "
      "the cause time of all the events");

  std::vector<EdgeT> shuffled_edges;
  shuffled_edges.reserve(net.edges_cause().size());

  auto sp = static_projection(net);
  for (auto& link : sp.edges()) {
    auto timeline = link_timeline(net, link);

    std::vector<double> iets;
    iets.reserve(timeline.size());

    for (std::size_t i = 1; i < timeline.size(); i++)
      iets.push_back(timeline[i].cause_time() - timeline[i - 1].cause_time());

    std::ranges::shuffle(iets, generator);
    std::partial_sum(iets.begin(), iets.end(), iets.begin());

    auto ctimes = timeline | std::views::transform(
                               [](const auto& e) { return e.cause_time(); });
    auto [start_t, end_t] = std::ranges::minmax(ctimes);
    double interval = end_t - start_t;

    auto start = std::uniform_real_distribution<double>(
      t_start, t_end - interval)(generator);
    shuffled_edges.push_back(timeline.front());

    for (std::size_t i = 1; i < timeline.size(); i++) {
      auto e = timeline[i];
      if constexpr (!is_instantaneous_v<EdgeT>)
        shuffled_edges.emplace_back(
          e.static_projection(), iets[i - 1] + start,
          iets[i - 1] + start + e.effect_time() - e.cause_time());
      else
        shuffled_edges.emplace_back(e.static_projection(), iets[i - 1] + start);
    }
  }

  return network<EdgeT>(shuffled_edges, net.vertices());
}

template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
auto inter_event_shuffling(const NetT& net, Gen& generator)
  -> network<typename NetT::EdgeType> {
  using EdgeT = typename NetT::EdgeType;
  std::vector<EdgeT> shuffled_edges;
  shuffled_edges.reserve(net.edges_cause().size());

  auto sp = static_projection(net);
  for (auto& link : sp.edges()) {
    auto timeline = link_timeline(net, link);

    std::vector<double> iets;
    iets.reserve(timeline.size());

    for (std::size_t i = 1; i < timeline.size(); i++)
      iets.push_back(timeline[i].cause_time() - timeline[i - 1].cause_time());

    std::ranges::shuffle(iets, generator);
    std::partial_sum(iets.begin(), iets.end(), iets.begin());

    auto t_start = timeline.front().cause_time();
    shuffled_edges.push_back(timeline.front());

    for (std::size_t i = 1; i < timeline.size(); i++) {
      auto e = timeline[i];
      if constexpr (!is_instantaneous_v<EdgeT>)
        shuffled_edges.emplace_back(
          e.static_projection(), iets[i - 1] + t_start,
          iets[i - 1] + t_start + e.effect_time() - e.cause_time());
      else
        shuffled_edges.emplace_back(
          e.static_projection(), iets[i - 1] + t_start);
    }
  }

  return network<EdgeT>(shuffled_edges, net.vertices());
}

} // namespace reticula
