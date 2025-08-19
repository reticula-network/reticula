#include <reticula/concepts.hpp>
#include <reticula/edges/undirected_edge.hpp>
#include <reticula/utils.hpp>

namespace reticula {
undirected_edge::undirected_edge(VertexType v1, VertexType v2)
    : verts_{utils::minmax_array(v1, v2)} {}

auto undirected_edge::is_incident(VertexType vert) const -> bool {
  return (verts_[0] == vert || verts_[1] == vert);
}

auto undirected_edge::is_out_incident(VertexType vert) const -> bool {
  return is_incident(vert);
}

auto undirected_edge::is_in_incident(VertexType vert) const -> bool {
  return is_incident(vert);
}

auto undirected_edge::incident_verts() const -> std::span<const VertexType> {
  std::size_t n = (verts_[0] == verts_[1]) ? 1 : 2;
  return {verts_.data(), n};
}

auto undirected_edge::mutator_verts() const -> std::span<const VertexType> {
  return incident_verts();
}

auto undirected_edge::mutated_verts() const -> std::span<const VertexType> {
  return incident_verts();
}

auto effect_cmp(const undirected_edge& a, const undirected_edge& b) -> bool {
  return a < b;
}

auto adjacent(const undirected_edge& a, const undirected_edge& b) -> bool {
  return (
    a.verts_[0] == b.verts_[0] || a.verts_[0] == b.verts_[1] ||
    a.verts_[1] == b.verts_[0] || a.verts_[1] == b.verts_[1]);
}
} // namespace reticula
