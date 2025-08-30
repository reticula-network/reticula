#include <nanobind/nanobind.h>

#include <reticula/algorithms/static_reachability.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_static_reachability_for_net(nanobind::module_& m) {
  m.def(
    "weakly_connected_components", &weakly_connected_components<NetT>,
    nanobind::arg("net"), nanobind::arg("singletons") = true);
  m.def(
    "largest_weakly_connected_component",
    &largest_weakly_connected_component<NetT>, nanobind::arg("net"));
  m.def(
    "weakly_connected_component", &weakly_connected_component<NetT>,
    nanobind::arg("net"), nanobind::arg("v"));
  m.def(
    "is_weakly_connected", &is_weakly_connected<NetT>, nanobind::arg("net"));
  m.def(
    "is_weakly_reachable", &is_weakly_reachable<NetT>, nanobind::arg("net"),
    nanobind::arg("v1"), nanobind::arg("v2"));

  m.def("in_components", &in_components<NetT>, nanobind::arg("net"));
  m.def("out_components", &out_components<NetT>, nanobind::arg("net"));
  m.def(
    "in_component", &in_component<NetT>, nanobind::arg("net"),
    nanobind::arg("v"));
  m.def(
    "out_component", &out_component<NetT>, nanobind::arg("net"),
    nanobind::arg("v"));
  m.def("in_component_sizes", &in_component_sizes<NetT>, nanobind::arg("net"));
  m.def(
    "out_component_sizes", &out_component_sizes<NetT>, nanobind::arg("net"));
  m.def(
    "in_component_size_estimates", &in_component_size_estimates<NetT>,
    nanobind::arg("net"));
  m.def(
    "out_component_size_estimates", &out_component_size_estimates<NetT>,
    nanobind::arg("net"));

  m.def(
    "strongly_connected_components", &strongly_connected_components<NetT>,
    nanobind::arg("net"), nanobind::arg("singletons") = true);
  m.def(
    "largest_strongly_connected_component",
    &largest_strongly_connected_component<NetT>, nanobind::arg("net"));
  m.def(
    "strongly_connected_component", &strongly_connected_component<NetT>,
    nanobind::arg("net"), nanobind::arg("v"));
  m.def(
    "is_strongly_connected", &is_strongly_connected<NetT>,
    nanobind::arg("net"));
  m.def(
    "is_strongly_reachable", &is_strongly_reachable<NetT>, nanobind::arg("net"),
    nanobind::arg("v1"), nanobind::arg("v2"));

  if constexpr (is_undirected_v<NetT>) {
    m.def(
      "connected_components", &connected_components<NetT>, nanobind::arg("net"),
      nanobind::arg("singletons") = true);
    m.def(
      "largest_connected_component", &largest_connected_component<NetT>,
      nanobind::arg("net"));
    m.def(
      "connected_component", &connected_component<NetT>, nanobind::arg("net"),
      nanobind::arg("v"));
    m.def("is_connected", &is_connected<NetT>, nanobind::arg("net"));
    m.def(
      "is_reachable", &is_reachable<NetT>, nanobind::arg("net"),
      nanobind::arg("source"), nanobind::arg("destination"));
  }
}
} // namespace

void define_static_reachability(nanobind::module_& m) {
  define_static_reachability_for_net<undirected_network>(m);
  define_static_reachability_for_net<directed_network>(m);
  define_static_reachability_for_net<undirected_hypernetwork>(m);
  define_static_reachability_for_net<directed_hypernetwork>(m);
}
} // namespace reticula::python
