#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

namespace SeqView {
using RangeT = int64_t;
constexpr RangeT START = 0;
constexpr RangeT END = std::numeric_limits<RangeT>::max();
constexpr RangeT STEP = 1;

struct Range {
  Range(RangeT start, RangeT stop, RangeT step = STEP)
      : start(start), stop(stop), step(step) {}
  RangeT start = START;
  RangeT stop = END;
  RangeT step = STEP;
};

struct StepRange : Range {
  StepRange(RangeT step) : Range(START, END, step) {}
};

struct StartRange : Range {
  StartRange(RangeT start, RangeT step = STEP) : Range(start, END, step) {}
};

struct StopRange : Range {
  StopRange(RangeT stop, RangeT step = STEP) : Range(START, stop, step) {}
};

template <typename T>
struct BaseIterator {
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

  BaseIterator(T* data, int64_t step = STEP)
      : _data(data), _step(step), _dstep(step < 0 ? step : 0) {}

  const T& operator*() const { return *ptr(); }

  const T* operator->() const { return ptr(); }

  friend bool operator==(const BaseIterator<T>& lhs,
                         const BaseIterator<T>& rhs) {
    return lhs._data == rhs._data;
  }

  friend bool operator!=(const BaseIterator<T>& lhs,
                         const BaseIterator<T>& rhs) {
    return lhs._data != rhs._data;
  }

  friend bool operator<(const BaseIterator<T>& lhs,
                        const BaseIterator<T>& rhs) {
    return lhs._data < rhs._data;
  }

  BaseIterator& operator++() {
    _data += _step;
    return *this;
  }

  BaseIterator& operator--() {
    _data -= _step;
    return *this;
  }

  BaseIterator operator+(int steps) {
    return BaseIterator<T>(_data + _step * steps, _step);
  }

  BaseIterator operator+=(int steps) {
    _data += _step * steps;
    return *this;
  }

  BaseIterator operator-(int steps) {
    return BaseIterator<T>(_data - _step * steps, _step);
  }

  friend int64_t operator-(const BaseIterator<T>& lhs,
                           const BaseIterator<T>& rhs) {
    return (lhs._data + lhs._dstep - rhs._data - rhs._dstep) / lhs._step;
  }

  T& operator*() { return *BaseIterator<T>::ptr(); }

  T* operator->() { return BaseIterator<T>::ptr(); }

  T* base() { return BaseIterator<T>::ptr(); }

  const T* base() const { return BaseIterator<T>::ptr(); }

 protected:
  T* ptr() { return _data + _dstep; }
  const T* ptr() const { return _data + _dstep; }

 private:
  T* _data = nullptr;
  int64_t _step = STEP;
  int64_t _dstep = 0;
};

template <typename T>
class View {
 public:
  using size_type = std::size_t;
  using value_type = T;
  using pointer = T*;
  using const_pointer = T const*;
  using iterator = BaseIterator<T>;
  using const_iterator = const BaseIterator<T>;
  using reference = T&;
  using const_reference = T const&;

  constexpr static int64_t validate_step(int64_t step) {
    if (step == 0) return 1;
    return step;
  }

  constexpr static uint64_t elements(uint64_t size, int64_t step) {
    if (size == 0 || step == 0) return 0;
    step = std::abs(step);
    if (step > size) return 1;
    return (size - 1) / step + 1;
  }

  constexpr static pointer last_ptr(pointer ptr, uint64_t size, int64_t step) {
    auto number_of_jumps = elements(size, step);
    auto first_outside_boundary = number_of_jumps * std::abs(step);
    return ptr + first_outside_boundary;
  }

  constexpr static pointer base_ptr(pointer ptr, uint64_t size, int64_t step) {
    if (step > 0) return ptr;
    return last_ptr(ptr, size, step);
  }

  constexpr static pointer end_ptr(pointer ptr, uint64_t size, int64_t step) {
    if (step < 0) return ptr;
    return last_ptr(ptr, size, step);
  }

  View(pointer ptr, uint64_t size, int64_t step = 1)
      : _ptr(base_ptr(ptr, size, step)),
        _end(end_ptr(ptr, size, step)),
        _step(validate_step(step)),
        _size(elements(size, step)) {}

  uint64_t size() const { return _size; }

  iterator begin() { return iterator(_ptr, _step); }

  iterator end() { return iterator(_end, _step); }

  const_iterator begin() const { return const_iterator(_ptr, _step); }

  const_iterator end() const { return const_iterator(_end, _step); }

  const_iterator cbegin() const { return const_iterator(_ptr, _step); }
  const_iterator cend() const { return const_iterator(_end, _step); }

  View operator()(Range rng) {
    return View(_ptr + rng.start, rng.stop - rng.start, rng.step);
  }

  reference operator[](uint64_t idx) { return *element_at(idx); }

  const_reference operator[](uint64_t idx) const { return *element_at(idx); }

 protected:
  T* element_at(int idx) const {
    return _ptr + idx * _step + (_step < 0 ? _step : 0);
  }

 private:
  int64_t _step;
  uint64_t _size = 0;
  pointer _ptr = nullptr;
  pointer _end = nullptr;
};
}  // namespace SeqView