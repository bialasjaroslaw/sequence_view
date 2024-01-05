#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace SeqView {
using MaskT = uint8_t;
constexpr MaskT MASK_TRUE = 1;
constexpr MaskT MASK_FALSE = 0;

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
        begin(ptr),
        end(ptr + span.size()),
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
}  // namespace SeqView