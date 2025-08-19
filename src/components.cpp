#include <reticula/components.hpp>
#include <reticula/concepts.hpp>

namespace reticula {
component::component(std::size_t size_hint, std::size_t /* seed */) : verts_() {
  if (size_hint > 0)
    verts_.reserve(size_hint);
}

component::component(
  std::initializer_list<VertexType> verts, std::size_t size_hint,
  std::size_t /* seed */)
    : component(std::views::all(verts), size_hint) {}

void component::insert(VertexType v) { verts_.insert(v); }

void component::merge(const component& c) { verts_.insert(c.begin(), c.end()); }

auto component::contains(VertexType v) const -> bool {
  return verts_.contains(v);
}

auto component::empty() const -> bool { return verts_.empty(); }

auto component::size() const -> std::size_t { return verts_.size(); }

auto component::begin() const -> typename component::IteratorType {
  return verts_.begin();
}

auto component::end() const -> typename component::IteratorType {
  return verts_.end();
}

component_sketch::component_sketch(
  std::size_t /* size_hint */, std::size_t seed)
    : verts_{true, seed} {}

component_sketch::component_sketch(
  std::initializer_list<VertexType> verts, std::size_t size_hint,
  std::size_t seed)
    : component_sketch(std::views::all(verts), seed, size_hint) {}

void component_sketch::insert(VertexType v) { verts_.insert(v); }

void component_sketch::merge(const component_sketch& other) {
  verts_.merge(other.verts_);
}

auto component_sketch::size_estimate() const -> double { return verts_.estimate(); }
} // namespace reticula
