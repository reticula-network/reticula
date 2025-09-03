#include <reticula/edges.hpp>

#include <nanobind/nanobind.h>

#include "vector_properties.hpp"

namespace reticula::python {
void define_vector_types(nanobind::module_& m) {
  define_vector<std::uint64_t>(m, "uint64");
  define_vector<std::int64_t>(m, "int64");
  define_vector<std::uint32_t>(m, "uint32");
  define_vector<std::int32_t>(m, "int32");
  define_vector<double>(m, "double");
  define_vector<float>(m, "float");

  define_vector<std::pair<double, double>>(m, "pair_double_double");
  define_vector<std::pair<VertexType, double>>(m, "pair_uint64_double");
  define_vector<std::pair<std::uint64_t, std::uint64_t>>(
    m, "pair_uint64_uint64");

  define_vector<undirected_edge>(m, "undirected_edge");
  define_vector<directed_edge>(m, "directed_edge");
  define_vector<undirected_temporal_edge>(m, "undirected_temporal_edge");
  define_vector<directed_temporal_edge>(m, "directed_temporal_edge");
  define_vector<directed_delayed_temporal_edge>(
    m, "directed_delayed_temporal_edge");

  define_vector<undirected_hyperedge>(m, "undirected_hyperedge");
  define_vector<directed_hyperedge>(m, "directed_hyperedge");
  define_vector<undirected_temporal_hyperedge>(
    m, "undirected_temporal_hyperedge");
  define_vector<directed_temporal_hyperedge>(m, "directed_temporal_hyperedge");
  define_vector<directed_delayed_temporal_hyperedge>(
    m, "directed_delayed_temporal_hyperedge");
}
} // namespace reticula::python
