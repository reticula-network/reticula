#include <nanobind/nanobind.h>

#include <random>

namespace reticula::python {
void define_random_state(nanobind::module_& m) {
  nanobind::class_<std::mt19937_64>(m, "mersenne_twister")
    .def(
      "__init__",
      [](std::mt19937_64* t) {
        new (t) std::mt19937_64{std::random_device{}()};
      },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      nanobind::init<std::mt19937_64::result_type>(), nanobind::arg("seed"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__copy__",
      [](const std::mt19937_64& self) { return std::mt19937_64(self); },
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__deepcopy__",
      [](const std::mt19937_64& self, nanobind::dict) {
        return std::mt19937_64(self);
      },
      nanobind::arg("memo"),
      nanobind::call_guard<nanobind::gil_scoped_release>())
    .def(
      "__call__", [](std::mt19937_64& self) { return self(); },
      nanobind::call_guard<nanobind::gil_scoped_release>());
}
} // namespace reticula::python
