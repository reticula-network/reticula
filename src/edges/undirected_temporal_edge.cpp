#include <reticula/concepts.hpp>
#include <reticula/edges/undirected_temporal_edge.hpp>
#include <reticula/utils.hpp>

#include <reticula/edges/undirected_edge.hpp>

namespace reticula {
undirected_temporal_edge::undirected_temporal_edge(
  VertexType v1, VertexType v2, TimeType time)
    : time_{time}, verts_{utils::minmax_array(v1, v2)} {}

undirected_temporal_edge::undirected_temporal_edge(
  const undirected_temporal_edge::StaticProjectionType& edge, TimeType time)
    : undirected_temporal_edge(
        edge.mutated_verts().back(), edge.mutated_verts().front(), time) {}

auto undirected_temporal_edge::effect_time() const -> TimeType { return time_; }

auto undirected_temporal_edge::cause_time() const -> TimeType { return time_; }

auto undirected_temporal_edge::static_projection() const
  -> undirected_temporal_edge::StaticProjectionType {
  return {verts_[0], verts_[1]};
}

auto undirected_temporal_edge::is_incident(VertexType vert) const -> bool {
  return (verts_[0] == vert || verts_[1] == vert);
}

auto undirected_temporal_edge::is_out_incident(VertexType vert) const -> bool {
  return is_incident(vert);
}

auto undirected_temporal_edge::is_in_incident(VertexType vert) const -> bool {
  return is_incident(vert);
}

auto undirected_temporal_edge::incident_verts() const
  -> std::span<const VertexType> {
  std::size_t n = (verts_[0] == verts_[1]) ? 1 : 2;
  return {verts_.data(), n};
}

auto undirected_temporal_edge::mutator_verts() const
  -> std::span<const VertexType> {
  return incident_verts();
}

auto undirected_temporal_edge::mutated_verts() const
  -> std::span<const VertexType> {
  return incident_verts();
}

auto effect_cmp(
  const undirected_temporal_edge& a, const undirected_temporal_edge& b)
  -> bool {
  return a < b;
}

auto adjacent(
  const undirected_temporal_edge& a, const undirected_temporal_edge& b)
  -> bool {
  return (b.time_ > a.time_) &&
         (a.verts_[0] == b.verts_[0] || a.verts_[0] == b.verts_[1] ||
          a.verts_[1] == b.verts_[0] || a.verts_[1] == b.verts_[1]);
}
} // namespace reticula
