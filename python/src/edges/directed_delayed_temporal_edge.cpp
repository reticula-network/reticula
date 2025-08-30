#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>

#include <reticula/edges/directed_delayed_temporal_edge.hpp>

#include "common_edge_properties.hpp"

namespace reticula::python {
void define_directed_delayed_temporal_edge(nanobind::module_& m) {
  nanobind::class_<directed_delayed_temporal_edge>(
    m, "directed_delayed_temporal_edge")
    .def(nanobind::init<VertexType, VertexType, double, double>())
    .def(
      "__init__",
      [](
        directed_delayed_temporal_edge* e,
        std::tuple<VertexType, VertexType, double, double> t) {
        new (e) directed_delayed_temporal_edge(
          std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t));
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__repr__",
      [](const directed_delayed_temporal_edge& e) {
        return "directed_delayed_temporal_edge(" + std::to_string(e.tail()) +
               ", " + std::to_string(e.head()) + ", " +
               std::to_string(e.cause_time()) + ", " +
               std::to_string(e.effect_time()) + ")";
      })
    .def("tail", &directed_delayed_temporal_edge::tail)
    .def("head", &directed_delayed_temporal_edge::head)
    .def("cause_time", &directed_delayed_temporal_edge::cause_time)
    .def("effect_time", &directed_delayed_temporal_edge::effect_time)
    .def(edge_properties());

  nanobind::implicitly_convertible<
    std::tuple<VertexType, VertexType, double, double>,
    directed_delayed_temporal_edge>();
}
} // namespace reticula::python
