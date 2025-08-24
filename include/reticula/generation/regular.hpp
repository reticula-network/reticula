#include <optional>
#include <random>

#include <reticula/concepts.hpp>
#include <reticula/networks.hpp>
#include <reticula/views/enumerate.hpp>

namespace reticula {
template <std::uniform_random_bit_generator Gen>
[[nodiscard]]
auto try_random_regular_graph(
  VertexType size, VertexType degree, Gen& gen, std::size_t max_tries)
  -> std::optional<undirected_network>;

template <std::uniform_random_bit_generator Gen>
[[nodiscard]]
auto random_regular_graph(VertexType size, VertexType degree, Gen& gen)
  -> undirected_network;

template <std::uniform_random_bit_generator Gen>
[[nodiscard]]
auto try_random_directed_regular_graph(
  VertexType size, VertexType degree, Gen& gen, std::size_t max_tries)
  -> std::optional<directed_network>;

template <std::uniform_random_bit_generator Gen>
[[nodiscard]]
auto random_directed_regular_graph(VertexType size, VertexType degree, Gen& gen)
  -> directed_network;
} // namespace reticula

namespace reticula {
template <std::uniform_random_bit_generator Gen>
auto try_random_regular_graph(
  VertexType size, VertexType degree, Gen& gen, std::size_t max_tries)
  -> std::optional<undirected_network> {

  if (size == 0)
    throw std::domain_error("size must be non-zero");

  if (degree >= size)
    throw std::domain_error("degree must be less than size");

  const std::size_t stub_count = size * degree;
  if (stub_count % 2 != 0)
    throw std::domain_error("size or degree must be even");

  for (std::size_t t = 0; t < max_tries; t++) {
    std::unordered_set<undirected_edge> edges;
    edges.reserve(stub_count / 2);

    std::vector<VertexType> stubs;
    stubs.reserve(stub_count);
    for (VertexType i = 0; i < size; i++)
      for (VertexType d = 0; d < degree; d++)
        stubs.emplace_back(i);

    while (true) {
      std::ranges::shuffle(stubs, gen);

      std::vector<std::size_t> unpaired_degree(size, 0);

      for (auto iter = stubs.begin(); iter < stubs.end();
           std::advance(iter, 2)) {
        if (
          *iter != *(iter + 1) &&
          edges.find({*iter, *(iter + 1)}) == edges.end()) {
          edges.emplace(*iter, *(iter + 1));
        } else {
          unpaired_degree[*iter]++;
          unpaired_degree[*(iter + 1)]++;
        }
      }

      stubs.clear();
      for (const auto [node, count] : unpaired_degree | views::enumerate)
        for (std::size_t i = 0; i < count; i++)
          stubs.emplace_back(node);

      bool has_available_pair = false;
      std::vector<VertexType> keys;
      keys.reserve(unpaired_degree.size());
      for (const auto [node, cnt] : unpaired_degree | views::enumerate)
        if (cnt)
          keys.push_back(static_cast<VertexType>(node));

      for (std::size_t i = 0; !has_available_pair && i < keys.size(); ++i)
        for (std::size_t j = i + 1; j < keys.size(); ++j)
          if ((has_available_pair = !edges.contains({keys[i], keys[j]})))
            break;

      if (!has_available_pair)
        break;
    }

    if (edges.size() == stub_count / 2)
      return undirected_network(edges, std::views::iota(VertexType{}, size));
  }

  return std::nullopt;
}

template <std::uniform_random_bit_generator Gen>
auto random_regular_graph(VertexType size, VertexType degree, Gen& gen)
  -> undirected_network {
  std::optional<undirected_network> maybe_g = std::nullopt;
  while (!maybe_g)
    maybe_g = try_random_regular_graph(size, degree, gen, 1000);
  return *maybe_g;
}

template <std::uniform_random_bit_generator Gen>
auto try_random_directed_regular_graph(
  VertexType size, VertexType degree, Gen& gen, std::size_t max_tries)
  -> std::optional<directed_network> {
  if (size == 0)
    throw std::domain_error("size must be non-zero");

  if (degree >= size)
    throw std::domain_error("degree must be less than size");

  const std::size_t stub_count = size * degree;

  for (std::size_t t = 0; t < max_tries; ++t) {
    std::unordered_set<directed_edge> edges;
    edges.reserve(stub_count);

    std::vector<VertexType> out_stubs;
    out_stubs.reserve(stub_count);
    std::vector<VertexType> in_stubs;
    in_stubs.reserve(stub_count);
    for (VertexType v = 0; v < size; ++v) {
      for (VertexType d = 0; d < degree; ++d) {
        out_stubs.emplace_back(v);
        in_stubs.emplace_back(v);
      }
    }

    while (true) {
      std::ranges::shuffle(out_stubs, gen);
      std::ranges::shuffle(in_stubs, gen);

      std::vector<std::size_t> out_unpaired(size, 0);
      std::vector<std::size_t> in_unpaired(size, 0);

      for (std::size_t i = 0; i < out_stubs.size(); ++i) {
        const VertexType u = out_stubs[i];
        const VertexType v = in_stubs[i];

        if (u != v && !edges.contains({u, v})) {
          edges.emplace(u, v);
        } else {
          ++out_unpaired[u];
          ++in_unpaired[v];
        }
      }

      if (edges.size() == stub_count)
        break;

      out_stubs.clear();
      in_stubs.clear();
      for (const auto [u, c] : out_unpaired | views::enumerate)
        for (std::size_t k = 0; k < c; ++k)
          out_stubs.emplace_back(u);
      for (const auto [v, c] : in_unpaired | views::enumerate)
        for (std::size_t k = 0; k < c; ++k)
          in_stubs.emplace_back(v);

      bool has_available_pair = false;
      std::vector<VertexType> out_keys;
      out_keys.reserve(out_unpaired.size());
      std::vector<VertexType> in_keys;
      in_keys.reserve(in_unpaired.size());
      for (const auto [u, c] : out_unpaired | views::enumerate)
        if (c)
          out_keys.push_back(static_cast<reticula::VertexType>(u));
      for (const auto& [v, c] : in_unpaired | views::enumerate)
        if (c)
          in_keys.push_back(static_cast<reticula::VertexType>(v));

      for (std::size_t i = 0; !has_available_pair && i < out_keys.size(); ++i)
        for (auto in_key : in_keys)
          if ((has_available_pair =
                 (out_keys[i] != in_key &&
                  !edges.contains({out_keys[i], in_key}))))
            break;

      if (!has_available_pair)
        break;
    }

    if (edges.size() == stub_count)
      return directed_network(edges, std::views::iota(VertexType{}, size));
  }

  return std::nullopt;
}

template <std::uniform_random_bit_generator Gen>
auto random_directed_regular_graph(VertexType size, VertexType degree, Gen& gen)
  -> directed_network {
  std::optional<directed_network> maybe_g = std::nullopt;
  while (!maybe_g)
    maybe_g = try_random_directed_regular_graph(size, degree, gen, 1000);
  return *maybe_g;
}
} // namespace reticula
