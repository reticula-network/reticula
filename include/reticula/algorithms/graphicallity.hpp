#pragma once

#include <ranges>

#include <reticula/concepts.hpp>

namespace reticula {
template <std::ranges::forward_range Range>
  requires degree_range<Range>
auto is_graphic(Range&& degree_sequence) -> bool;

template <std::ranges::forward_range PairRange>
  requires degree_pair_range<PairRange>
auto is_digraphic(PairRange&& in_out_degree_sequence) -> bool;
} // namespace reticula

#include <algorithm>
#include <limits>
#include <vector>

namespace reticula {
template <std::ranges::forward_range Range>
  requires degree_range<Range>
auto is_graphic(Range&& degree_sequence) -> bool {
  std::size_t num_degrees = 0;
  std::size_t max_deg = std::numeric_limits<std::size_t>::min();
  std::size_t min_deg = std::numeric_limits<std::size_t>::max();
  std::size_t total_degrees = 0;

  for (auto deg : degree_sequence) {
    if (deg < 0)
      return false;
    total_degrees += static_cast<std::size_t>(deg);
    min_deg = std::min(min_deg, static_cast<std::size_t>(deg));
    max_deg = std::max(max_deg, static_cast<std::size_t>(deg));
    num_degrees++;
  }

  // an empty sequence is graphic
  if (num_degrees == 0)
    return true;

  if (
    max_deg >= num_degrees || total_degrees % 2 != 0 ||
    total_degrees > num_degrees * (num_degrees - 1))
    return false;

  if (
    4 * num_degrees * min_deg <
    (max_deg + min_deg + 1) * (max_deg + min_deg + 1))
    return true;

  std::vector<size_t> degree_dist(max_deg + 1, 0);
  for (auto deg : degree_sequence)
    degree_dist[static_cast<std::size_t>(deg)]++;

  std::size_t k = 0, partial_deg_sum = 0;
  std::size_t partial_n_j_sum = 0, partial_jn_j_sum = 0;
  for (std::size_t d_k = max_deg; d_k >= min_deg; d_k--) {
    if (d_k < k + 1)
      return true;

    if (degree_dist[d_k] > 0) {
      std::size_t run = degree_dist[d_k];
      if (d_k < k + run)
        run = d_k - k;

      partial_deg_sum += run * d_k;
      for (std::size_t i = 0; i < run; i++) {
        partial_n_j_sum += degree_dist[k + i];
        partial_jn_j_sum += (k + i) * degree_dist[k + i];
      }

      k += run;
      if (
        partial_deg_sum >
        k * (num_degrees - 1) - k * partial_n_j_sum + partial_jn_j_sum)
        return false;
    }
  }

  return true;
}

template <std::ranges::forward_range PairRange>
  requires degree_pair_range<PairRange>
auto is_digraphic(PairRange&& in_out_degree_sequence) -> bool {
  using InType = std::tuple_element_t<0, std::ranges::range_value_t<PairRange>>;
  using OutType =
    std::tuple_element_t<1, std::ranges::range_value_t<PairRange>>;

  InType sum_in{}, max_in{};
  OutType sum_out{};

  std::vector<std::pair<OutType, InType>> zero_heap, nonzero_heap;
  for (auto [in, out] : in_out_degree_sequence) {
    if (in < 0 || out < 0)
      return false;

    max_in = std::max(in, max_in);
    sum_in += in;
    sum_out += out;

    if (in == 0)
      zero_heap.emplace_back(out, in);
    else
      nonzero_heap.emplace_back(out, in);
  }

  if (sum_in != sum_out)
    return false;

  std::ranges::make_heap(zero_heap);
  std::ranges::make_heap(nonzero_heap);

  std::vector<std::pair<OutType, InType>> modified_stubs;
  modified_stubs.reserve(static_cast<std::size_t>(max_in));

  while (!nonzero_heap.empty()) {
    std::ranges::pop_heap(nonzero_heap);
    auto [out, in] = nonzero_heap.back();
    nonzero_heap.pop_back();

    if (static_cast<std::size_t>(in) > nonzero_heap.size() + zero_heap.size())
      return false;

    for (InType i{}; i < in; i++) {
      OutType stub_out;
      InType stub_in;
      if (
        !zero_heap.empty() &&
        (nonzero_heap.empty() || nonzero_heap.front() < zero_heap.front())) {
        std::ranges::pop_heap(zero_heap);
        std::tie(stub_out, stub_in) = zero_heap.back();
        zero_heap.pop_back();
      } else {
        std::ranges::pop_heap(nonzero_heap);
        std::tie(stub_out, stub_in) = nonzero_heap.back();
        nonzero_heap.pop_back();
      }

      if (stub_out == 0)
        return false;
      else if (stub_out > 1 || stub_in > 0)
        modified_stubs.emplace_back(stub_out - 1, stub_in);
    }

    while (!modified_stubs.empty()) {
      auto [mout, min] = modified_stubs.back();
      if (min > 0) {
        nonzero_heap.emplace_back(mout, min);
        std::ranges::push_heap(nonzero_heap);
      } else {
        zero_heap.emplace_back(mout, min);
        std::ranges::push_heap(zero_heap);
      }

      modified_stubs.pop_back();
    }

    if (out > 0) {
      zero_heap.emplace_back(out, 0);
      std::ranges::push_heap(zero_heap);
    }
  }

  return true;
}
} // namespace reticula
