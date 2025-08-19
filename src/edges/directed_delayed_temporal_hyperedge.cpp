#include <reticula/concepts.hpp>
#include <reticula/edges/directed_delayed_temporal_hyperedge.hpp>
#include <reticula/utils.hpp>

namespace reticula {
using std::span;

directed_delayed_temporal_hyperedge::directed_delayed_temporal_hyperedge(
  std::initializer_list<VertexType> tails,
  std::initializer_list<VertexType> heads, TimeType cause_time,
  TimeType effect_time)
    : directed_delayed_temporal_hyperedge(
        std::views::all(tails), std::views::all(heads), cause_time,
        effect_time) {}

directed_delayed_temporal_hyperedge::directed_delayed_temporal_hyperedge(
  const StaticProjectionType& hyperedge, TimeType cause_time,
  TimeType effect_time)
    : directed_delayed_temporal_hyperedge(
        hyperedge.tails(), hyperedge.heads(), cause_time, effect_time) {}

auto directed_delayed_temporal_hyperedge::static_projection() const
  -> directed_delayed_temporal_hyperedge::StaticProjectionType {
  return {tails_, heads_};
}

auto directed_delayed_temporal_hyperedge::effect_time() const -> TimeType {
  return effect_time_;
}

auto directed_delayed_temporal_hyperedge::cause_time() const -> TimeType {
  return cause_time_;
}

auto directed_delayed_temporal_hyperedge::is_out_incident(VertexType vert) const
  -> bool {
  return std::ranges::binary_search(tails_, vert);
}

auto directed_delayed_temporal_hyperedge::is_in_incident(VertexType vert) const
  -> bool {
  return std::ranges::binary_search(heads_, vert);
}

auto directed_delayed_temporal_hyperedge::is_incident(VertexType vert) const
  -> bool {
  return (is_out_incident(vert) || is_in_incident(vert));
}

auto directed_delayed_temporal_hyperedge::mutator_verts() const
  -> span<const VertexType> {
  return tails_;
}

auto directed_delayed_temporal_hyperedge::mutated_verts() const
  -> std::span<const VertexType> {
  return heads_;
}

auto directed_delayed_temporal_hyperedge::tails() const
  -> std::span<const VertexType> {
  return tails_;
}

auto directed_delayed_temporal_hyperedge::heads() const
  -> std::span<const VertexType> {
  return heads_;
}

auto directed_delayed_temporal_hyperedge::incident_verts() const
  -> std::vector<VertexType> {
  std::vector<VertexType> res;
  res.reserve(heads_.size() + tails_.size());
  std::ranges::set_union(tails_, heads_, std::back_inserter(res));
  return res;
}

auto effect_cmp(
  const directed_delayed_temporal_hyperedge& a,
  const directed_delayed_temporal_hyperedge& b) -> bool {
  return std::make_tuple(a.effect_time_, a.cause_time_, a.heads_, a.tails_) <
         std::make_tuple(b.effect_time_, b.cause_time_, b.heads_, b.tails_);
}

auto adjacent(
  const directed_delayed_temporal_hyperedge& a,
  const directed_delayed_temporal_hyperedge& b) -> bool {
  return b.cause_time_ > a.effect_time_ &&
         utils::intersects(a.heads_, b.tails_);
}
} // namespace reticula
