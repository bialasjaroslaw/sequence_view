#pragma once

#include <fmt/printf.h>

#include <cmath>
#include <cstdint>
#include <limits>
#include <span>

namespace SeqView {
using RangeT = int64_t;
constexpr RangeT START = 0;
constexpr RangeT END = std::numeric_limits<RangeT>::max();
constexpr RangeT STEP = 1;
constexpr RangeT MASK = 0;

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

struct MaskInfo {
  const uint8_t* ptr = nullptr;
  const uint8_t* begin = nullptr;
  const uint8_t* end = nullptr;

  MaskInfo() {}

  MaskInfo(const std::vector<uint8_t>& span)
      : ptr(&*span.begin()), begin(&*span.begin()), end(&*span.end()) {}

  int64_t next(int64_t steps = 1) const {
    fmt::print("Calc for idx {}\n", steps);
    if (steps < 0) return previous(-steps);
    auto current = ptr;
    while (current != end && steps != 0) {
      if (*++current) --steps;
    }
    return current - ptr;
  }

  int64_t previous(int64_t steps = 1) const {
    if (steps < 0) return next(-steps);
    auto current = ptr;
    while (current != begin && steps != 0) {
      if (*--current) --steps;
    }
    return ptr - current;
  }

  MaskInfo& operator+=(int64_t jump) {
    ptr += jump;
    return *this;
  }
};

template <typename T>
struct BaseIterator {
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

  BaseIterator(T* data, int64_t step = STEP, MaskInfo mask = {})
      : _data(data), _step(step), _dstep(step < 0 ? step : 0), _mask(mask) {}

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
    next_advance();
    return *this;
  }

  BaseIterator& operator--() {
    previous_advance();
    return *this;
  }

  BaseIterator operator+(int steps) const {
    return BaseIterator<T>(next(steps), _step);
  }

  BaseIterator operator+=(int steps) {
    next_advance(steps);
    return *this;
  }

  BaseIterator operator-(int steps) const {
    return BaseIterator<T>(previous(steps), _step);
  }

  // TODO fix for mask
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

  pointer next(int64_t steps = 1) const {
    auto jump = _step != MASK ? steps * _step : next_mask(steps);
    return _data + jump;
  }

  pointer previous(int64_t steps = 1) const {
    auto jump = _step != MASK ? steps * _step : previous_mask(steps);
    return _data - jump;
  }

  pointer next_advance(int64_t steps = 1) {
    auto ptr = next(steps);
    _mask += (ptr - _data);
    _data = ptr;
    return _data;
  }

  pointer previous_advance(int64_t steps = 1) {
    auto ptr = previous(steps);
    _mask += (ptr - _data);
    _data = ptr;
    return _data;
  }

  int64_t next_mask(int64_t steps = 1) const { return _mask.next(steps); }

  int64_t previous_mask(int64_t steps = 1) const {
    return _mask.previous(steps);
  }

 private:
  T* _data = nullptr;
  int64_t _step = STEP;
  int64_t _dstep = 0;
  MaskInfo _mask;
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

  View(pointer ptr, std::vector<uint8_t> mask)
      : _ptr(ptr),
        _end(ptr + mask.size()),
        _step(MASK),
        _size(std::count(mask.cbegin(), mask.cend(), true)),
        _mask(std::move(mask)) {}

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

  View operator()(const std::vector<uint8_t>& mask) { return View(_ptr, mask); }

  reference operator[](uint64_t idx) { return *element_at(idx); }

  const_reference operator[](uint64_t idx) const { return *element_at(idx); }

  friend std::vector<bool> operator<(const View& view, const T& val) {
    std::vector<bool> mask;
    mask.resize(view.size());
    uint64_t idx = 0;
    for (const auto& elem : view) mask[idx++] = elem < val;
    return mask;
  }

 protected:
  T* element_at(int idx) const {
    if (_step == MASK) {
      return _ptr + MaskInfo(_mask).next(idx);
    }
    return _ptr + idx * _step + (_step < 0 ? _step : 0);
  }

 private:
  int64_t _step;
  uint64_t _size = 0;
  pointer _ptr = nullptr;
  pointer _end = nullptr;
  std::vector<uint8_t> _mask;
};
}  // namespace SeqView