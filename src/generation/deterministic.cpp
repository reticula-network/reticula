#include "reticula/networks.hpp"
#include <functional>
#include <numeric>
#include <ranges>
#include <vector>

#include <reticula/concepts.hpp>
#include <reticula/generation/deterministic.hpp>
#include <reticula/views/cartesian_product.hpp>
#include <reticula/views/chain.hpp>

auto reticula::path_graph(VertexType size, bool periodic)
  -> undirected_network {
  if (size == 0)
    return {};
  if (size == 1)
    return reticula::undirected_network{
      std::views::empty<undirected_edge>, std::views::iota(VertexType{}, size)};

  auto edges =
    std::views::iota(VertexType{}, size) |
    std::views::transform([&size](VertexType i) {
      return reticula::undirected_edge{i, (i + 1) % size};
    }) |
    std::views::filter([&](const auto& edge) {
      return periodic || edge != reticula::undirected_edge{size - 1, 0};
    });
  return reticula::undirected_network{
    edges, std::views::iota(VertexType{}, size)};
}

auto reticula::cycle_graph(VertexType size) -> undirected_network {
  return reticula::path_graph(size, true);
}

auto reticula::star_graph(VertexType size) -> undirected_network {
  if (size == 0)
    return {};
  if (size == 1)
    return reticula::undirected_network{
      std::views::empty<undirected_edge>, std::views::iota(VertexType{}, size)};

  auto edges = std::views::iota(VertexType{1}, size) |
               std::views::transform(
                 [](VertexType i) { return reticula::undirected_edge{0, i}; });
  return reticula::undirected_network{
    edges, std::views::iota(VertexType{}, size)};
}

auto reticula::wheel_graph(VertexType size) -> undirected_network {
  if (size == 0)
    return {};
  if (size == 1)
    return reticula::undirected_network{
      std::views::empty<undirected_edge>, std::views::iota(VertexType{}, size)};
  auto circle =
    std::views::iota(VertexType{0}, size - 1) |
    std::views::transform([&](VertexType i) {
      return reticula::undirected_edge{i + 1, (i + 1) % (size - 1) + 1};
    });
  auto star = std::views::iota(VertexType{1}, size) |
              std::views::transform(
                [](VertexType i) { return reticula::undirected_edge{0, i}; });

  return reticula::undirected_network{
    views::chain(circle, star), std::views::iota(VertexType{}, size)};
}

auto reticula::complete_bipartite_graph(VertexType size_a, VertexType size_b)
  -> undirected_network {
  if (size_a == 0 || size_b == 0)
    return reticula::undirected_network{
      std::views::empty<undirected_edge>,
      std::views::iota(VertexType{}, size_a + size_b)};

  auto edges = views::cartesian_product(
                 std::views::iota(VertexType{}, size_a),
                 std::views::iota(VertexType{size_a}, size_a + size_b)) |
               std::views::transform([](const auto& pair) {
                 auto [i, j] = pair;
                 return reticula::undirected_edge{i, j};
               });
  return reticula::undirected_network{
    edges, std::views::iota(VertexType{}, size_a + size_b)};
}

auto reticula::complete_graph(VertexType size) -> undirected_network {
  if (size == 0)
    return {};

  auto edges = std::views::cartesian_product(
                 std::views::iota(VertexType{}, size),
                 std::views::iota(VertexType{}, size)) |
               std::views::filter([](const auto& pair) {
                 auto [i, j] = pair;
                 return i < j;
               }) |
               std::views::transform([](const auto& pair) {
                 auto [i, j] = pair;
                 return reticula::undirected_edge{i, j};
               });
  return reticula::undirected_network{
    edges, std::views::iota(VertexType{}, size)};
}

auto reticula::complete_bipartite_directed_graph(
  VertexType size_a, VertexType size_b) -> directed_network {
  if (size_a == 0 || size_b == 0)
    return reticula::directed_network{
      std::views::empty<directed_edge>,
      std::views::iota(VertexType{}, size_a + size_b)};
  auto edges = views::cartesian_product(
                 std::views::iota(VertexType{}, size_a),
                 std::views::iota(VertexType{size_a}, size_a + size_b)) |
               std::views::transform([](const auto& pair) {
                 auto [i, j] = pair;
                 return reticula::directed_edge{i, j};
               });
  return reticula::directed_network{
    edges, std::views::iota(VertexType{}, size_a + size_b)};
}

auto reticula::complete_directed_graph(VertexType size) -> directed_network {
  if (size == 0)
    return {};

  auto edges = std::views::cartesian_product(
                 std::views::iota(VertexType{}, size),
                 std::views::iota(VertexType{}, size)) |
               std::views::filter([](const auto& pair) {
                 auto [i, j] = pair;
                 return i != j;
               }) |
               std::views::transform([](const auto& pair) {
                 auto [i, j] = pair;
                 return reticula::directed_edge{i, j};
               });
  return reticula::directed_network{
    edges, std::views::iota(VertexType{}, size)};
}

auto reticula::regular_ring_lattice(VertexType size, VertexType degree)
  -> undirected_network {
  if (size == 0 && degree == 0)
    return {};

  if (degree % 2 != 0)
    throw std::invalid_argument("degree should be divisible by two");

  if (degree >= size)
    throw std::invalid_argument("degree should be smaller than size");

  if (degree == 0)
    return reticula::undirected_network{
      std::views::empty<undirected_edge>, std::views::iota(VertexType{}, size)};

  auto edges = std::views::iota(VertexType{}, size) |
               std::views::transform([=](VertexType i) {
                 return std::views::iota(i + 1, i + degree / 2 + 1) |
                        std::views::transform([=](VertexType j) {
                          return reticula::undirected_edge{i, j % size};
                        });
               }) |
               std::views::join;

  return reticula::undirected_network{
    edges, std::views::iota(VertexType{}, size)};
}

auto reticula::square_grid_graph(
  std::vector<VertexType> dimensions, bool periodic) -> undirected_network {
  if (dimensions.empty() || std::ranges::any_of(dimensions, [](auto d) {
        return d == 0;
      })) {
    return {};
  }

  const auto size = std::accumulate(
    dimensions.begin(), dimensions.end(), VertexType{1},
    std::multiplies<VertexType>{});

  const std::size_t d = dimensions.size();
  std::vector<VertexType> stride(d, 1);
  for (std::size_t k = d - 1; k > 0; --k)
    stride[k - 1] = stride[k] * dimensions[k];

  auto outward_edges = [=, &stride](VertexType vid) {
    return std::views::iota(std::size_t{0}, d) |
           std::views::filter([&](std::size_t k) {
             const auto dimk = dimensions[k];
             const auto ck = (vid / stride[k]) % dimk;
             return periodic ? (dimk > 1) : (ck + 1 < dimk);
           }) |
           std::views::transform([=, &stride](std::size_t k) {
             const auto dimk = dimensions[k];
             const auto ck = (vid / stride[k]) % dimk;
             const auto nb = (ck + 1 < dimk) ? (vid + stride[k])
                                             : (vid - (dimk - 1) * stride[k]);
             return undirected_edge(vid, nb);
           });
  };

  auto edges = std::views::iota(VertexType{0}, size) |
               std::views::transform(outward_edges) | std::views::join |
               std::views::common;

  return reticula::undirected_network{
    edges, std::views::iota(VertexType{}, size)};
}
