#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>

namespace reticula::python {
void define_random_state(nanobind::module_& m);
// algorithms
void define_vertex_degree(nanobind::module_& m);
void define_edge_degree(nanobind::module_& m);
void define_properties(nanobind::module_& m);
void define_static_reachability(nanobind::module_& m);
void define_graphicallity(nanobind::module_& m);
void define_dag(nanobind::module_& m);
void define_bipartite(nanobind::module_& m);
void define_assortativity(nanobind::module_& m);
} // namespace reticula::python

NB_MODULE(_reticula_algorithms, m) {
  reticula::python::define_vertex_degree(m);
  reticula::python::define_edge_degree(m);
  reticula::python::define_properties(m);
  reticula::python::define_static_reachability(m);
  reticula::python::define_graphicallity(m);
  reticula::python::define_dag(m);
  reticula::python::define_bipartite(m);
  reticula::python::define_assortativity(m);
}
