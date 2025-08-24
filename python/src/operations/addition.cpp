#include <nanobind/nanobind.h>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/operations/addition.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_addition_for_net(nanobind::module_& m) {
  m.def(
    "with_vertices", reticula::with_vertices<NetT, std::vector<VertexType>>,
    nanobind::arg("g"), nanobind::arg("vertices"));

  m.def(
    "with_edges",
    reticula::with_edges<NetT, std::vector<typename NetT::EdgeType>>,
    nanobind::arg("g"), nanobind::arg("edges"));
}
} // namespace

void define_addition(nanobind::module_& m) {
  define_addition_for_net<undirected_network>(m);
  define_addition_for_net<directed_network>(m);
  define_addition_for_net<undirected_temporal_network>(m);
  define_addition_for_net<directed_temporal_network>(m);
  define_addition_for_net<directed_delayed_temporal_network>(m);

  define_addition_for_net<undirected_hypernetwork>(m);
  define_addition_for_net<directed_hypernetwork>(m);
  define_addition_for_net<undirected_temporal_hypernetwork>(m);
  define_addition_for_net<directed_temporal_hypernetwork>(m);
  define_addition_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
