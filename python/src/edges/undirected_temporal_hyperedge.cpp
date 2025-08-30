#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>

#include <reticula/edges/undirected_temporal_hyperedge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_undirected_temporal_hyperedge(nanobind::module_& m) {
  nanobind::class_<undirected_temporal_hyperedge>(
    m, "undirected_temporal_hyperedge")
    .def(nanobind::init<std::vector<VertexType>, double>())
    .def(
      "__init__",
      [](
        undirected_temporal_hyperedge* e,
        std::tuple<std::vector<VertexType>, double> t) {
        new (e) undirected_temporal_hyperedge(std::get<0>(t), std::get<1>(t));
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const undirected_temporal_hyperedge& e) {
        return "undirected_temporal_hyperedge(" +
               reticula::utils::join(e.incident_verts(), ", ") + ", " +
               std::to_string(e.cause_time()) + ")";
      })
    .def("cause_time", &undirected_temporal_hyperedge::cause_time)
    .def("effect_time", &undirected_temporal_hyperedge::effect_time)
    .def(edge_properties());

  nanobind::implicitly_convertible<
    std::tuple<std::vector<VertexType>, double>,
    undirected_temporal_hyperedge>();
}
} // namespace reticula::python
