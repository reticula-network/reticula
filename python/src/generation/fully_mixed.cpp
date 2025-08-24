#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>

#include <reticula/generation/fully_mixed.hpp>

namespace reticula::python {
void define_fully_mixed(nanobind::module_& m) {
  m.def(
    "random_fully_mixed_temporal_network",
    &reticula::random_fully_mixed_temporal_network<std::mt19937_64>,
    nanobind::arg("n"), nanobind::arg("rate"), nanobind::arg("max_t"),
    nanobind::arg("random_state"));

  m.def(
    "random_directed_fully_mixed_temporal_network",
    &reticula::random_directed_fully_mixed_temporal_network<std::mt19937_64>,
    nanobind::arg("n"), nanobind::arg("rate"), nanobind::arg("max_t"),
    nanobind::arg("random_state"));
}
} // namespace reticula::python
