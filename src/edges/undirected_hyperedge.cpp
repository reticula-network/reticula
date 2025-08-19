#include <reticula/concepts.hpp>
#include <reticula/edges/undirected_hyperedge.hpp>
#include <reticula/utils.hpp>

namespace reticula {
undirected_hyperedge::undirected_hyperedge(
  std::initializer_list<VertexType> verts)
    : undirected_hyperedge(std::views::all(verts)) {}

auto undirected_hyperedge::is_out_incident(VertexType vert) const -> bool {
  return is_incident(vert);
}

auto undirected_hyperedge::is_in_incident(VertexType vert) const -> bool {
  return is_incident(vert);
}

auto undirected_hyperedge::is_incident(VertexType vert) const -> bool {
  return std::ranges::binary_search(verts_, vert);
}

auto undirected_hyperedge::mutator_verts() const
  -> std::span<const VertexType> {
  return verts_;
}

auto undirected_hyperedge::mutated_verts() const
  -> std::span<const VertexType> {
  return verts_;
}

auto undirected_hyperedge::incident_verts() const
  -> std::span<const VertexType> {
  return verts_;
}

auto effect_cmp(const undirected_hyperedge& a, const undirected_hyperedge& b)
  -> bool {
  return a.verts_ < b.verts_;
}

auto adjacent(const undirected_hyperedge& a, const undirected_hyperedge& b)
  -> bool {
  return utils::intersects(a.verts_, b.verts_);
}
} // namespace reticula
