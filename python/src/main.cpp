#include <numeric>
#include <vector>

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

void define_undirected_hyperedge(nanobind::module_& m);
void define_directed_hyperedge(nanobind::module_& m);
void define_undirected_temporal_hyperedge(nanobind::module_& m);
void define_directed_temporal_hyperedge(nanobind::module_& m);
void define_directed_delayed_temporal_hyperedge(nanobind::module_& m);

// networks
void define_networks(nanobind::module_& m);

// vectors
void define_vector_types(nanobind::module_& m);
// spans
void define_span_types(nanobind::module_& m);

// components
void define_component(nanobind::module_& m);

// processes
void define_processes(nanobind::module_& m);

// event graphs
void define_event_graph(nanobind::module_& m);
} // namespace reticula::python

NB_MODULE(_reticula_ext, m_) {
  (void)m_;
  nanobind::module_ m = nanobind::module_::import_("reticula");

  reticula::python::define_random_state(m);

  reticula::python::define_undirected_edge(m);
  reticula::python::define_directed_edge(m);
  reticula::python::define_undirected_temporal_edge(m);
  reticula::python::define_directed_temporal_edge(m);
  reticula::python::define_directed_delayed_temporal_edge(m);

  reticula::python::define_undirected_hyperedge(m);
  reticula::python::define_directed_hyperedge(m);
  reticula::python::define_undirected_temporal_hyperedge(m);
  reticula::python::define_directed_temporal_hyperedge(m);
  reticula::python::define_directed_delayed_temporal_hyperedge(m);

  reticula::python::define_networks(m);

  reticula::python::define_vector_types(m);

  reticula::python::define_span_types(m);

  reticula::python::define_component(m);

  reticula::python::define_processes(m);

  reticula::python::define_event_graph(m);

  m.def(
    "__version__", []() { return reticula::python::full_version; },
    nanobind::call_guard<nanobind::gil_scoped_release>());

  auto m_helpers = m.def_submodule("test_helpers");
  m_helpers.def(
    "test_vector", []() { return std::vector<std::uint64_t>{1, 2, 3, 4, 5}; },
    nanobind::call_guard<nanobind::gil_scoped_release>());
  m_helpers.def(
    "vector_sum",
    [](const std::vector<std::uint64_t>& v) {
      return std::accumulate(v.begin(), v.end(), 0);
    },
    nanobind::call_guard<nanobind::gil_scoped_release>());
}
