#include <nanobind/nanobind.h>
#include <nanobind/stl/variant.h>

#include <reticula/concepts.hpp>
#include <reticula/event_graphs.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_event_graph_for_net(nanobind::module_& m) {
  using adjacency_variant = std::variant<
    adjacency::simple, adjacency::limited_waiting_time, adjacency::exponential,
    adjacency::geometric>;

  m.def(
    "successors",
    [](
      const NetT& net, typename NetT::EdgeType e, adjacency_variant adj_variant,
      bool reduce) {
      return std::visit(
        [&](auto&& adj) {
          return reticula::event_graph::successors(net, e, adj, reduce);
        },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("e"), nanobind::arg("adjacency"),
    nanobind::arg("reduce") = false,
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "predecessors",
    [](
      const NetT& net, typename NetT::EdgeType e, adjacency_variant adj_variant,
      bool reduce) {
      return std::visit(
        [&](auto&& adj) {
          return reticula::event_graph::predecessors(net, e, adj, reduce);
        },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("e"), nanobind::arg("adjacency"),
    nanobind::arg("reduce") = false,
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace

void define_event_graph(nanobind::module_& m) {
  nanobind::module_ m2 = m.def_submodule("event_graph");

  define_event_graph_for_net<undirected_temporal_network>(m2);
  define_event_graph_for_net<directed_temporal_network>(m2);
  define_event_graph_for_net<directed_delayed_temporal_network>(m2);
  define_event_graph_for_net<undirected_temporal_hypernetwork>(m2);
  define_event_graph_for_net<directed_temporal_hypernetwork>(m2);
  define_event_graph_for_net<directed_delayed_temporal_hypernetwork>(m2);
}
} // namespace reticula::python
