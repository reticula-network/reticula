#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>

#include <reticula/edges/undirected_edge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_undirected_edge(nanobind::module_& m) {
  nanobind::class_<undirected_edge>(m, "undirected_edge")
    .def(nanobind::init<VertexType, VertexType>())
    .def(
      "__init__",
      [](undirected_edge* e, std::tuple<VertexType, VertexType> t) {
        new (e) undirected_edge(std::get<0>(t), std::get<1>(t));
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const undirected_edge& e) {
        return "undirected_edge(" + std::to_string(e.incident_verts().front()) +
               ", " + std::to_string(e.incident_verts().back()) + ")";
      })
    .def(edge_properties());

  nanobind::implicitly_convertible<
    std::tuple<VertexType, VertexType>, undirected_edge>();
}
} // namespace reticula::python
