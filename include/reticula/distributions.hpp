#ifndef INCLUDE_RETICULA_DISTRIBUTIONS_HPP_
#define INCLUDE_RETICULA_DISTRIBUTIONS_HPP_

#include <random>
#include <concepts>

namespace reticula {
  template <typename T>
  concept random_number_distribution =
    std::is_arithmetic_v<typename T::result_type> &&
    requires(T dist, std::mt19937_64& gen) {
      { dist(gen) } -> std::convertible_to<typename T::result_type>;
    };  // NOLINT(readability/braces)

  template <std::floating_point RealType = double>
  class power_law_with_specified_mean {
  public:
    using result_type = RealType;

    power_law_with_specified_mean(RealType exponent, RealType mean);

    template <std::uniform_random_bit_generator Generator>
    RealType operator()(Generator& g) const;

    RealType x_min() const;
    RealType exponent() const;
    RealType mean() const;
  private:
    RealType _x_min, _exponent, _mean;
  };

  template <std::floating_point RealType = double>
  class residual_power_law_with_specified_mean {
  public:
    using result_type = RealType;

    residual_power_law_with_specified_mean(RealType exponent, RealType mean);

    template <std::uniform_random_bit_generator Generator>
    RealType operator()(Generator& g) const;

    RealType x_min() const;
    RealType exponent() const;
    RealType mean() const;
  private:
    RealType _x_min, _exponent, _mean;
  };

  template <std::floating_point RealType = double>
  class hawkes_univariate_exponential {
  public:
    using result_type = RealType;

    hawkes_univariate_exponential(
        RealType mu, RealType alpha, RealType theta, RealType phi = {});

    template <std::uniform_random_bit_generator Generator>
    RealType operator()(Generator& g);

    RealType mu() const;
    RealType alpha() const;
    RealType theta() const;
    RealType phi() const;
  private:
    RealType _mu, _alpha, _theta, _phi;
  };

  template <typename ResultType = double>
  requires std::integral<ResultType> || std::floating_point<ResultType>
  class delta_distribution {
  public:
    using result_type = ResultType;

    explicit delta_distribution(ResultType mean);

    template <std::uniform_random_bit_generator Generator>
    ResultType operator()(Generator& g);

    ResultType mean() const;
  private:
    ResultType _mean;
  };

}  // namespace reticula

#include "../../src/distributions.tpp"

#endif  // INCLUDE_RETICULA_DISTRIBUTIONS_HPP_
