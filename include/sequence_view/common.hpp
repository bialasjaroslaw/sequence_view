#pragma once

#include <cstdint>
#include <cmath>
#include <limits>

namespace SeqView{
	using RangeT = int64_t;
	constexpr RangeT START = 0;
	constexpr RangeT END = std::numeric_limits<RangeT>::max();
	constexpr RangeT STEP = 1;

	struct Range{
		Range(RangeT start, RangeT stop, RangeT step = STEP)
		: start(start)
		, stop(stop)
		, step(step)
		{}
		RangeT start = START;
		RangeT stop = END;
		RangeT step = STEP;
	};

	struct StepRange: Range{
		StepRange(RangeT step) 
		: Range(START, END, step)
		{}
	};

	struct StartRange: Range{
		StartRange(RangeT start, RangeT step = STEP) 
		: Range(start, END, step)
		{}
	};

	struct StopRange: Range{
		StopRange(RangeT stop, RangeT step = STEP) 
		: Range(START, stop, step)
		{}
	};

	template <typename T>
	class View{
	public:
		using size_type = std::size_t;
        using value_type = T;
        using pointer = T*;
        using const_pointer = T const*;
        using reference = T&;
        using const_reference = T const&;

		View(T* ptr, uint64_t size, int64_t step = 1)
		: _ptr(ptr)
		, _step(step == 0 ? step = 1 : step)
		, _size(size)
		{}

		uint64_t size() const{
			return _size / std::abs(_step);
		}
		pointer begin(){
			return _ptr;
		}
		pointer end(){
			return _ptr + _size;
		}

		const_pointer begin() const {
			return _ptr;
		}
		const_pointer end() const {
			return _ptr + _size;
		}

		View operator()(Range rng) {
			return View(_ptr + rng.start, rng.stop - rng.start, rng.step);
		}

		reference operator[](uint64_t idx){
			return *(_ptr + idx * _step);
		}

		const_reference operator[](uint64_t idx) const{
			return *(_ptr + idx * _step);
		}

	private:
		pointer _ptr = nullptr;
		int64_t _step;
		uint64_t _size = 0;
	};
}