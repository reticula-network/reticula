#include "reticula/concepts.hpp"
#include <nanobind/make_iterator.h>
#include <nanobind/nanobind.h>

#include <reticula/components.hpp>

namespace reticula::python {
void define_component(nanobind::module_& m) {
  nanobind::class_<component>(m, "component")
    .def(
      nanobind::init<std::size_t, std::size_t>(),
      nanobind::arg("size_hint") = 0, nanobind::arg("seed") = 0)
    .def(
      nanobind::init<std::vector<VertexType>, std::size_t, std::size_t>(),
      nanobind::arg("verts"), nanobind::arg("size_hint") = 0,
      nanobind::arg("seed") = 0,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "insert", [](component& c, VertexType v) { c.insert(v); },
      nanobind::arg("v"), nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "insert",
      [](component& c, const std::vector<VertexType>& verts) {
        c.insert(verts);
      },
      nanobind::arg("verts"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "merge", &component::merge, nanobind::arg("c"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "contains", &component::contains, nanobind::arg("v"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "empty", &component::empty,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "size", &component::size,
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__iter__",
      [](const component& c) {
        return nanobind::make_iterator(
          nanobind::type<component>(), "iterator", c.begin(), c.end());
      },
      nanobind::keep_alive<0, 1>())
    .def(
      "__len__", &component::size,
      nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace reticula::python
