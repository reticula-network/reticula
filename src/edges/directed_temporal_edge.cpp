#include <reticula/concepts.hpp>
#include <reticula/edges/directed_temporal_edge.hpp>
#include <reticula/utils.hpp>

#include <reticula/edges/directed_edge.hpp>

namespace reticula {
directed_temporal_edge::directed_temporal_edge(
  VertexType tail, VertexType head, TimeType time)
    : time_{time}, verts_{tail, head} {}

directed_temporal_edge::directed_temporal_edge(
  const directed_temporal_edge::StaticProjectionType& edge, TimeType time)
    : directed_temporal_edge(edge.tail(), edge.head(), time) {}

auto directed_temporal_edge::effect_time() const -> TimeType { return time_; }

auto directed_temporal_edge::cause_time() const -> TimeType { return time_; }

auto directed_temporal_edge::static_projection() const
  -> directed_temporal_edge::StaticProjectionType {
  return {verts_[0], verts_[1]};
}

auto directed_temporal_edge::is_incident(VertexType vert) const -> bool {
  return (is_out_incident(vert) || is_in_incident(vert));
}

auto directed_temporal_edge::is_out_incident(VertexType vert) const -> bool {
  return (verts_[0] == vert);
}

auto directed_temporal_edge::is_in_incident(VertexType vert) const -> bool {
  return (verts_[1] == vert);
}

auto directed_temporal_edge::incident_verts() const
  -> std::span<const VertexType> {
  std::size_t n = (verts_[0] == verts_[1]) ? 1 : 2;
  return {verts_.data(), n};
}

auto directed_temporal_edge::mutator_verts() const
  -> std::span<const VertexType, 1> {
  return std::span<const VertexType, 1>(&verts_[0], 1);
}

auto directed_temporal_edge::mutated_verts() const
  -> std::span<const VertexType, 1> {
  return std::span<const VertexType, 1>(&verts_[1], 1);
}

auto effect_cmp(
  const directed_temporal_edge& a, const directed_temporal_edge& b) -> bool {
  return std::make_tuple(a.time_, a.verts_[1], a.verts_[0]) <
         std::make_tuple(b.time_, b.verts_[1], b.verts_[0]);
}

auto adjacent(const directed_temporal_edge& a, const directed_temporal_edge& b)
  -> bool {
  return (b.time_ > a.time_) && (a.verts_[1] == b.verts_[0]);
}
} // namespace reticula
