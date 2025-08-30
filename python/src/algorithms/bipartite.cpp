#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>

#include <reticula/algorithms/bipartite.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {

template <network_like NetT>
void define_bipartite_for_net(nanobind::module_& m) {
  m.def("is_bipartite", reticula::is_bipartite<NetT>, nanobind::arg("net"));
  m.def("two_colouring", reticula::two_colouring<NetT>, nanobind::arg("net"));
  m.def(
    "try_two_colouring", reticula::try_two_colouring<NetT>,
    nanobind::arg("net"));
}
} // namespace

void define_bipartite(nanobind::module_& m) {
  define_bipartite_for_net<undirected_network>(m);
  define_bipartite_for_net<directed_network>(m);
  define_bipartite_for_net<undirected_temporal_network>(m);
  define_bipartite_for_net<directed_temporal_network>(m);
  define_bipartite_for_net<directed_delayed_temporal_network>(m);

  define_bipartite_for_net<undirected_hypernetwork>(m);
  define_bipartite_for_net<directed_hypernetwork>(m);
  define_bipartite_for_net<undirected_temporal_hypernetwork>(m);
  define_bipartite_for_net<directed_temporal_hypernetwork>(m);
  define_bipartite_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
