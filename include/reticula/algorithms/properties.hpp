#pragma once

#include <vector>
#include <format>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula {
template <network_like NetT>
  requires is_dyadic_v<NetT> && static_network_edge<typename NetT::EdgeType>
[[nodiscard]] auto density(const NetT& net) -> double;

struct window {
  TimeType start;
  TimeType end;

  [[nodiscard]] auto duration() const noexcept -> TimeType;
  [[nodiscard]] auto duration_is_valid() const noexcept -> bool;
  [[nodiscard]] auto contains(TimeType t) const noexcept -> bool;
  [[nodiscard]] auto contains(const window& other) const noexcept -> bool;
  [[nodiscard]] auto overlaps(const window& other) const noexcept -> bool;
  [[nodiscard]] auto intersection_with(const window& other) const noexcept
    -> window;
  [[nodiscard]] auto union_with(const window& other) const noexcept -> window;

  [[nodiscard]] auto operator==(const window& other) const noexcept -> bool;
};

template <network_like NetT>
  requires temporal_network_edge<typename NetT::EdgeType>
[[nodiscard]] auto time_window(const NetT& net) -> window;

template <network_like NetT>
  requires temporal_network_edge<typename NetT::EdgeType>
[[nodiscard]]
auto cause_time_window(const NetT& net) -> window;

template <network_like NetT>
  requires temporal_network_edge<typename NetT::EdgeType>
[[nodiscard]]
auto effect_time_window(const NetT& net) -> window;

template <network_like NetT>
  requires temporal_network_edge<typename NetT::EdgeType>
[[nodiscard]]
auto link_timeline(
  const NetT& net, const typename NetT::EdgeType::StaticProjectionType& link)
  -> std::vector<typename NetT::EdgeType>;
} // namespace reticula

namespace reticula {
template <network_like NetT>
  requires is_dyadic_v<NetT> && static_network_edge<typename NetT::EdgeType>
auto density(const NetT& net) -> double {
  auto n = static_cast<double>(net.vertices().size());
  if constexpr (is_directed_v<NetT>)
    return static_cast<double>(net.edges().size()) / (n * (n - 1));
  else
    return static_cast<double>(net.edges().size()) / (n * (n - 1) / 2.0);
}

template <network_like NetT>
  requires temporal_network_edge<typename NetT::EdgeType>
auto time_window(const NetT& net) -> window {
  if (net.edges().empty())
    throw std::invalid_argument(
      "time window of a temporal network with no events is undefined.");
  return {
    net.edges_cause().front().cause_time(),
    net.edges_effect().back().effect_time()};
}

template <network_like NetT>
  requires temporal_network_edge<typename NetT::EdgeType>
auto cause_time_window(const NetT& net) -> window {
  if (net.edges_cause().empty())
    throw std::invalid_argument(
      "time window of a temporal network with no events is undefined.");
  return {
    net.edges_cause().front().cause_time(),
    net.edges_cause().back().cause_time()};
}

template <network_like NetT>
  requires temporal_network_edge<typename NetT::EdgeType>
auto effect_time_window(const NetT& net) -> window {
  if (net.edges_effect().empty())
    throw std::invalid_argument(
      "time window of a temporal network with no events is undefined.");
  return {
    net.edges_effect().front().effect_time(),
    net.edges_effect().back().effect_time()};
}

template <network_like NetT>
  requires temporal_network_edge<typename NetT::EdgeType>
auto link_timeline(
  const NetT& net, const typename NetT::EdgeType::StaticProjectionType& link)
  -> std::vector<typename NetT::EdgeType> {
  auto node = std::ranges::min(
    link.mutator_verts(), std::ranges::less{},
    [&net](const auto& n) { return net.out_edges(n).size(); });

  std::vector<typename NetT::EdgeType> res;
  res.reserve(
    std::min(
      net.edges().size() * 3 / net.vertices().size(),
      net.out_edges(node).size()));

  for (auto& event : net.out_edges(node))
    if (event.static_projection() == link)
      res.push_back(event);

  return res;
}
} // namespace reticula

namespace std {
template <>
struct formatter<reticula::window> {
  constexpr auto parse(format_parse_context& ctx) {
    auto it = ctx.begin();
    if (it != ctx.end() && *it == '}')
      return it;
    throw format_error("Invalid format for reticula::window");
  }

  template <typename FormatContext>
  auto format(const reticula::window& w, FormatContext& ctx) const {
    return format_to(ctx.out(), "({}, {}]", w.start, w.end);
  }
};
} // namespace std
