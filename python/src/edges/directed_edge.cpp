#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>

#include <reticula/edges/directed_edge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_directed_edge(nanobind::module_& m) {
  nanobind::class_<directed_edge>(m, "directed_edge")
    .def(nanobind::init<VertexType, VertexType>(),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__init__",
      [](directed_edge* e, std::tuple<VertexType, VertexType> t) {
        new (e) directed_edge(std::get<0>(t), std::get<1>(t));
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const directed_edge& e) {
        return "directed_edge(" + std::to_string(e.tail()) + ", " +
               std::to_string(e.head()) + ")";
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def("tail", &directed_edge::tail,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def("head", &directed_edge::head,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(edge_properties());

  nanobind::implicitly_convertible<
    std::tuple<VertexType, VertexType>, directed_edge>();
}
} // namespace reticula::python
