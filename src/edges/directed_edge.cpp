#include <reticula/concepts.hpp>
#include <reticula/edges/directed_edge.hpp>
#include <reticula/utils.hpp>

namespace reticula {
directed_edge::directed_edge(VertexType tail, VertexType head)
    : verts_({tail, head}) {}

auto directed_edge::is_out_incident(VertexType vert) const -> bool {
  return (verts_[0] == vert);
}

auto directed_edge::is_in_incident(VertexType vert) const -> bool {
  return (verts_[1] == vert);
}

auto directed_edge::is_incident(VertexType vert) const -> bool {
  return (is_out_incident(vert) || is_in_incident(vert));
}

auto directed_edge::mutator_verts() const -> std::span<const VertexType, 1> {
  return std::span<const VertexType, 1>(&verts_[0], 1);
}

auto directed_edge::mutated_verts() const -> std::span<const VertexType, 1> {
  return std::span<const VertexType, 1>(&verts_[1], 1);
}

auto directed_edge::incident_verts() const -> std::span<const VertexType> {
  std::size_t n = (verts_[0] == verts_[1]) ? 1 : 2;
  return {verts_.data(), n};
}

auto directed_edge::tail() const -> VertexType { return verts_[0]; }

auto directed_edge::head() const -> VertexType { return verts_[1]; }

auto effect_cmp(const directed_edge& a, const directed_edge& b) -> bool {
  return std::make_tuple(a.verts_[1], a.verts_[0]) <
         std::make_tuple(b.verts_[1], b.verts_[0]);
}

auto adjacent(const directed_edge& a, const directed_edge& b) -> bool {
  return (a.verts_[1] == b.verts_[0]);
}
} // namespace reticula
