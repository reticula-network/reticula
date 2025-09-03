import reticula as ret


def test_mersenne_twister():
    gen1 = ret.mersenne_twister(42)
    gen2 = ret.mersenne_twister(42)

    dist = ret.uniform_real_distribution(0.0, 1.0)
    val1 = dist(gen1)
    val2 = dist(gen2)
    assert val1 == val2


def test_uniform_real_distribution():
    gen = ret.mersenne_twister(42)
    dist = ret.uniform_real_distribution(0.0, 1.0)

    values = [dist(gen) for _ in range(100)]
    assert all(0.0 <= v <= 1.0 for v in values)
    assert len(set(values)) > 50


# def test_uniform_int_distribution():
#     gen = ret.mersenne_twister(42)
#     dist = ret.uniform_int_distribution(1, 6)
#
#     values = [dist(gen) for _ in range(100)]
#     assert all(1 <= v <= 6 for v in values)
#     assert all(isinstance(v, int) for v in values)


def test_exponential_distribution():
    gen = ret.mersenne_twister(42)
    dist = ret.exponential_distribution(1.0)

    values = [dist(gen) for _ in range(100)]
    assert all(v >= 0.0 for v in values)
    assert len(set(values)) > 50


def test_geometric_distribution():
    gen = ret.mersenne_twister(42)
    dist = ret.geometric_distribution(0.5)

    values = [dist(gen) for _ in range(100)]
    assert all(v >= 0 for v in values)


def test_delta_distribution():
    gen = ret.mersenne_twister(42)
    dist = ret.delta_distribution(5.0)

    values = [dist(gen) for _ in range(10)]
    assert all(v == 5.0 for v in values)


def test_power_law_with_specified_mean():
    gen = ret.mersenne_twister(42)
    dist = ret.power_law(exponent=2.5, mean=3.0)

    values = [dist(gen) for _ in range(100)]
    assert all(v >= 0.0 for v in values)


def test_residual_power_law_with_specified_mean():
    gen = ret.mersenne_twister(42)
    dist = ret.residual_power_law(exponent=2.5, mean=3.0)

    values = [dist(gen) for _ in range(100)]
    assert all(v >= 0.0 for v in values)


def test_hawkes_univariate_exponential():
    gen = ret.mersenne_twister(42)
    hawkes_dist = ret.hawkes_univariate_exponential(
        mu=1.0, alpha=0.5, theta=2.0)

    values = [hawkes_dist(gen) for _ in range(100)]
    assert all(v >= 0.0 for v in values)
