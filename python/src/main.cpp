#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>

#include "version.hpp"

namespace reticula::python {
void define_random_state(nanobind::module_& m);

// edges
void define_undirected_edge(nanobind::module_& m);
void define_directed_edge(nanobind::module_& m);
void define_undirected_temporal_edge(nanobind::module_& m);
void define_directed_temporal_edge(nanobind::module_& m);
void define_directed_delayed_temporal_edge(nanobind::module_& m);

// networks
void define_networks(nanobind::module_& m);

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

// operations
void define_algebraic(nanobind::module_& m);
void define_addition(nanobind::module_& m);
void define_removal(nanobind::module_& m);
void define_occupation(nanobind::module_& m);
void define_projections(nanobind::module_& m);
void define_subgraph(nanobind::module_& m);
void define_relabel(nanobind::module_& m);
} // namespace reticula::python

NB_MODULE(_reticula_ext, m) {
  m.def("__version__", []() { return reticula::python::full_version; });
  reticula::python::define_random_state(m);

  reticula::python::define_undirected_edge(m);
  reticula::python::define_directed_edge(m);
  reticula::python::define_undirected_temporal_edge(m);
  reticula::python::define_directed_temporal_edge(m);
  reticula::python::define_directed_delayed_temporal_edge(m);

  reticula::python::define_networks(m);

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

  // reticula::python::define_algebraic(m);
  reticula::python::define_addition(m);
  reticula::python::define_removal(m);
  reticula::python::define_occupation(m);
  reticula::python::define_projections(m);
  reticula::python::define_subgraph(m);
  reticula::python::define_relabel(m);
}
