#include <nanobind/nanobind.h>
#include <nanobind/stl/vector.h>

#include <reticula/algorithms/vertex_degree.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_vertex_degree_for_net(nanobind::module_& m) {
  m.def(
    "in_degree", reticula::in_degree<NetT>, nanobind::arg("net"),
    nanobind::arg("v"));
  m.def(
    "out_degree", reticula::out_degree<NetT>, nanobind::arg("net"),
    nanobind::arg("v"));
  m.def(
    "incident_degree", reticula::incident_degree<NetT>, nanobind::arg("net"),
    nanobind::arg("v"));

  if constexpr (is_undirected_v<NetT>)
    m.def(
      "degree", reticula::degree<NetT>, nanobind::arg("net"),
      nanobind::arg("v"));

  m.def(
    "in_degree_sequence", reticula::in_degree_sequence<NetT>,
    nanobind::arg("net"));
  m.def(
    "out_degree_sequence", reticula::out_degree_sequence<NetT>,
    nanobind::arg("net"));
  m.def(
    "incident_degree_sequence", reticula::incident_degree_sequence<NetT>,
    nanobind::arg("net"));
  if constexpr (is_undirected_v<NetT>)
    m.def(
      "degree_sequence", reticula::degree_sequence<NetT>, nanobind::arg("net"));
  m.def(
    "in_out_degree_pair_sequence", reticula::in_out_degree_pair_sequence<NetT>,
    nanobind::arg("net"));
}
} // namespace

void define_vertex_degree(nanobind::module_& m) {
  define_vertex_degree_for_net<undirected_network>(m);
  define_vertex_degree_for_net<directed_network>(m);
  define_vertex_degree_for_net<undirected_temporal_network>(m);
  define_vertex_degree_for_net<directed_temporal_network>(m);
  define_vertex_degree_for_net<directed_delayed_temporal_network>(m);

  define_vertex_degree_for_net<undirected_hypernetwork>(m);
  define_vertex_degree_for_net<directed_hypernetwork>(m);
  define_vertex_degree_for_net<undirected_temporal_hypernetwork>(m);
  define_vertex_degree_for_net<directed_temporal_hypernetwork>(m);
  define_vertex_degree_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
