#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <reticula/edges/directed_edge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_directed_edge(nanobind::module_& m) {
  nanobind::class_<directed_edge>(m, "directed_edge")
    .def(nanobind::init<VertexType, VertexType>())
    .def(
      "__repr__",
      [](const directed_edge& e) {
        return "directed_edge(" + std::to_string(e.tail()) + ", " +
               std::to_string(e.head()) + ")";
      })
    .def("tail", &directed_edge::tail)
    .def("head", &directed_edge::head)
    .def(edge_properties());
}
} // namespace reticula::python
