#include <reticula/concepts.hpp>
#include <reticula/edges/undirected_temporal_hyperedge.hpp>
#include <reticula/utils.hpp>

namespace reticula {
undirected_temporal_hyperedge::undirected_temporal_hyperedge(
  std::initializer_list<VertexType> verts, TimeType time)
    : undirected_temporal_hyperedge(std::vector<VertexType>(verts), time) {}

undirected_temporal_hyperedge::undirected_temporal_hyperedge(
  const StaticProjectionType& hyperedge, TimeType time)
    : undirected_temporal_hyperedge(hyperedge.mutated_verts(), time) {}

auto undirected_temporal_hyperedge::static_projection() const
  -> undirected_temporal_hyperedge::StaticProjectionType {
  return StaticProjectionType(verts_);
}

auto undirected_temporal_hyperedge::effect_time() const -> TimeType {
  return time_;
}

auto undirected_temporal_hyperedge::cause_time() const -> TimeType {
  return time_;
}

auto undirected_temporal_hyperedge::is_incident(VertexType vert) const -> bool {
  return std::ranges::binary_search(verts_, vert);
}

auto undirected_temporal_hyperedge::is_in_incident(VertexType vert) const
  -> bool {
  return is_incident(vert);
}

auto undirected_temporal_hyperedge::is_out_incident(VertexType vert) const
  -> bool {
  return is_incident(vert);
}

auto undirected_temporal_hyperedge::mutator_verts() const
  -> std::span<const VertexType> {
  return verts_;
}

auto undirected_temporal_hyperedge::mutated_verts() const
  -> std::span<const VertexType> {
  return verts_;
}

auto undirected_temporal_hyperedge::incident_verts() const
  -> std::span<const VertexType> {
  return verts_;
}

auto effect_cmp(
  const undirected_temporal_hyperedge& a,
  const undirected_temporal_hyperedge& b) -> bool {
  return std::make_tuple(a.time_, a.verts_) <
         std::make_tuple(b.time_, b.verts_);
}

auto adjacent(
  const undirected_temporal_hyperedge& a,
  const undirected_temporal_hyperedge& b) -> bool {
  return b.time_ > a.time_ && utils::intersects(a.verts_, b.verts_);
}
} // namespace reticula
