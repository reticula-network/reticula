#include <catch2/catch.hpp>
using Catch::Matchers::UnorderedEquals;
using Catch::Matchers::Equals;

#include <dag/utils.hpp>
#include <dag/networks.hpp>
#include <dag/generators.hpp>

// there are better ways to do this but this only has to be correct up to 6C3
std::size_t choose(std::size_t n, std::size_t k) {
  if (k > n)
    return 0;
  else if (k == n || k == 0)
    return 1;
  else
    return choose(n - 1, k - 1) + choose(n - 1, k);
}

std::size_t power(std::size_t n, std::size_t d) {
  std::size_t r = 1;
  for (std::size_t i = 0; i < d; i++)
    r *= n;
  return r;
}

TEMPLATE_TEST_CASE_SIG("square grid graph", "[dag::square_grid_graph]",
    ((std::size_t Dims), Dims), 1, 2, 3, 4, 5, 6) {
  std::size_t n = 5;

  SECTION("periodic") {
    dag::undirected_network<std::size_t> periodic =
      dag::square_grid_graph<std::size_t>(n, Dims, true);

    REQUIRE(periodic.vertices().size() == power(n, Dims));
    REQUIRE(periodic.edges().size() == power(n, Dims)*Dims);

    auto verts = periodic.vertices();

    // test degrees
    REQUIRE(std::count_if(verts.begin(), verts.end(),
          [&periodic, dims = Dims, n](std::size_t vert) {
            return periodic.degree(vert) == dims*2;
          }) == static_cast<std::ptrdiff_t>(verts.size()));

    // test local structure
    auto check_locality = [&periodic, dims = Dims](std::size_t vert) {
      std::unordered_set<std::size_t> d2;

      for (auto&& n1: periodic.neighbours(vert))
        for (auto&& n2: periodic.neighbours(n1))
          d2.insert(n2);

      return d2.size() == 2*power(dims, 2) + 1;
    };
    REQUIRE(std::all_of(verts.begin(), verts.end(), check_locality));
  }

  SECTION("aperiodic") {
    dag::undirected_network<std::size_t> aperiodic =
      dag::square_grid_graph<std::size_t>(n, Dims, false);

    REQUIRE(aperiodic.vertices().size() == power(n, Dims));
    REQUIRE(aperiodic.edges().size() == power(n, Dims)*Dims -
        power(n, Dims-1)*choose(Dims, Dims-1));

    auto verts = aperiodic.vertices();

    // test degrees
    for (std::size_t d = 1; d <= Dims; d++)
      REQUIRE(std::count_if(verts.begin(), verts.end(),
            [&aperiodic, d, dims = Dims, n](std::size_t vert) {
              return aperiodic.degree(vert) == dims+d;
            }) == static_cast<std::ptrdiff_t>(
              power(n-2, d)*choose(Dims, d)*power(2, Dims-d)));

    // test local structure
    auto check_centre_locality = [&aperiodic, dims = Dims](std::size_t vert) {
      std::unordered_set<std::size_t> d2;

      if (aperiodic.degree(vert) < dims*2)
        return false;

      for (auto&& n1: aperiodic.neighbours(vert))
        for (auto&& n2: aperiodic.neighbours(n1))
          d2.insert(n2);

      return d2.size() == 2*power(dims, 2) + 1;
    };

    // for now let's check locality not close to the boundaries:
    REQUIRE(
        std::count_if(
          verts.begin(), verts.end(),
          check_centre_locality) ==
        static_cast<std::ptrdiff_t>(power(n-4, Dims)));
  }
}

TEST_CASE("path graph", "[dag::path_graph]") {
  std::size_t n = 1000;

  SECTION("periodic") {
    dag::undirected_network<std::size_t> periodic =
      dag::path_graph<std::size_t>(n, true);

    REQUIRE(periodic.vertices().size() == n);
    REQUIRE(periodic.edges().size() == n);

    auto verts = periodic.vertices();

    // test degrees
    REQUIRE(std::count_if(verts.begin(), verts.end(),
          [&periodic, n](std::size_t vert) {
            return periodic.degree(vert) == 2;
          }) == static_cast<std::ptrdiff_t>(verts.size()));

    // test local structure
    auto check_locality = [&periodic](std::size_t vert) {
      std::unordered_set<std::size_t> d2;

      for (auto&& n1: periodic.neighbours(vert))
        for (auto&& n2: periodic.neighbours(n1))
          d2.insert(n2);

      return d2.size() == 3;
    };
    REQUIRE(std::all_of(verts.begin(), verts.end(), check_locality));
  }

  SECTION("aperiodic") {
    dag::undirected_network<std::size_t> aperiodic =
      dag::path_graph<std::size_t>(n, false);

    REQUIRE(aperiodic.vertices().size() == n);
    REQUIRE(aperiodic.edges().size() == n - 1);

    auto verts = aperiodic.vertices();

    // test degrees
    REQUIRE(std::count_if(verts.begin(), verts.end(),
          [&aperiodic, n](std::size_t vert) {
            return aperiodic.degree(vert) == 2;
          }) == static_cast<std::ptrdiff_t>(n-2));
    REQUIRE(std::count_if(verts.begin(), verts.end(),
          [&aperiodic, n](std::size_t vert) {
            return aperiodic.degree(vert) == 1;
          }) == 2);

    // test local structure
    auto check_centre_locality = [&aperiodic](std::size_t vert) {
      std::unordered_set<std::size_t> d2;

      if (aperiodic.degree(vert) < 2)
        return false;

      for (auto&& n1: aperiodic.neighbours(vert))
        for (auto&& n2: aperiodic.neighbours(n1))
          d2.insert(n2);

      return d2.size() == 3;
    };

    // for now let's check locality not close to the boundaries:
    REQUIRE(
        std::count_if(
          verts.begin(), verts.end(),
          check_centre_locality) ==
        static_cast<std::ptrdiff_t>(power(n-4, 1)));
  }
}

TEST_CASE("cycle graph", "[dag::cycle_graph]") {
  std::size_t n = 1000;
  dag::undirected_network<std::size_t> cycle =
    dag::cycle_graph<std::size_t>(n);

  REQUIRE(cycle.vertices().size() == n);
  REQUIRE(cycle.edges().size() == n);

  auto verts = cycle.vertices();

  // test degrees
  REQUIRE(std::count_if(verts.begin(), verts.end(),
        [&cycle](std::size_t vert) {
          return cycle.degree(vert) == 2;
        }) == static_cast<std::ptrdiff_t>(verts.size()));

  // test local structure
  auto check_locality = [&cycle](std::size_t vert) {
    std::unordered_set<std::size_t> d2;

    for (auto&& n1: cycle.neighbours(vert))
      for (auto&& n2: cycle.neighbours(n1))
        d2.insert(n2);

    return d2.size() == 3;
  };
  REQUIRE(std::all_of(verts.begin(), verts.end(), check_locality));
}

TEST_CASE("regular ring lattice", "[dag::regular_ring_lattice]") {
  std::size_t n = 1000, k = 4;
  REQUIRE_THROWS(dag::regular_ring_lattice(n, k+1));

  dag::undirected_network<std::size_t> ring = dag::regular_ring_lattice(n, k);

  REQUIRE(ring.vertices().size() == n);
  REQUIRE(ring.edges().size() == n*k/2);

  auto verts = ring.vertices();

  // test degrees
  REQUIRE(std::count_if(verts.begin(), verts.end(),
        [&ring, k](std::size_t vert) {
          return ring.degree(vert) == k;
        }) == static_cast<std::ptrdiff_t>(verts.size()));

  // test local structure
  auto check_locality = [&ring, k](std::size_t vert) {
    std::unordered_set<std::size_t> d2;

    for (auto&& n1: ring.neighbours(vert))
      for (auto&& n2: ring.neighbours(n1))
        d2.insert(n2);

    return d2.size() == k*2+1;
  };
  REQUIRE(std::all_of(verts.begin(), verts.end(), check_locality));
}

TEST_CASE("complete graph", "[dag::complete_graph]") {
  std::size_t n = 100;
  dag::undirected_network<std::size_t> comp = dag::complete_graph(n);

  REQUIRE(comp.vertices().size() == n);
  REQUIRE(comp.edges().size() == n*(n-1)/2);
}

TEST_CASE("complete directed graph", "[dag::complete_directed_graph]") {
  std::size_t n = 100;
  dag::directed_network<std::size_t> comp = dag::complete_directed_graph(n);

  REQUIRE(comp.vertices().size() == n);
  REQUIRE(comp.edges().size() == n*(n-1));
}
