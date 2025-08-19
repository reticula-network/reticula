// IWYU pragma: always_keep
#pragma once

#include <reticula/algorithms/properties.hpp>
#include <reticula/concepts.hpp>

#include <catch2/catch_tostring.hpp>

namespace Catch {
template <reticula::network_edge T>
struct StringMaker<T> {
  static auto convert(const T& v) -> std::string {
    return std::format("{}", v);
  }
};

template <>
struct StringMaker<reticula::window> {
  static auto convert(const reticula::window& v) -> std::string {
    return std::format("{}", v);
  }
};
} // namespace Catch
