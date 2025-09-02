#include <nanobind/nanobind.h>

namespace reticula::python {
// generation
void define_deterministic(nanobind::module_& m);
void define_gnp(nanobind::module_& m);
void define_gnm(nanobind::module_& m);
void define_barabasi_albert(nanobind::module_& m);
void define_watts_strogatz(nanobind::module_& m);
void define_regular(nanobind::module_& m);
void define_expected_degree_sequence(nanobind::module_& m);
void define_degree_sequence(nanobind::module_& m);
void define_uniform_hypergraph(nanobind::module_& m);
void define_fully_mixed(nanobind::module_& m);
void define_link_activation(nanobind::module_& m);
void define_node_activation(nanobind::module_& m);
} // namespace reticula::python

NB_MODULE(_reticula_generation, m_) {
  (void)m_;
  nanobind::module_ m = nanobind::module_::import_("reticula");

  reticula::python::define_deterministic(m);
  reticula::python::define_gnp(m);
  reticula::python::define_gnm(m);
  reticula::python::define_barabasi_albert(m);
  reticula::python::define_watts_strogatz(m);
  reticula::python::define_regular(m);
  reticula::python::define_expected_degree_sequence(m);
  reticula::python::define_degree_sequence(m);
  reticula::python::define_uniform_hypergraph(m);
  reticula::python::define_fully_mixed(m);
  reticula::python::define_link_activation(m);
  reticula::python::define_node_activation(m);
}
