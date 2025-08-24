#include <nanobind/stl/string.h>

#include <reticula/edges/directed_delayed_temporal_edge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_directed_delayed_temporal_edge(nanobind::module_& m) {
  nanobind::class_<directed_delayed_temporal_edge>(m, "directed_delayed_temporal_edge")
    .def(nanobind::init<VertexType, VertexType, double, double>())
    .def(
      "__repr__",
      [](const directed_delayed_temporal_edge& e) {
        return "directed_delayed_temporal_edge(" + std::to_string(e.tail()) + ", " +
               std::to_string(e.head()) + ", " +
               std::to_string(e.cause_time()) + ", " +
               std::to_string(e.effect_time()) + ")";
      })
    .def("tail", &directed_delayed_temporal_edge::tail)
    .def("head", &directed_delayed_temporal_edge::head)
    .def("cause_time", &directed_delayed_temporal_edge::cause_time)
    .def("effect_time", &directed_delayed_temporal_edge::effect_time)
    .def(edge_properties());
}
} // namespace reticula::python
