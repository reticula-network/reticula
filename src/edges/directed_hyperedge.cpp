#include <reticula/concepts.hpp>
#include <reticula/edges/directed_hyperedge.hpp>
#include <reticula/utils.hpp>

namespace reticula {
directed_hyperedge::directed_hyperedge(
  std::initializer_list<VertexType> tails,
  std::initializer_list<VertexType> heads)
    : directed_hyperedge(std::views::all(tails), std::views::all(heads)) {}

auto directed_hyperedge::is_out_incident(VertexType vert) const -> bool {
  return std::ranges::binary_search(tails_, vert);
}

auto directed_hyperedge::is_in_incident(VertexType vert) const -> bool {
  return std::ranges::binary_search(heads_, vert);
}

auto directed_hyperedge::is_incident(VertexType vert) const -> bool {
  return (is_out_incident(vert) || is_in_incident(vert));
}

auto directed_hyperedge::mutator_verts() const -> std::span<const VertexType> {
  return tails_;
}

auto directed_hyperedge::mutated_verts() const -> std::span<const VertexType> {
  return heads_;
}

auto directed_hyperedge::tails() const -> std::span<const VertexType> {
  return tails_;
}

auto directed_hyperedge::heads() const -> std::span<const VertexType> {
  return heads_;
}

auto directed_hyperedge::incident_verts() const -> std::vector<VertexType> {
  std::vector<VertexType> res;
  res.reserve(heads_.size() + tails_.size());
  std::ranges::set_union(tails_, heads_, std::back_inserter(res));
  return res;
}

auto effect_cmp(const directed_hyperedge& a, const directed_hyperedge& b)
  -> bool {
  return std::make_tuple(a.heads_, a.tails_) <
         std::make_tuple(b.heads_, b.tails_);
}

auto adjacent(const directed_hyperedge& a, const directed_hyperedge& b)
  -> bool {
  return utils::intersects(a.heads_, b.tails_);
}
} // namespace reticula
