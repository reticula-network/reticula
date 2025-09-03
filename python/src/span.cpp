#include <reticula/edges.hpp>

#include <nanobind/nanobind.h>

#include "vector_properties.hpp"

namespace reticula::python {
void define_span_types(nanobind::module_& m) {
  define_span<std::uint64_t, 1>(m, "uint64");
  define_span<std::uint64_t>(m, "uint64");
  define_span<std::int64_t>(m, "int64");
  define_span<std::uint32_t>(m, "uint32");
  define_span<std::int32_t>(m, "int32");
  define_span<double>(m, "double");
  define_span<float>(m, "float");

  define_span<std::pair<double, double>>(m, "pair_double_double");
  define_span<std::pair<std::uint64_t, std::uint64_t>>(m, "pair_uint64_uint64");

  define_span<undirected_edge>(m, "undirected_edge");
  define_span<directed_edge>(m, "directed_edge");
  define_span<undirected_temporal_edge>(m, "undirected_temporal_edge");
  define_span<directed_temporal_edge>(m, "directed_temporal_edge");
  define_span<directed_delayed_temporal_edge>(
    m, "directed_delayed_temporal_edge");

  define_span<undirected_hyperedge>(m, "undirected_hyperedge");
  define_span<directed_hyperedge>(m, "directed_hyperedge");
  define_span<undirected_temporal_hyperedge>(
    m, "undirected_temporal_hyperedge");
  define_span<directed_temporal_hyperedge>(m, "directed_temporal_hyperedge");
  define_span<directed_delayed_temporal_hyperedge>(
    m, "directed_delayed_temporal_hyperedge");
}
} // namespace reticula::python
