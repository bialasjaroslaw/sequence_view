#pragma once

#include <cstdint>
#include <sequence_view/mask.hpp>
#include <sequence_view/range.hpp>

namespace SeqView {

template <typename T>
struct BaseIterator {
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

  BaseIterator() = default;

  BaseIterator(T* data, int64_t step = STEP, MaskInfo mask = {})
      : _data(data), _step(step), _dstep(step < 0 ? step : 0), _mask(mask) {}

  BaseIterator& operator++() {
    next_advance();
    return *this;
  }

  BaseIterator operator++(int) {
    auto copy = *this;
    operator++();
    return copy;
  }

  BaseIterator& operator--() {
    previous_advance();
    return *this;
  }

  BaseIterator operator--(int) {
    auto copy = *this;
    operator--();
    return copy;
  }

  BaseIterator& operator+=(difference_type steps) {
    uint64_t steps_abs = static_cast<uint64_t>(std::abs(steps));
    steps > 0 ? next_advance(steps_abs) : previous_advance(steps_abs);
    return *this;
  }

  BaseIterator& operator-=(difference_type steps) {
    uint64_t steps_abs = static_cast<uint64_t>(std::abs(steps));
    steps > 0 ? previous_advance(steps_abs) : next_advance(steps_abs);
    return *this;
  }

  friend bool operator==(const BaseIterator<T>& lhs,
                         const BaseIterator<T>& rhs) {
    return lhs._data == rhs._data;
  }

  friend bool operator!=(const BaseIterator<T>& lhs,
                         const BaseIterator<T>& rhs) {
    return !(lhs == rhs);
  }

  friend bool operator<(const BaseIterator<T>& lhs,
                        const BaseIterator<T>& rhs) {
    return lhs._step > 0 ? lhs._data < rhs._data : lhs._data > rhs._data;
  }

  friend bool operator>(const BaseIterator<T>& lhs,
                        const BaseIterator<T>& rhs) {
    return rhs < lhs;
  }

  friend bool operator<=(const BaseIterator<T>& lhs,
                         const BaseIterator<T>& rhs) {
    return !(rhs < lhs);
  }

  friend bool operator>=(const BaseIterator<T>& lhs,
                         const BaseIterator<T>& rhs) {
    return !(lhs < rhs);
  }

  friend BaseIterator operator+(const BaseIterator& it, difference_type steps) {
    auto copy = it;
    copy += steps;
    return copy;
  }

  friend BaseIterator operator+(difference_type steps, const BaseIterator& it) {
    return it + steps;
  }

  friend BaseIterator operator-(const BaseIterator& it, difference_type steps) {
    uint64_t steps_abs = static_cast<uint64_t>(std::abs(steps));
    return BaseIterator<T>(
        steps >= 0 ? it.previous(steps_abs) : it.next(steps_abs), it._step);
  }

  friend difference_type operator-(const BaseIterator<T>& lhs,
                                   const BaseIterator<T>& rhs) {
    return lhs._mask.good()
               ? static_cast<difference_type>(lhs._mask.valid()) -
                     static_cast<difference_type>(rhs._mask.valid())
               : (lhs._data + lhs._dstep - rhs._data - rhs._dstep) / lhs._step;
  }

  reference operator*() const { return *ptr(); }
  pointer operator->() const { return ptr(); }
  reference operator[](difference_type steps) const {
    uint64_t steps_abs = static_cast<uint64_t>(std::abs(steps));
    if (steps < 0) return *(previous(steps_abs) + _dstep);
    return *(next(steps_abs) + _dstep);
  }  // TODO Check
  pointer base() const { return ptr(); }

 protected:
  T* ptr() const { return _data + _dstep; }

  pointer next(uint64_t steps = 1) const {
    auto jump =
        _step != MASK ? static_cast<int64_t>(steps) * _step : next_mask(steps);
    return _data + jump;
  }

  pointer previous(uint64_t steps = 1) const {
    auto jump = _step != MASK ? static_cast<int64_t>(steps) * _step
                              : previous_mask(steps);
    return _data - jump;
  }

  pointer next_advance(uint64_t steps = 1) {
    auto ptr = next(steps);
    _mask += (ptr - _data);
    _data = ptr;
    return _data;
  }

  pointer previous_advance(uint64_t steps = 1) {
    auto ptr = previous(steps);
    _mask += (ptr - _data);
    _data = ptr;
    return _data;
  }

  int64_t next_mask(uint64_t steps = 1) const { return _mask.next(steps); }

  int64_t previous_mask(uint64_t steps = 1) const {
    return _mask.previous(steps);
  }

 private:
  T* _data = nullptr;
  int64_t _step = STEP;
  int64_t _dstep = 0;
  MaskInfo _mask;
};
}  // namespace SeqView
