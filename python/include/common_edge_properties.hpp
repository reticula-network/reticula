#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>

namespace reticula::python {
struct edge_properties : nanobind::def_visitor<edge_properties> {
  template <typename Class, typename... Extra>
  void execute(Class& cl, const Extra&...) {
    using EdgeT = Class::Type;
    cl.def(nanobind::init<>())
      .def(
        nanobind::self < nanobind::self,
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        nanobind::self <= nanobind::self,
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        nanobind::self == nanobind::self,
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        nanobind::self != nanobind::self,
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        nanobind::self >= nanobind::self,
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        nanobind::self > nanobind::self,
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        nanobind::hash(nanobind::self),
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        "__copy__", [](const EdgeT& e) { return EdgeT(e); },
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        "__deepcopy__", [](const EdgeT& e, nanobind::dict) { return EdgeT(e); },
        nanobind::arg("memo"),
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        "is_incident", &EdgeT::is_incident,
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        "is_in_incident", &EdgeT::is_in_incident,
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        "is_out_incident", &EdgeT::is_out_incident,
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        "mutator_verts", &EdgeT::mutator_verts, nanobind::keep_alive<0, 1>(),
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        "mutated_verts", &EdgeT::mutated_verts, nanobind::keep_alive<0, 1>(),
        nanobind::call_guard<nanobind::gil_scoped_release>())
      .def(
        "incident_verts", &EdgeT::incident_verts, nanobind::keep_alive<0, 1>(),
        nanobind::call_guard<nanobind::gil_scoped_release>());
  }
};
} // namespace reticula::python
