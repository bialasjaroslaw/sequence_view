#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <span>
#include <vector>

namespace SeqView {
using RangeT = int64_t;
constexpr RangeT START = 0;
constexpr RangeT END = std::numeric_limits<RangeT>::max();
constexpr RangeT STEP = 1;
constexpr RangeT MASK = 0;
using MaskT = uint8_t;
constexpr MaskT MASK_TRUE = 1;
constexpr MaskT MASK_FALSE = 0;

struct Range {
  Range(RangeT start, RangeT stop, RangeT step = STEP)
      : _start(start), _stop(stop), _step(step) {
    if (_start > _stop) std::swap(_start, _stop);
  }
  RangeT _start = START;
  RangeT _stop = END;
  RangeT _step = STEP;

  RangeT start() const { return _start; }
  RangeT stop() const { return _stop; }
  RangeT step() const { return _step; }
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
  const MaskT* ptr = nullptr;
  const MaskT* begin = nullptr;
  const MaskT* end = nullptr;
  uint64_t cnt = 0;
  std::vector<uint64_t> valid_until;

  static std::vector<uint64_t> init_valid(const std::vector<MaskT>& span) {
    std::vector<uint64_t> valids;
    valids.resize(span.size() + 1);
    uint64_t totals = 0;
    for (uint64_t idx = 0; idx < span.size(); ++idx) {
      totals += span.at(idx);
      valids.at(idx + 1) = totals;
    }
    return valids;
  }

  MaskInfo() {}

  MaskInfo(const std::vector<uint8_t>& span)
      : ptr(&*span.begin()),
        begin(&*span.begin()),
        end(&*span.end()),
        cnt(static_cast<uint64_t>(
            std::count(span.begin(), span.end(), MASK_TRUE))),
        valid_until(init_valid(span)) {}

  int64_t next(uint64_t steps = 1) const {
    if (ptr == end) return 0;
    auto current = ptr;
    while (steps != 0) {
      if (++current == end) break;
      if (*current == MASK_TRUE) --steps;
    }
    return current - ptr;
  }

  int64_t previous(uint64_t steps = 1) const {
    auto current = ptr;
    while (steps != 0 && current != begin) {
      --current;
      if (*current == MASK_TRUE) --steps;
    }
    return ptr - current;
  }

  uint64_t valid() const {
    return valid_until.at(static_cast<uint64_t>(ptr - begin));
  }

  MaskInfo& operator+=(int64_t jump) {
    ptr += jump;
    return *this;
  }

  MaskInfo operator+(int64_t jump) const {
    auto copy = *this;
    copy += jump;
    return copy;
  }

  uint64_t count() const { return cnt; }

  bool good() const {
    return ptr != nullptr && begin != nullptr && end != nullptr;
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
    uint64_t steps_abs = static_cast<uint64_t>(std::abs(steps));
    return BaseIterator<T>(steps >= 0 ? next(steps_abs) : previous(steps_abs),
                           _step);
  }

  BaseIterator operator+=(int steps) {
    uint64_t steps_abs = static_cast<uint64_t>(std::abs(steps));
    steps > 0 ? next_advance(steps_abs) : previous_advance(steps_abs);
    return *this;
  }

  BaseIterator operator-(int steps) const {
    uint64_t steps_abs = static_cast<uint64_t>(std::abs(steps));
    return BaseIterator<T>(steps >= 0 ? previous(steps_abs) : next(steps_abs),
                           _step);
  }

  friend int64_t operator-(const BaseIterator<T>& lhs,
                           const BaseIterator<T>& rhs) {
    return lhs._mask.good()
               ? static_cast<int64_t>(lhs._mask.valid()) -
                     static_cast<int64_t>(rhs._mask.valid())
               : (lhs._data + lhs._dstep - rhs._data - rhs._dstep) / lhs._step;
  }

  reference operator*() const { return *ptr(); }
  pointer operator->() const { return ptr(); }
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

template <typename T>
class View {
 public:
  using size_type = std::size_t;
  using value_type = T;
  using pointer = T*;
  using const_pointer = T const*;
  using iterator = BaseIterator<T>;
  // TODO, fix type for const_iterator (iterator can be modified, access can not
  // modify values)
  using const_iterator = const BaseIterator<T>;
  using reference = T&;
  using const_reference = T const&;

  constexpr static int64_t validate_step(int64_t step) {
    if (step == 0) return 1;
    return step;
  }

  constexpr static uint64_t elements(uint64_t size, int64_t step) {
    if (size == 0 || step == 0) return 0;
    uint64_t step_abs = static_cast<uint64_t>(std::abs(step));
    if (step_abs > size) return 1;
    return (size - 1) / step_abs + 1;
  }

  constexpr static pointer last_ptr(pointer ptr, uint64_t size, int64_t step) {
    auto number_of_jumps = elements(size, step);
    auto first_outside_boundary =
        number_of_jumps * static_cast<uint64_t>(std::abs(step));
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
        _info(mask),
        _mask(std::move(mask)) {
    _size = _info.count();
  }

  uint64_t size() const { return _step != MASK ? _size : _info.count(); }

  iterator begin() const { return iterator(_ptr, _step, _info); }

  iterator end() const { return iterator(_end, _step, _info + (_end - _ptr)); }

  const_iterator cbegin() const { return const_iterator(_ptr, _step, _info); }
  const_iterator cend() const {
    return const_iterator(_end, _step, _info + (_end - _ptr));
  }

  View operator()(Range rng) {
    return rng._start > rng._stop
               ? View(_ptr + rng._start, 0, rng._step)
               : View(_ptr + rng._start,
                      static_cast<uint64_t>(rng._stop - rng._start), rng._step);
  }

  View operator()(const std::vector<uint8_t>& mask) { return View(_ptr, mask); }

  reference operator[](uint64_t idx) { return *element_at(idx); }

  const_reference operator[](uint64_t idx) const { return *element_at(idx); }

  friend std::vector<uint8_t> operator<(const View& view, const T& val) {
    std::vector<uint8_t> mask;
    mask.resize(view.size());
    uint64_t idx = 0;
    for (const auto& elem : view)
      mask[idx++] = elem < val ? MASK_TRUE : MASK_FALSE;
    return mask;
  }

 protected:
  T* element_at(uint64_t idx) const {
    if (_step == MASK) {
      return _ptr + _info.next(idx);
    }
    return _ptr + static_cast<int64_t>(idx) * _step + (_step < 0 ? _step : 0);
  }

 private:
  pointer _ptr = nullptr;
  pointer _end = nullptr;
  int64_t _step = 1;
  uint64_t _size = 0;
  MaskInfo _info;
  std::vector<uint8_t> _mask;
};
}  // namespace SeqView