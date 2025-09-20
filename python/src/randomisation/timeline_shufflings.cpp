#include <nanobind/nanobind.h>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/randomisation/timeline_shufflings.hpp>

namespace reticula::python {
namespace {
template <temporal_network_like NetT, std::uniform_random_bit_generator Gen>
  requires is_dyadic_v<NetT>
void define_timeline_shufflings_for_net(nanobind::module_& m) {
  m.def(
    "timeline_shuffling",
    [](
      const NetT& net, Gen& generator, double t_start, double t_end,
      const std::vector<typename NetT::EdgeType::StaticProjectionType>&
        unobserved_links = {}) {
      return reticula::timeline_shuffling(
        net, generator, t_start, t_end, unobserved_links);
    },
    nanobind::arg("net"), nanobind::arg("generator"), nanobind::arg("t_start"),
    nanobind::arg("t_end"),
    nanobind::arg("unobserved_links") =
      std::vector<typename NetT::EdgeType::StaticProjectionType>{});

  m.def(
    "timeline_shuffling",
    [](
      const NetT& net, Gen& generator,
      const std::vector<typename NetT::EdgeType::StaticProjectionType>&
        unobserved_links) {
      return reticula::timeline_shuffling(net, generator, unobserved_links);
    },
    nanobind::arg("net"), nanobind::arg("generator"),
    nanobind::arg("unobserved_links") =
      std::vector<typename NetT::EdgeType::StaticProjectionType>{});

  m.def(
    "weight_constrained_timeline_shuffling",
    [](const NetT& net, Gen& generator) {
      return reticula::weight_constrained_timeline_shuffling(net, generator);
    },
    nanobind::arg("net"), nanobind::arg("generator"));

  m.def(
    "weight_constrained_timeline_shuffling",
    [](const NetT& net, Gen& generator, double t_start, double t_end) {
      return reticula::weight_constrained_timeline_shuffling(
        net, generator, t_start, t_end);
    },
    nanobind::arg("net"), nanobind::arg("generator"), nanobind::arg("t_start"),
    nanobind::arg("t_end"));

  m.def(
    "activity_constrained_timeline_shuffling",
    &reticula::activity_constrained_timeline_shuffling<NetT, Gen>,
    nanobind::arg("net"), nanobind::arg("generator"));

  m.def(
    "interval_shuffling",
    [](const NetT& net, Gen& generator) {
      return reticula::interval_shuffling(net, generator);
    },
    nanobind::arg("net"), nanobind::arg("generator"));

  m.def(
    "interval_shuffling",
    [](const NetT& net, Gen& generator, double t_start, double t_end) {
      return reticula::interval_shuffling(net, generator, t_start, t_end);
    },
    nanobind::arg("net"), nanobind::arg("generator"), nanobind::arg("t_start"),
    nanobind::arg("t_end"));

  m.def(
    "inter_event_shuffling", &reticula::inter_event_shuffling<NetT, Gen>,
    nanobind::arg("net"), nanobind::arg("generator"));
}
} // namespace

void define_timeline_shufflings(nanobind::module_& m) {
  define_timeline_shufflings_for_net<
    undirected_temporal_network, std::mt19937_64>(m);
  define_timeline_shufflings_for_net<
    directed_temporal_network, std::mt19937_64>(m);
  define_timeline_shufflings_for_net<
    directed_delayed_temporal_network, std::mt19937_64>(m);
}
} // namespace reticula::python
