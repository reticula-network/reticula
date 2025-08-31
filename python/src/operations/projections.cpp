#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/unordered_map.h>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/operations/projections.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_projections_for_net(nanobind::module_& m) {
  if constexpr (temporal_network_edge<typename NetT::EdgeType>) {
    m.def(
      "static_projection", reticula::static_projection<NetT>,
      nanobind::arg("net"),
      nanobind::call_guard<nanobind::gil_scoped_release>());
  } else {
    m.def(
      "directed_projection", reticula::directed_projection<NetT>,
      nanobind::arg("net"),
      nanobind::call_guard<nanobind::gil_scoped_release>());
    m.def(
      "clique_expansion", reticula::clique_expansion<NetT>,
      nanobind::arg("net"),
      nanobind::call_guard<nanobind::gil_scoped_release>());
  }
}
} // namespace

void define_projections(nanobind::module_& m) {
  define_projections_for_net<undirected_network>(m);
  define_projections_for_net<directed_network>(m);
  define_projections_for_net<undirected_temporal_network>(m);
  define_projections_for_net<directed_temporal_network>(m);
  define_projections_for_net<directed_delayed_temporal_network>(m);

  define_projections_for_net<undirected_hypernetwork>(m);
  define_projections_for_net<directed_hypernetwork>(m);
  define_projections_for_net<undirected_temporal_hypernetwork>(m);
  define_projections_for_net<directed_temporal_hypernetwork>(m);
  define_projections_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
