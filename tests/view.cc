#include <fmt/format.h>
#include <gmock/gmock-more-matchers.h>
#include <gtest/gtest-matchers.h>
#include <gtest/gtest.h>

#include <numeric>
#include <sequence_view/base-iterator.hpp>
#include <sequence_view/mask.hpp>
#include <sequence_view/range.hpp>
#include <sequence_view/view.hpp>

using namespace ::testing;

static void init_range(uint64_t* ptr, uint64_t len) {
  std::iota(ptr, ptr + len, 0);
}

TEST(View, ValidateStep) {
  std::vector<int64_t> input{-2, -1, 0, 1, 2};
  std::vector<int64_t> expected{-2, -1, 1, 1, 2};

  for (auto idx = 0U; idx < input.size(); ++idx)
    EXPECT_THAT(SeqView::View<uint64_t>::validate_step(input[idx]),
                Eq(expected[idx]))
        << fmt::format("Failed for {}", input[idx]);
}

TEST(View, Elements) {
  std::vector<uint64_t> input_size{0, 0,  1,  1,  9, 10, 11, 12, 13,
                                   9, 10, 11, 12, 9, 9,  10, 11, 12};
  std::vector<int64_t> input_step{0, 1, 1, 2, 3,  3,  3,  3,  3,
                                  2, 2, 2, 2, -2, -3, -3, -3, -3};
  std::vector<int64_t> expected{0, 0, 1, 1, 3, 4, 4, 4, 5,
                                5, 5, 6, 6, 5, 3, 4, 4, 4};

  for (auto idx = 0U; idx < input_size.size(); ++idx)
    EXPECT_THAT(
        SeqView::View<uint64_t>::elements(input_size[idx], input_step[idx]),
        Eq(expected[idx]))
        << fmt::format("Failed for {}/{}", input_size[idx], input_step[idx]);
}

TEST(View, LastPtr) {
  uint16_t data = 0;
  auto ptr = &data;
  auto PC = [=](uint16_t value) { return ptr + value; };
  std::vector<uint64_t> input_size{0, 0,  1,  1,  9, 10, 11, 12, 13,
                                   9, 10, 11, 12, 9, 9,  10, 11, 12};
  std::vector<int64_t> input_step{0, 1, 1, 2, 3,  3,  3,  3,  3,
                                  2, 2, 2, 2, -2, -3, -3, -3, -3};
  // expected is smallest multiple of step larger or equal size
  std::vector<uint16_t*> expected{
      PC(0),  PC(0),  PC(1),  PC(2),  PC(9),  PC(12), PC(12), PC(12), PC(15),
      PC(10), PC(10), PC(12), PC(12), PC(10), PC(9),  PC(12), PC(12), PC(12)};

  for (auto idx = 0U; idx < input_size.size(); ++idx) {
    EXPECT_THAT(SeqView::View<uint16_t>::last_ptr(ptr, input_size[idx],
                                                  input_step[idx]),
                Eq(expected[idx]))
        << fmt::format("Failed for {}/{}/{}", fmt::ptr(ptr), input_size[idx],
                       input_step[idx]);
  }
}

TEST(View, BasePtr) {
  uint16_t data = 0;
  auto ptr = &data;
  auto PC = [=](uint16_t value) { return ptr + value; };
  std::vector<uint64_t> input_size{0, 0,  1,  1,  9, 10, 11, 12, 13,
                                   9, 10, 11, 12, 9, 9,  10, 11, 12};
  std::vector<int64_t> input_step{0, 1, 1, 2, 3,  3,  3,  3,  3,
                                  2, 2, 2, 2, -2, -3, -3, -3, -3};
  // expected is smallest multiple of step larger or equal size
  std::vector<uint16_t*> expected{PC(0), PC(0),  PC(0), PC(0),  PC(0),  PC(0),
                                  PC(0), PC(0),  PC(0), PC(0),  PC(0),  PC(0),
                                  PC(0), PC(10), PC(9), PC(12), PC(12), PC(12)};

  for (auto idx = 0U; idx < input_size.size(); ++idx) {
    EXPECT_THAT(SeqView::View<uint16_t>::base_ptr(ptr, input_size[idx],
                                                  input_step[idx]),
                Eq(expected[idx]))
        << fmt::format("Failed for {}/{}/{}", fmt::ptr(ptr), input_size[idx],
                       input_step[idx]);
  }
}

TEST(View, EndPtr) {
  uint16_t data = 0;
  auto ptr = &data;
  auto PC = [=](uint16_t value) { return ptr + value; };
  std::vector<uint64_t> input_size{0, 0,  1,  1,  9, 10, 11, 12, 13,
                                   9, 10, 11, 12, 9, 9,  10, 11, 12};
  std::vector<int64_t> input_step{0, 1, 1, 2, 3,  3,  3,  3,  3,
                                  2, 2, 2, 2, -2, -3, -3, -3, -3};
  // expected is smallest multiple of step larger or equal size
  std::vector<uint16_t*> expected{
      PC(0),  PC(0),  PC(1),  PC(2),  PC(9), PC(12), PC(12), PC(12), PC(15),
      PC(10), PC(10), PC(12), PC(12), PC(0), PC(0),  PC(0),  PC(0),  PC(0)};

  for (auto idx = 0U; idx < input_size.size(); ++idx) {
    EXPECT_THAT(
        SeqView::View<uint16_t>::end_ptr(ptr, input_size[idx], input_step[idx]),
        Eq(expected[idx]))
        << fmt::format("Failed for {}/{}/{}", fmt::ptr(ptr), input_size[idx],
                       input_step[idx]);
  }
}

TEST(View, Make1DView) {
  uint64_t data[100];
  SeqView::View view(data, 100);
  EXPECT_THAT(view, SizeIs(100));
}

TEST(View, IterateAssignOver1DView) {
  uint64_t data[100];
  SeqView::View view(data, 100);
  uint64_t num = 0;
  for (auto& elem : view) elem = num++;
  EXPECT_THAT(view, ElementsAreArray(data, 100));
}

TEST(View, Make1DSubView) {
  uint64_t data[100];
  SeqView::View view(data, 100);
  auto sub = view(SeqView::Range{10, 20, 2});
  EXPECT_THAT(sub, SizeIs(5));
}

TEST(View, IterateWithStep) {
  uint64_t data[100];
  init_range(data, 100);
  SeqView::View view(data, 100);
  auto sub = view(SeqView::Range{10, 20, 2});
  EXPECT_THAT(sub, SizeIs(5));
  for (uint64_t idx = 10, view_idx = 0; idx < 20; idx += 2, ++view_idx) {
    EXPECT_THAT(sub[view_idx], Eq(data[idx]));
  }
}

TEST(View, IterateWithNegativeStep) {
  uint64_t data[100];
  init_range(data, 100);
  SeqView::View view(data, 100);
  auto sub = view(SeqView::Range{10, 20, -2});
  EXPECT_THAT(sub, SizeIs(5));
  for (uint64_t idx = 18, view_idx = 0; idx > 8; idx -= 2, ++view_idx) {
    EXPECT_THAT(sub[view_idx], Eq(data[idx]));
  }
}

TEST(View, ForEachLoop) {
  uint64_t data[100];
  init_range(data, 100);
  SeqView::View view(data, 100);
  auto sub = view(SeqView::Range{10, 20, 2});
  EXPECT_THAT(sub, SizeIs(5));
  int64_t idx = 10;
  for (auto& elem : sub) {
    EXPECT_THAT(elem, Eq(data[idx]));
    idx += 2;
  }
}

TEST(View, ForEachLoopReversed) {
  uint64_t data[100];
  init_range(data, 100);
  SeqView::View view(data, 100);
  auto sub = view(SeqView::Range{10, 20, -2});
  EXPECT_THAT(sub, SizeIs(5));
  int64_t idx = 18;
  for (auto& elem : sub) {
    EXPECT_THAT(elem, Eq(data[idx]));
    idx -= 2;
  }
}

TEST(View, AssignSingleValue) {
  uint64_t array[] = {13, 22, 18, 44, 15};
  SeqView::View view(array, 5);
  view = 13;
  for (auto& elem : view) {
    EXPECT_THAT(elem, Eq(13));
  }
}

TEST(View, CascadeAssignSingleValue) {
  uint64_t array[] = {13, 22, 18, 44, 15};
  uint64_t other[] = {13, 22, 18, 44, 15};
  SeqView::View view(array, 5);
  SeqView::View view_other(other, 5);
  view_other = view = 13;
  for (auto& elem : view) {
    EXPECT_THAT(elem, Eq(13));
  }
  for (auto& elem : view_other) {
    EXPECT_THAT(elem, Eq(13));
  }
}
