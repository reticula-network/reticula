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
      nanobind::init<
        std::vector<VertexType>, std::size_t, std::size_t>(),
      nanobind::arg("verts"), nanobind::arg("size_hint") = 0,
      nanobind::arg("seed") = 0)
    .def(
      "insert", [](component& c, VertexType v) { c.insert(v); },
      nanobind::arg("v"))
    .def(
      "insert",
      [](component& c, const std::vector<VertexType>& verts) {
        c.insert(verts);
      },
      nanobind::arg("verts"))
    .def("merge", &component::merge, nanobind::arg("c"))
    .def("contains", &component::contains, nanobind::arg("v"))
    .def("empty", &component::empty)
    .def("size", &component::size)
    .def(
      "__iter__",
      [](const component& c) {
        return nanobind::make_iterator(
          nanobind::type<component>(), "iterator", c.begin(), c.end());
      },
      nanobind::keep_alive<0, 1>())
    .def("__len__", &component::size);
}
} // namespace reticula::python
