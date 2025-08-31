#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>

#include <reticula/edges/directed_temporal_edge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_directed_temporal_edge(nanobind::module_& m) {
  nanobind::class_<directed_temporal_edge>(m, "directed_temporal_edge")
    .def(nanobind::init<VertexType, VertexType, double>(),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__init__",
      [](
        directed_temporal_edge* e,
        std::tuple<VertexType, VertexType, double> t) {
        new (e) directed_temporal_edge(
          std::get<0>(t), std::get<1>(t), std::get<2>(t));
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const directed_temporal_edge& e) {
        return "directed_temporal_edge(" + std::to_string(e.tail()) + ", " +
               std::to_string(e.head()) + ", " +
               std::to_string(e.cause_time()) + ")";
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def("tail", &directed_temporal_edge::tail,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def("head", &directed_temporal_edge::head,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def("cause_time", &directed_temporal_edge::cause_time,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def("effect_time", &directed_temporal_edge::effect_time,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(edge_properties());

  nanobind::implicitly_convertible<
    std::tuple<VertexType, VertexType, double>, directed_temporal_edge>();
}
} // namespace reticula::python
