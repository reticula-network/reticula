#include <nanobind/nanobind.h>

namespace reticula::python {
// operations
void define_algebraic(nanobind::module_& m);
void define_addition(nanobind::module_& m);
void define_removal(nanobind::module_& m);
void define_occupation(nanobind::module_& m);
void define_projections(nanobind::module_& m);
void define_subgraph(nanobind::module_& m);
void define_relabel(nanobind::module_& m);
} // namespace reticula::python

NB_MODULE(_reticula_operations, m) {
  reticula::python::define_algebraic(m);
  reticula::python::define_addition(m);
  reticula::python::define_removal(m);
  reticula::python::define_occupation(m);
  reticula::python::define_projections(m);
  reticula::python::define_subgraph(m);
  reticula::python::define_relabel(m);
}
