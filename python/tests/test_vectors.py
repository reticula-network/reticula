import reticula as ret
import reticula.test_helpers as ret_test

import pytest

import numpy as np


def test_creation():
    assert ret_test.vector_sum(
        ret.vector_int64([1, 2, 3, 4, 5])) == 15
    assert ret_test.vector_sum(
        ret.vector_uint64([1, 2, 3, 4, 5])) == 15
    assert ret_test.vector_sum(
        list(ret.vector_uint64([1, 2, 3, 4, 5]))) == 15
    assert ret_test.vector_sum(
        np.array([1, 2, 3, 4, 5], dtype=np.uint64)) == 15
    assert ret_test.vector_sum(
        np.array([1, 2, 3, 4, 5], dtype=np.int64)) == 15
    assert ret_test.vector_sum([1, 2, 3, 4, 5]) == 15
    assert ret_test.vector_sum(range(1, 6)) == 15
    assert ret_test.vector_sum(i for i in range(1, 6)) == 15


def test_explicit_construction():
    ret.vector_uint64(np.array([1, 2, 3, 4], dtype=np.uint64)),
    ret.vector_int64(ret.vector_uint64([1, 2, 3, 4, 5, 6])),
    # ret.vector_int64([1, 2, 3, 4, 5, 6.2]),
    # ret.vector_uint64(np.array([1.4, 2.3, 3.2, 4.0], dtype=np.double)),
    # ret.vector_int64(ret.vector_double([1.0, 2.3, 3, 4, 5, 6])),


def test_bad_implicit_construction():
    with pytest.raises(TypeError):
        ret_test.vector_sum([1, 2, 3, 4, 5, -6])

    with pytest.raises(TypeError):
        ret_test.vector_sum([1, 2, 3, 4, 5, 2**100])

    with pytest.raises(TypeError):
        ret_test.vector_sum([1, 2, 3, 4, 5, 6.2])

    with pytest.raises(TypeError):
        ret_test.vector_sum(
            np.array([1.1, 2.0, 3.0, 4.1, 5.2], dtype=np.double))

    with pytest.raises(TypeError):
        ret_test.vector_sum(i / 2 for i in range(100))

    with pytest.raises(TypeError):
        ret_test.vector_sum(ret.vector_double([1, 2, 3, 4, 5.2]))


def test_length():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([1, 2, 3, 4, 5])
    c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])

    assert len(a) == 5
    assert len(b) == 5
    assert len(c) == 5

    assert a[0] == 1
    assert b[1] == 2
    assert c[2] == 3.0


def test_indexing():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([1, 2, 3, 4, 5])
    c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])

    assert a[0] == 1
    assert a[-1] == 5
    assert b[1] == 2
    assert b[-2] == 4
    assert c[2] == 3.0
    assert c[-3] == 3.0

    with pytest.raises(IndexError):
        _ = a[5]

    with pytest.raises(IndexError):
        _ = b[-6]

    with pytest.raises(IndexError):
        _ = c[10]


def test_slicing():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([1, 2, 3, 4, 5])
    c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])

    assert a[1:4] == ret.vector_int64([2, 3, 4])
    assert b[:3] == ret.vector_uint64([1, 2, 3])
    assert c[2:] == ret.vector_double([3.0, 4.0, 5.0])

    assert a[-3:] == ret.vector_int64([3, 4, 5])
    assert b[:-1] == ret.vector_uint64([1, 2, 3, 4])
    assert c[::2] == ret.vector_double([1.0, 3.0, 5.0])


def test_iteration():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([1, 2, 3, 4, 5])
    c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])

    assert list(a) == [1, 2, 3, 4, 5]
    assert list(b) == [1, 2, 3, 4, 5]
    assert list(c) == [1.0, 2.0, 3.0, 4.0, 5.0]


def test_contains():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([1, 2, 3, 4, 5])
    c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])

    assert 3 in a
    assert 6 not in a
    assert 2 in b
    assert 7 not in b
    assert 4.0 in c
    assert 5.5 not in c

    assert all(x in a for x in [1, 2, 3])
    assert all(x not in b for x in [6, 7])


def test_dlpack():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([1, 2, 3, 4, 5])
    c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])

    assert a.__dlpack__() is not None
    assert b.__dlpack__() is not None
    assert c.__dlpack__() is not None

    # Check that the DLPack tensors can be converted back to numpy arrays
    assert np.from_dlpack(a).tolist() == [1, 2, 3, 4, 5]
    assert np.from_dlpack(b).tolist() == [1, 2, 3, 4, 5]
    assert np.from_dlpack(c).tolist() == [1.0, 2.0, 3.0, 4.0, 5.0]


# def test_repr():
#     a = ret.vector_int64([1, 2, 3, 4, 5])
#     b = ret.vector_uint64([1, 2, 3, 4, 5])
#     c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])
#
#     assert repr(a) == "vector_int64([1, 2, 3, 4, 5])"
#     assert repr(b) == "vector_uint64([1, 2, 3, 4, 5])"
#     assert repr(c) == "vector_double([1.0, 2.0, 3.0, 4.0, 5.0])"


# def test_str():
#     a = ret.vector_int64([1, 2, 3, 4, 5])
#     b = ret.vector_uint64([1, 2, 3, 4, 5])
#     c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])
#
#     assert str(a) == "vector_int64([1, 2, 3, 4, 5])"
#     assert str(b) == "vector_uint64([1, 2, 3, 4, 5])"
#     assert str(c) == "vector_double([1.0, 2.0, 3.0, 4.0, 5.0])"


def test_unhashable():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([1, 2, 3, 4, 5])
    c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])

    with pytest.raises(TypeError):
        _ = {a: "test"}
    with pytest.raises(TypeError):
        _ = {b: "test"}
    with pytest.raises(TypeError):
        _ = {c: "test"}


def test_bool():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([1, 2, 3, 4, 5])
    c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])

    assert bool(a) is True
    assert bool(b) is True
    assert bool(c) is True

    empty_a = ret.vector_int64([])
    empty_b = ret.vector_uint64([])
    empty_c = ret.vector_double([])

    assert bool(empty_a) is False
    assert bool(empty_b) is False
    assert bool(empty_c) is False


def test_clear():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([1, 2, 3, 4, 5])
    c = ret.vector_double([1.0, 2.0, 3.0, 4.0, 5.0])

    a.clear()
    b.clear()
    c.clear()

    assert len(a) == 0
    assert len(b) == 0
    assert len(c) == 0

    assert list(a) == []
    assert list(b) == []
    assert list(c) == []


def test_append():
    a = ret.vector_int64([1, 2, 3])
    b = ret.vector_uint64([4, 5, 6])
    c = ret.vector_double([7.0, 8.0, 9.0])

    a.append(4)
    b.append(7)
    c.append(10.0)

    assert list(a) == [1, 2, 3, 4]
    assert list(b) == [4, 5, 6, 7]
    assert list(c) == [7.0, 8.0, 9.0, 10.0]


def test_extend():
    a = ret.vector_int64([1, 2, 3])
    b = ret.vector_uint64([4, 5, 6])
    c = ret.vector_double([7.0, 8.0, 9.0])

    a.extend([4, 5])
    b.extend([7, 8])
    c.extend([10.0, 11.0])

    assert list(a) == [1, 2, 3, 4, 5]
    assert list(b) == [4, 5, 6, 7, 8]
    assert list(c) == [7.0, 8.0, 9.0, 10.0, 11.0]


def test_insert():
    a = ret.vector_int64([1, 2, 3])
    b = ret.vector_uint64([4, 5, 6])
    c = ret.vector_double([7.0, 8.0, 9.0])

    a.insert(1, 4)
    b.insert(2, 7)
    c.insert(0, 10.0)

    assert list(a) == [1, 4, 2, 3]
    assert list(b) == [4, 5, 7, 6]
    assert list(c) == [10.0, 7.0, 8.0, 9.0]


def test_pop():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([6, 7, 8, 9, 10])
    c = ret.vector_double([11.0, 12.0, 13.0])

    assert a.pop() == 5
    assert list(a) == [1, 2, 3, 4]

    assert b.pop(1) == 7
    assert list(b) == [6, 8, 9, 10]

    assert c.pop(0) == 11.0
    assert list(c) == [12.0, 13.0]


def test_remove():
    a = ret.vector_int64([1, 2, 3, 4, 5])
    b = ret.vector_uint64([6, 7, 8, 9, 10])
    c = ret.vector_double([11.0, 12.0, 13.0])

    a.remove(3)
    assert list(a) == [1, 2, 4, 5]

    with pytest.raises(ValueError):
        a.remove(10)  # Element not found

    b.remove(8)
    assert list(b) == [6, 7, 9, 10]

    c.remove(12.0)
    assert list(c) == [11.0, 13.0]


def test_count():
    a = ret.vector_int64([1, 2, 3, 2, 5])
    b = ret.vector_uint64([6, 7, 8, 7, 10])
    c = ret.vector_double([11.0, 12.0, 11.0])

    assert a.count(2) == 2
    assert a.count(4) == 0

    assert b.count(7) == 2
    assert b.count(9) == 0

    assert c.count(11.0) == 2
    assert c.count(13.0) == 0
