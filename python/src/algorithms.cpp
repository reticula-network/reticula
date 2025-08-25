#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>

namespace reticula::python {
void define_random_state(nanobind::module_& m);
// algorithms
void define_vertex_degree(nanobind::module_& m);
void define_edge_degree(nanobind::module_& m);
} // namespace reticula::python

NB_MODULE(_reticula_algorithms, m) {
  reticula::python::define_vertex_degree(m);
  reticula::python::define_edge_degree(m);
}
