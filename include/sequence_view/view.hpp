#pragma once

#include <cstdint>
#include <vector>

namespace SeqView {
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

  View operator()(const std::vector<uint8_t>& mask) {
    if (_size < mask.size()) return View(_ptr, 0);
    return View(_ptr, mask);
  }

  reference operator[](uint64_t idx) { return *element_at(idx); }

  const_reference operator[](uint64_t idx) const { return *element_at(idx); }

  friend std::vector<uint8_t> operator<(const View& view, const T& val) {
    std::vector<uint8_t> mask;
    mask.resize(view.size());
    uint64_t idx = 0;
    for (const auto& elem : view)
      mask.at(idx++) = elem < val ? MASK_TRUE : MASK_FALSE;
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