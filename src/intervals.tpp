#include <queue>
#include <cmath>

namespace dag {
  template <typename T>
  bool interval_set<T>::can_merge(std::pair<T, T> a, std::pair<T, T> b) const {
    return std::max(a.first, b.first) <= std::min(a.second, b.second);
  }

  template <typename T>
  void interval_set<T>::insert(T start, T end) {
    if (end < start)
      throw std::invalid_argument("interval end cannot be less than the start");

    typename std::vector<std::pair<T, T>>::iterator ostart;
    if (_ints.empty() || start > _ints.back().second)
      ostart = _ints.end();
    else
      ostart = std::ranges::lower_bound(
          _ints, start, std::ranges::less{}, [](auto& p) { return p.second; });
    auto oend = ostart;

    std::pair<T, T> current(start, end);
    while (oend != _ints.end() && can_merge(current, *oend)) {
      current.first = std::min(current.first, oend->first);
      current.second = std::max(current.second, oend->second);
      oend++;
    }

    if (ostart == oend) {
      _ints.insert(ostart, current);
    } else {
      *(ostart++) = current;
      std::move(oend, _ints.end(), ostart);
    }
  }

  template <typename T>
  void interval_set<T>::merge(const interval_set<T>& cs) {
    std::vector<std::pair<T, T>> out;
    out.reserve((_ints.capacity() + cs._ints.capacity()));

    auto f1 = _ints.begin(), l1= _ints.end();
    auto f2 = cs._ints.begin(), l2 = cs._ints.end();
    while (f1 != l1 && f2 != l2) {
      if (out.empty())
        out.push_back(std::min(*f1, *f2));
      auto& c = out.back();
      if (*f1 < *f2) {
        if (f1->first >= c.first && f1->first <= c.second)
          c.second = std::max(c.second, f1->second);
        else
          out.push_back(*f1);
        f1++;
      } else {
        if (f2->first >= c.first && f2->first <= c.second)
          c.second = std::max(c.second, f2->second);
        else
          out.push_back(*f2);
        f2++;
      }
    }

    while (f1 != l1) {
      if (out.empty())
        out.push_back(*f1);
      if (f1->first >= out.back().first && f1->first <= out.back().second)
        out.back().second = std::max(out.back().second, f1->second);
      else
        out.push_back(*f1);
      f1++;
    }

    while (f2 != l2) {
      if (out.empty())
        out.push_back(*f2);
      if (f2->first >= out.back().first && f2->first <= out.back().second)
        out.back().second = std::max(out.back().second, f2->second);
      else
        out.push_back(*f2);
      f2++;
    }

    _ints.swap(out);
  }

  template <typename T>
  bool interval_set<T>::is_covered(T time) const {
    auto lower = std::ranges::lower_bound(
        _ints, time, std::ranges::less{}, [](auto& p) { return p.second; });
    if (lower != _ints.end() &&
        lower->first < time && time <= lower->second)
      return true;
    else
      return false;
  }

  template <typename T>
  T interval_set<T>::cover() const {
    T total {};
    for (auto& [s, e]: _ints)
      total += e - s;
    return total;
  }

  template <typename T>
  typename interval_set<T>::IteratorType
  interval_set<T>::begin() const {
    return _ints.cbegin();
  }

  template <typename T>
  typename interval_set<T>::IteratorType
  interval_set<T>::end() const {
    return _ints.cend();
  }
}  // namespace dag
