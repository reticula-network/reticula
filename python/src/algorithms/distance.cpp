#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/unordered_map.h>

#include <reticula/algorithms/distance.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_distance_for_net(nanobind::module_& m) {
  m.def(
    "shortest_path_lengths_from", reticula::shortest_path_lengths_from<NetT>,
    nanobind::arg("net"), nanobind::arg("source"),
    nanobind::arg("size_hint") = 0);
  m.def(
    "shortest_path_lengths_to", reticula::shortest_path_lengths_to<NetT>,
    nanobind::arg("net"), nanobind::arg("destination"),
    nanobind::arg("size_hint") = 0);

  m.def("try_diameter", reticula::try_diameter<NetT>, nanobind::arg("net"));
  m.def("diameter", reticula::diameter<NetT>, nanobind::arg("net"));

  m.def(
    "try_out_eccentricity", reticula::try_out_eccentricity<NetT>,
    nanobind::arg("net"), nanobind::arg("v"));
  m.def(
    "out_eccentricity", reticula::out_eccentricity<NetT>, nanobind::arg("net"),
    nanobind::arg("v"));
  m.def(
    "try_in_eccentricity", reticula::try_in_eccentricity<NetT>,
    nanobind::arg("net"), nanobind::arg("v"));
  m.def(
    "in_eccentricity", reticula::in_eccentricity<NetT>, nanobind::arg("net"),
    nanobind::arg("v"));

  m.def("try_out_radius", reticula::try_out_radius<NetT>, nanobind::arg("net"));
  m.def("out_radius", reticula::out_radius<NetT>, nanobind::arg("net"));
  m.def("try_in_radius", reticula::try_in_radius<NetT>, nanobind::arg("net"));
  m.def("in_radius", reticula::in_radius<NetT>, nanobind::arg("net"));

  if constexpr (is_undirected_v<NetT>) {
    m.def(
      "try_eccentricity", reticula::try_eccentricity<NetT>,
      nanobind::arg("net"), nanobind::arg("v"));
    m.def(
      "eccentricity", reticula::eccentricity<NetT>, nanobind::arg("net"),
      nanobind::arg("v"));
    m.def("try_radius", reticula::try_radius<NetT>, nanobind::arg("net"));
    m.def("radius", reticula::radius<NetT>, nanobind::arg("net"));
  }
}
} // namespace

void define_distance(nanobind::module_& m) {
  define_distance_for_net<undirected_network>(m);
  define_distance_for_net<directed_network>(m);
  define_distance_for_net<undirected_temporal_network>(m);
  define_distance_for_net<directed_temporal_network>(m);
  define_distance_for_net<directed_delayed_temporal_network>(m);

  define_distance_for_net<undirected_hypernetwork>(m);
  define_distance_for_net<directed_hypernetwork>(m);
  define_distance_for_net<undirected_temporal_hypernetwork>(m);
  define_distance_for_net<directed_temporal_hypernetwork>(m);
  define_distance_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
