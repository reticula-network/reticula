#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/unordered_map.h>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/operations/relabel.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_relabel_for_net(nanobind::module_& m) {
  m.def(
    "relabel", reticula::relabel<NetT>, nanobind::arg("g"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "relabel", reticula::relabel<NetT, std::function<VertexType(VertexType)>>,
    nanobind::arg("g"), nanobind::arg("mapping"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "relabel",
    reticula::relabel<NetT, std::unordered_map<VertexType, VertexType>>,
    nanobind::arg("g"), nanobind::arg("mapping"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace

void define_relabel(nanobind::module_& m) {
  define_relabel_for_net<undirected_network>(m);
  define_relabel_for_net<directed_network>(m);
  define_relabel_for_net<undirected_temporal_network>(m);
  define_relabel_for_net<directed_temporal_network>(m);
  define_relabel_for_net<directed_delayed_temporal_network>(m);

  define_relabel_for_net<undirected_hypernetwork>(m);
  define_relabel_for_net<directed_hypernetwork>(m);
  define_relabel_for_net<undirected_temporal_hypernetwork>(m);
  define_relabel_for_net<directed_temporal_hypernetwork>(m);
  define_relabel_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
