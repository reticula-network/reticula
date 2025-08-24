#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>

#include "spans.hpp"

namespace reticula::python {
struct edge_properties : nanobind::def_visitor<edge_properties> {
  template <typename Class, typename... Extra>
  void execute(Class& cl, const Extra&...) {
    using EdgeT = Class::Type;
    cl.def(nanobind::init<>())
      .def(nanobind::self < nanobind::self)
      .def(nanobind::self <= nanobind::self)
      .def(nanobind::self == nanobind::self)
      .def(nanobind::self != nanobind::self)
      .def(nanobind::self >= nanobind::self)
      .def(nanobind::self > nanobind::self)
      .def(nanobind::hash(nanobind::self))
      .def("__copy__", [](const EdgeT& e) { return EdgeT(e); })
      .def(
        "__deepcopy__", [](const EdgeT& e, nanobind::dict) { return EdgeT(e); },
        nanobind::arg("memo"))
      .def("is_incident", &EdgeT::is_incident)
      .def("is_in_incident", &EdgeT::is_in_incident)
      .def("is_out_incident", &EdgeT::is_out_incident)
      .def(
        "mutator_verts", utils::ndarray_output<&EdgeT::mutator_verts>(),
        nanobind::rv_policy::reference_internal)
      .def(
        "mutated_verts", utils::ndarray_output<&EdgeT::mutated_verts>(),
        nanobind::rv_policy::reference_internal)
      .def(
        "incident_verts", utils::ndarray_output<&EdgeT::incident_verts>(),
        nanobind::rv_policy::reference_internal);
  }
};
} // namespace reticula::python
