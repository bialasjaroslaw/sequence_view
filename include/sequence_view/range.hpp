#pragma once

#include <cstdint>
#include <limits>

namespace SeqView {
using RangeT = int64_t;
constexpr RangeT START = 0;
constexpr RangeT END = std::numeric_limits<RangeT>::max();
constexpr RangeT STEP = 1;
constexpr RangeT MASK = 0;

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
}  // namespace SeqView