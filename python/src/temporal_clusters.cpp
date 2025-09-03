#include <nanobind/nanobind.h>

#include <reticula/edges.hpp>
#include <reticula/temporal_clusters.hpp>

#include "vector_properties.hpp"

namespace reticula::python {
namespace {
template <reticula::temporal_network_edge EdgeT>
void define_temporal_cluster(nanobind::module_& m, std::string name) {
  nanobind::class_<temporal_cluster<EdgeT>>(m, name.c_str())
    .def(
      nanobind::init<std::size_t>(), nanobind::arg("size_hint") = 0,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "merge", &temporal_cluster<EdgeT>::merge, nanobind::arg("other"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "contains", &temporal_cluster<EdgeT>::contains, nanobind::arg("edge"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "covers", &temporal_cluster<EdgeT>::covers, nanobind::arg("vertex"),
      nanobind::arg("time"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "empty", &temporal_cluster<EdgeT>::empty,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "begin", &temporal_cluster<EdgeT>::begin,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "end", &temporal_cluster<EdgeT>::end,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "intervals", &temporal_cluster<EdgeT>::intervals, nanobind::arg("vertex"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "lifetime", &temporal_cluster<EdgeT>::lifetime,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "size", &temporal_cluster<EdgeT>::size,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "volume", &temporal_cluster<EdgeT>::volume,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  define_vector<temporal_cluster<EdgeT>>(m, name);
  define_vector<std::pair<EdgeT, temporal_cluster<EdgeT>>>(
    m, std::string("pair_") + name.substr(8) + "_" + name);

  define_vector<std::pair<EdgeT, temporal_cluster_size>>(
    m, std::string("pair_") + name.substr(8) + "_" + "temporal_cluster_size");
}
} // namespace

void define_temporal_clusters(nanobind::module_& m) {
  define_temporal_cluster<undirected_temporal_edge>(
    m, "temporal_cluster_undirected_temporal_edge");
  define_temporal_cluster<directed_temporal_edge>(
    m, "temporal_cluster_directed_temporal_edge");
  define_temporal_cluster<directed_delayed_temporal_edge>(
    m, "temporal_cluster_directed_delayed_temporal_edge");
  define_temporal_cluster<undirected_temporal_hyperedge>(
    m, "temporal_cluster_undirected_temporal_hyperedge");
  define_temporal_cluster<directed_temporal_hyperedge>(
    m, "temporal_cluster_directed_temporal_hyperedge");
  define_temporal_cluster<directed_delayed_temporal_hyperedge>(
    m, "temporal_cluster_directed_delayed_temporal_hyperedge");

  nanobind::class_<temporal_cluster_size>(m, "temporal_cluster_size")
    .def(
      nanobind::init<const temporal_cluster<undirected_temporal_edge>&>(),
      nanobind::arg("cluster"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      nanobind::init<const temporal_cluster<directed_temporal_edge>&>(),
      nanobind::arg("cluster"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      nanobind::init<const temporal_cluster<directed_delayed_temporal_edge>&>(),
      nanobind::arg("cluster"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      nanobind::init<const temporal_cluster<undirected_temporal_hyperedge>&>(),
      nanobind::arg("cluster"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      nanobind::init<const temporal_cluster<directed_temporal_hyperedge>&>(),
      nanobind::arg("cluster"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      nanobind::init<
        const temporal_cluster<directed_delayed_temporal_hyperedge>&>(),
      nanobind::arg("cluster"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "lifetime", &temporal_cluster_size::lifetime,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "mass", &temporal_cluster_size::mass,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "volume", &temporal_cluster_size::volume,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "size", &temporal_cluster_size::size,
      nanobind::call_guard<nanobind::gil_scoped_release>());

  define_vector<temporal_cluster_size>(m, "temporal_cluster_size");
}
} // namespace reticula::python
