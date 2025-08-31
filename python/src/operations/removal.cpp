#include <nanobind/nanobind.h>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/operations/removal.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_removal_for_net(nanobind::module_& m) {
  m.def(
    "without_isolated_vertices", reticula::without_isolated_vertices<NetT>,
    nanobind::arg("g"), nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "without_vertices",
    reticula::without_vertices<NetT, std::vector<VertexType>>,
    nanobind::arg("g"), nanobind::arg("edges"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "without_edges",
    reticula::without_edges<NetT, std::vector<typename NetT::EdgeType>>,
    nanobind::arg("g"), nanobind::arg("edges"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace

void define_removal(nanobind::module_& m) {
  define_removal_for_net<undirected_network>(m);
  define_removal_for_net<directed_network>(m);
  define_removal_for_net<undirected_temporal_network>(m);
  define_removal_for_net<directed_temporal_network>(m);
  define_removal_for_net<directed_delayed_temporal_network>(m);

  define_removal_for_net<undirected_hypernetwork>(m);
  define_removal_for_net<directed_hypernetwork>(m);
  define_removal_for_net<undirected_temporal_hypernetwork>(m);
  define_removal_for_net<directed_temporal_hypernetwork>(m);
  define_removal_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
