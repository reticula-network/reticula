#include <vector>
#include <tuple>
#include <compare>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <catch2/catch_approx.hpp>

#include <reticula/utils.hpp>

TEST_CASE("Test compare function") {
    SECTION("Simple types") {
        REQUIRE(reticula::utils::compare(1, 2) ==
                std::weak_ordering::less);
        REQUIRE(reticula::utils::compare(2, 1) ==
                std::weak_ordering::greater);
        REQUIRE(reticula::utils::compare(1, 1) ==
                std::weak_ordering::equivalent);
    }

    SECTION("Vectors") {
        std::vector<int> a = {1, 2, 3};
        std::vector<int> b = {1, 2, 4};
        std::vector<int> c = {1, 2, 3, 4};
        REQUIRE(reticula::utils::compare(a, b) ==
                std::weak_ordering::less);
        REQUIRE(reticula::utils::compare(b, a) ==
                std::weak_ordering::greater);
        REQUIRE(reticula::utils::compare(a, a) ==
                std::weak_ordering::equivalent);
        REQUIRE(reticula::utils::compare(a, c) ==
                std::weak_ordering::less);
        REQUIRE(reticula::utils::compare(c, a) ==
                std::weak_ordering::greater);
    }

    SECTION("Tuples") {
        auto a = std::make_tuple(1, 2, 3);
        auto b = std::make_tuple(1, 2, 4);
        REQUIRE(reticula::utils::compare(a, b) ==
                std::weak_ordering::less);
        REQUIRE(reticula::utils::compare(b, a) ==
                std::weak_ordering::greater);
        REQUIRE(reticula::utils::compare(a, a) ==
                std::weak_ordering::equivalent);

        auto c = std::make_tuple(1, 2.0, std::string("abc"));
        auto d = std::make_tuple(1, 2.0, std::string("def"));
        REQUIRE(reticula::utils::compare(c, d) ==
                std::weak_ordering::less);
        REQUIRE(reticula::utils::compare(d, c) ==
                std::weak_ordering::greater);
        REQUIRE(reticula::utils::compare(c, c) ==
                std::weak_ordering::equivalent);
    }
}
