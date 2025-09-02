#include <nanobind/nanobind.h>
#include <nanobind/stl/variant.h>

#include <reticula/algorithms/temporal_reachability.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/temporal_adjacency.hpp>

namespace reticula::python {
namespace {
template <network_like NetT>
void define_temporal_reachability_for_net(nanobind::module_& m) {
  using adjacency_variant = std::variant<
    adjacency::simple, adjacency::limited_waiting_time, adjacency::exponential,
    adjacency::geometric>;

  m.def(
    "out_cluster",
    [](
      const NetT& net, VertexType v, double time,
      adjacency_variant adj_variant) {
      return std::visit(
        [&](auto&& adj) {
          return reticula::out_cluster<NetT>(net, v, time, adj);
        },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("v"), nanobind::arg("time"),
    nanobind::arg("adjacency"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "out_cluster",
    [](
      const NetT& net, typename NetT::EdgeType e,
      adjacency_variant adj_variant) {
      return std::visit(
        [&](auto&& adj) { return reticula::out_cluster<NetT>(net, e, adj); },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("e"), nanobind::arg("adjacency"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "in_cluster",
    [](
      const NetT& net, VertexType v, double time,
      adjacency_variant adj_variant) {
      return std::visit(
        [&](auto&& adj) {
          return reticula::in_cluster<NetT>(net, v, time, adj);
        },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("v"), nanobind::arg("time"),
    nanobind::arg("adjacency"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "in_cluster",
    [](
      const NetT& net, typename NetT::EdgeType e,
      adjacency_variant adj_variant) {
      return std::visit(
        [&](auto&& adj) { return reticula::in_cluster<NetT>(net, e, adj); },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("e"), nanobind::arg("adjacency"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "weak_cluster",
    [](
      const NetT& net, VertexType v, double time,
      adjacency_variant adj_variant) {
      return std::visit(
        [&](auto&& adj) {
          return reticula::weak_cluster<NetT>(net, v, time, adj);
        },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("v"), nanobind::arg("time"),
    nanobind::arg("adjacency"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "weak_cluster",
    [](
      const NetT& net, typename NetT::EdgeType e,
      adjacency_variant adj_variant) {
      return std::visit(
        [&](auto&& adj) { return reticula::weak_cluster<NetT>(net, e, adj); },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("e"), nanobind::arg("adjacency"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "out_clusters",
    [](const NetT& net, adjacency_variant adj_variant) {
      return std::visit(
        [&](auto&& adj) { return reticula::out_clusters<NetT>(net, adj); },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("adjacency"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "in_clusters",
    [](const NetT& net, adjacency_variant adj_variant) {
      return std::visit(
        [&](auto&& adj) { return reticula::in_clusters<NetT>(net, adj); },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("adjacency"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "weak_clusters",
    [](const NetT& net, adjacency_variant adj_variant, bool singletons) {
      return std::visit(
        [&](auto&& adj) {
          return reticula::weak_clusters<NetT>(net, adj, singletons);
        },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("adjacency"),
    nanobind::arg("singletons") = true,
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "out_cluster_sizes",
    [](const NetT& net, adjacency_variant adj_variant) {
      return std::visit(
        [&](auto&& adj) { return reticula::out_cluster_sizes<NetT>(net, adj);
        }, adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("adjacency"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "in_cluster_sizes",
    [](const NetT& net, adjacency_variant adj_variant) {
      return std::visit(
        [&](auto&& adj) { return reticula::in_cluster_sizes<NetT>(net, adj);
        }, adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("adjacency"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "weak_cluster_sizes",
    [](const NetT& net, adjacency_variant adj_variant, bool singletons) {
      return std::visit(
        [&](auto&& adj) {
          return reticula::weak_cluster_sizes<NetT>(net, adj, singletons);
        },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("adjacency"),
    nanobind::arg("singletons") = true,
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "out_cluster_size_estimates",
    [](
      const NetT& net, adjacency_variant adj_variant,
      double temporal_resolution, std::size_t seed) {
      return std::visit(
        [&](auto&& adj) {
          return reticula::out_cluster_size_estimates<NetT>(
            net, adj, temporal_resolution, seed);
        },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("adjacency"),
    nanobind::arg("temporal_resolution"), nanobind::arg("seed"),
    nanobind::call_guard<nanobind::gil_scoped_release>());

  m.def(
    "in_cluster_size_estimates",
    [](
      const NetT& net, adjacency_variant adj_variant,
      double temporal_resolution, std::size_t seed) {
      return std::visit(
        [&](auto&& adj) {
          return reticula::in_cluster_size_estimates<NetT>(
            net, adj, temporal_resolution, seed);
        },
        adj_variant);
    },
    nanobind::arg("net"), nanobind::arg("adjacency"),
    nanobind::arg("temporal_resolution"), nanobind::arg("seed"),
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace

void define_temporal_reachability(nanobind::module_& m) {
  define_temporal_reachability_for_net<undirected_temporal_network>(m);
  define_temporal_reachability_for_net<directed_temporal_network>(m);
  define_temporal_reachability_for_net<directed_delayed_temporal_network>(m);
  define_temporal_reachability_for_net<undirected_temporal_hypernetwork>(m);
  define_temporal_reachability_for_net<directed_temporal_hypernetwork>(m);
  define_temporal_reachability_for_net<directed_delayed_temporal_hypernetwork>(
    m);
}
} // namespace reticula::python
