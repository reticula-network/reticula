#include <nanobind/nanobind.h>

#include <reticula/algorithms/edge_degree.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_edge_degree_for_net(nanobind::module_& m) {
  m.def(
    "edge_in_degree", reticula::edge_in_degree<typename NetT::EdgeType>,
    nanobind::arg("edge"));
  m.def(
    "edge_out_degree", reticula::edge_out_degree<typename NetT::EdgeType>,
    nanobind::arg("edge"));
  m.def(
    "edge_incident_degree", reticula::edge_incident_degree<typename NetT::EdgeType>,
    nanobind::arg("edge"));
  if constexpr (is_undirected_v<NetT>)
    m.def(
      "edge_degree", reticula::edge_degree<typename NetT::EdgeType>,
      nanobind::arg("edge"));
  m.def(
    "edge_in_degree_sequence", reticula::edge_in_degree_sequence<NetT>,
    nanobind::arg("net"));
  m.def(
    "edge_out_degree_sequence", reticula::edge_out_degree_sequence<NetT>,
    nanobind::arg("net"));
  m.def(
    "edge_in_out_degree_pair_sequence",
    reticula::edge_in_out_degree_pair_sequence<NetT>, nanobind::arg("net"));
  m.def(
    "edge_incident_degree_sequence", reticula::edge_incident_degree_sequence<NetT>,
    nanobind::arg("net"));
  if constexpr (is_undirected_v<NetT>)
    m.def(
      "edge_degree_sequence", reticula::edge_degree_sequence<NetT>,
      nanobind::arg("net"));
}
} // namespace

void define_edge_degree(nanobind::module_& m) {
  define_edge_degree_for_net<undirected_network>(m);
  define_edge_degree_for_net<directed_network>(m);
  define_edge_degree_for_net<undirected_temporal_network>(m);
  define_edge_degree_for_net<directed_temporal_network>(m);
  define_edge_degree_for_net<directed_delayed_temporal_network>(m);

  define_edge_degree_for_net<undirected_hypernetwork>(m);
  define_edge_degree_for_net<directed_hypernetwork>(m);
  define_edge_degree_for_net<undirected_temporal_hypernetwork>(m);
  define_edge_degree_for_net<directed_temporal_hypernetwork>(m);
  define_edge_degree_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
