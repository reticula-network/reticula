#include <nanobind/nanobind.h>

#include <reticula/algorithms/vertex_degree.hpp>
#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>

namespace reticula::python {
namespace {

template <class Fn>
auto wrap_u64(Fn&& f) {
  if constexpr (std::same_as<std::size_t, std::uint64_t>)
    return f;
  else
    return [f](auto&&... args) {
      auto v = std::invoke(f, std::forward<decltype(args)>(args)...);

      std::vector<std::uint64_t> out;
      out.reserve(v.size());
      for (const auto& x : v)
        out.emplace_back(static_cast<std::uint64_t>(x));
      return out;
    };
}

template <network_like NetT>
void define_vertex_degree_for_net(nanobind::module_& m) {
  m.def(
    "in_degree", &reticula::in_degree<NetT>, nanobind::arg("net"),
    nanobind::arg("v"));
  m.def(
    "out_degree", &reticula::out_degree<NetT>, nanobind::arg("net"),
    nanobind::arg("v"));
  m.def(
    "incident_degree", &reticula::incident_degree<NetT>, nanobind::arg("net"),
    nanobind::arg("v"));

  if constexpr (is_undirected_v<NetT>)
    m.def(
      "degree", &reticula::degree<NetT>, nanobind::arg("net"),
      nanobind::arg("v"));

  m.def(
    "in_degree_sequence", wrap_u64(&reticula::in_degree_sequence<NetT>),
    nanobind::arg("net"));
  m.def(
    "out_degree_sequence", wrap_u64(&reticula::out_degree_sequence<NetT>),
    nanobind::arg("net"));
  m.def(
    "incident_degree_sequence",
    wrap_u64(&reticula::incident_degree_sequence<NetT>), nanobind::arg("net"));
  if constexpr (is_undirected_v<NetT>)
    m.def(
      "degree_sequence", wrap_u64(&reticula::degree_sequence<NetT>),
      nanobind::arg("net"));
  m.def(
    "in_out_degree_pair_sequence",
    wrap_u64(&reticula::in_out_degree_pair_sequence<NetT>),
    nanobind::arg("net"));
}
} // namespace

void define_vertex_degree(nanobind::module_& m) {
  define_vertex_degree_for_net<undirected_network>(m);
  define_vertex_degree_for_net<directed_network>(m);
  define_vertex_degree_for_net<undirected_temporal_network>(m);
  define_vertex_degree_for_net<directed_temporal_network>(m);
  define_vertex_degree_for_net<directed_delayed_temporal_network>(m);

  define_vertex_degree_for_net<undirected_hypernetwork>(m);
  define_vertex_degree_for_net<directed_hypernetwork>(m);
  define_vertex_degree_for_net<undirected_temporal_hypernetwork>(m);
  define_vertex_degree_for_net<directed_temporal_hypernetwork>(m);
  define_vertex_degree_for_net<directed_delayed_temporal_hypernetwork>(m);
}
} // namespace reticula::python
