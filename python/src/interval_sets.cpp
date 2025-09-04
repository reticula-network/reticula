#include <nanobind/make_iterator.h>
#include <nanobind/nanobind.h>
#include <nanobind/operators.h>

#include <reticula/interval_sets.hpp>

namespace reticula::python {
void define_interval_set(nanobind::module_& m) {
  nanobind::class_<interval_set>(m, "interval_set")
    .def(nanobind::init<>())
    .def(
      "insert", &interval_set::insert, nanobind::arg("start"),
      nanobind::arg("end"))
    .def("merge", &interval_set::merge, nanobind::arg("other"))
    .def("covers", &interval_set::covers, nanobind::arg("time"))
    .def("cover", &interval_set::cover)
    .def("__contains__", &interval_set::covers, nanobind::arg("time"))
    .def(
      "__iter__",
      [](const interval_set& self) {
        return nanobind::make_iterator(
          nanobind::type<interval_set>(), "iterator", self.begin(), self.end());
      },
      nanobind::keep_alive<0, 1>())
    .def(nanobind::self == nanobind::self);
}
} // namespace reticula::python
