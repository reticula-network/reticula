#include <catch2/catch_test_macros.hpp>

#include <reticula/enumeration.hpp>

namespace {
auto next_comb_inplace(std::vector<std::uint64_t>& v, std::uint64_t n) -> bool {
  const std::size_t d = v.size();
  if (d == 0)
    return false;
  const std::uint64_t n_minus_d = n - static_cast<std::uint64_t>(d);
  for (std::size_t j = d; j-- > 0;) {
    const std::uint64_t max_j = n_minus_d + static_cast<std::uint64_t>(j);
    if (v[j] < max_j) {
      ++v[j];
      for (std::size_t k = j + 1; k < d; ++k)
        v[k] = v[k - 1] + 1;
      return true;
    }
  }
  return false;
}

auto advance_ref(std::vector<std::uint64_t> v, std::uint64_t n, std::uint64_t k)
  -> std::vector<std::uint64_t> {
  while (k--) {
    if (!next_comb_inplace(v, n)) {
      // Match your sentinel convention.
      if (!v.empty())
        v.back() = n;
      break;
    }
  }
  return v;
}

auto binom(std::uint64_t n, std::uint64_t d) -> std::uint64_t {
  if (d > n)
    return 0;
  if (d == 0 || d == n)
    return 1;
  d = std::min(d, n - d);
  __uint128_t res = 1;
  for (std::uint64_t i = 1; i <= d; ++i) {
    res = (res * (n - d + i)) / i;
  }
  return static_cast<std::uint64_t>(res);
}

void require_eq(
  const std::vector<std::uint64_t>& a, const std::vector<std::uint64_t>& b) {
  REQUIRE(a.size() == b.size());
  for (std::size_t i = 0; i < a.size(); ++i)
    REQUIRE(a[i] == b[i]);
}
} // namespace

TEST_CASE("is_past_end semantics", "[reticula::enumeration::is_past_end]") {
  using reticula::enumeration::is_past_end;

  SECTION("empty vector -> false") {
    std::vector<std::uint64_t> v{};
    REQUIRE_FALSE(is_past_end(v, 10));
  }
  SECTION("last < n -> false") {
    std::vector<std::uint64_t> v{0, 2, 5};
    REQUIRE_FALSE(is_past_end(v, 6));
  }
  SECTION("last == n -> true") {
    std::vector<std::uint64_t> v{1, 4, 6};
    REQUIRE(is_past_end(v, 6));
  }
  SECTION("last > n -> true") {
    std::vector<std::uint64_t> v{1, 4, 7};
    REQUIRE(is_past_end(v, 6));
  }
}

TEST_CASE(
  "advance_by_skip: invalid and boundary conditions",
  "[reticula::enumeration::advance_by_skip]") {
  using reticula::enumeration::advance_by_skip;
  using reticula::enumeration::is_past_end;

  SECTION("empty input throws") {
    std::vector<std::uint64_t> v{};
    REQUIRE_THROWS_AS(advance_by_skip(v, 5, 1), std::invalid_argument);
  }

  SECTION("d > n returns sentinel") {
    std::vector<std::uint64_t> v{0, 1, 2, 3};
    auto out = advance_by_skip(v, 3, 1);
    REQUIRE(out.size() == v.size());
    REQUIRE(out.back() == 3);
    REQUIRE(is_past_end(out, 3));
  }

  SECTION("skip==0 returns input unchanged (if not past-end)") {
    std::vector<std::uint64_t> v{0, 2, 5};
    auto out = advance_by_skip(v, 7, 0);
    require_eq(out, v);
  }

  SECTION("starting at max tuple + any positive skip => sentinel") {
    const std::uint64_t n = 10;
    std::vector<std::uint64_t> v{7, 8, 9};
    auto out = advance_by_skip(v, n, 42);
    REQUIRE(out.back() == n);
    REQUIRE(is_past_end(out, n));
  }
}

TEST_CASE("advance_by_skip: known small sequence (n=5,d=3)") {
  using reticula::enumeration::advance_by_skip;
  using reticula::enumeration::is_past_end;

  const std::uint64_t n = 5;
  std::vector<std::uint64_t> v{0, 1, 2};

  std::vector<std::vector<std::uint64_t>> expected{
    {0, 1, 2}, {0, 1, 3}, {0, 1, 4}, {0, 2, 3}, {0, 2, 4},
    {0, 3, 4}, {1, 2, 3}, {1, 2, 4}, {1, 3, 4}, {2, 3, 4}};

  for (const auto& e : expected) {
    require_eq(v, e);
    v = advance_by_skip(v, n, 1);
  }
  REQUIRE(is_past_end(v, n));
}

TEST_CASE(
  "advance_by_skip: batch skip matches reference stepper",
  "[reticula::enumeration::advance_by_skip]") {
  using reticula::enumeration::advance_by_skip;

  const std::uint64_t n = 10;
  std::vector<std::uint64_t> start{1, 3, 6, 7};

  SECTION("pure batching without carry") {
    auto out = advance_by_skip(start, n, 2);
    std::vector<std::uint64_t> ref{1, 3, 6, 9};
    require_eq(out, ref);
  }

  SECTION("carry triggered mid-batch") {
    auto out = advance_by_skip({1, 3, 6, 8}, n, 2);
    std::vector<std::uint64_t> ref = advance_ref({1, 3, 6, 8}, n, 2);
    require_eq(out, ref);
  }

  SECTION("large skip equals repeated single steps") {
    for (std::uint64_t k : {1ull, 2ull, 3ull, 5ull, 20ull, 1000ull}) {
      auto out = advance_by_skip(start, n, k);
      auto ref = advance_ref(start, n, k);
      require_eq(out, ref);
    }
  }
}

TEST_CASE(
  "advance_by_skip: enumerates all combinations then hits sentinel",
  "[reticula::enumeration::advance_by_skip]") {
  using reticula::enumeration::advance_by_skip;
  using reticula::enumeration::is_past_end;

  const std::uint64_t n = 12;
  const std::uint64_t d = 5;
  std::vector<std::uint64_t> v(d);
  for (std::size_t i = 0; i < d; ++i)
    v[i] = static_cast<std::uint64_t>(i);

  const std::uint64_t total = binom(n, d);
  std::uint64_t seen = 0;

  while (!is_past_end(v, n)) {
    for (std::size_t j = 1; j < v.size(); ++j)
      REQUIRE(v[j - 1] < v[j]);
    REQUIRE(v.back() < n);

    ++seen;
    v = advance_by_skip(v, n, 1);
  }

  REQUIRE(seen == total);
  REQUIRE(v.back() == n);
}
