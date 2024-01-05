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

TEST(StlAlgo, Sort) {
  uint64_t data[] = {3, 6, 1, 5, 4, 3, 5, 1};
  uint64_t expected[] = {5, 6, 4, 5, 3, 3, 1, 1};
  EXPECT_FALSE(std::is_sorted(std::begin(data), std::end(data)));
  SeqView::View view(data, 8);
  {
    auto sub = view(SeqView::Range{1, 8, -2});
    EXPECT_TRUE(std::is_sorted(std::begin(sub), std::end(sub)));
  }
  {
    auto sub = view(SeqView::Range{0, 8, -2});
    EXPECT_FALSE(std::is_sorted(std::begin(sub), std::end(sub)));
    std::sort(std::begin(sub), std::end(sub));
    for (uint64_t idx = 0; idx < 8; ++idx) {
      EXPECT_THAT(data[idx], Eq(expected[idx]));
    }
  }
}

TEST(StlAlgo, MaxElem) {
  uint64_t data[] = {3, 6, 1, 5, 4, 3, 5, 1};
  SeqView::View view(data, 8);
  auto sub = view(SeqView::Range{0, 8, -2});
  EXPECT_THAT(*std::max_element(std::begin(sub), std::end(sub)), Eq(5));
}

TEST(StlAlgo, AllOfAnyOfNoneOf) {
  uint64_t data[] = {3, 6, 1, 4, 5, 2, 5, 0};
  SeqView::View view(data, 8);
  {
    auto sub = view(SeqView::Range{0, 8, 2});
    EXPECT_TRUE(std::all_of(std::begin(sub), std::end(sub),
                            [](auto elem) { return elem % 2 == 1; }));
  }
  {
    auto sub = view(SeqView::Range{0, 8, 2});
    EXPECT_FALSE(std::any_of(std::begin(sub), std::end(sub),
                             [](auto elem) { return elem == 6; }));
  }
  {
    auto sub = view(SeqView::Range{0, 8, 2});
    EXPECT_TRUE(std::none_of(std::begin(sub), std::end(sub),
                             [](auto elem) { return elem == 6; }));
  }
}

TEST(StlAlgo, StablePartition) {
  uint64_t data[] = {3, 6, 1, 5, 4, 3, 5, 1};
  uint64_t expected[] = {3, 3, 1, 1, 4, 6, 5, 5};
  SeqView::View view(data, 8);
  auto sub = view(SeqView::Range{1, 8, 2});
  std::stable_partition(std::begin(sub), std::end(sub),
                        [](auto elem) { return elem < 4; });

  for (int64_t idx = 0; idx < 8; ++idx) {
    EXPECT_THAT(data[idx], Eq(expected[idx]));
  }
}

TEST(StlAlgo, Find) {
  uint64_t data[] = {3, 6, 1, 5, 4, 3, 5, 1};
  SeqView::View view(data, 8);
  auto sub = view(SeqView::Range{0, 8, 2});
  auto rev_sub = view(SeqView::Range{1, 8, -2});
  EXPECT_THAT(std::find(sub.begin(), sub.end(), 5).base() - sub.begin().base(),
              Eq(6));
  EXPECT_THAT(std::find(rev_sub.begin(), rev_sub.end(), 5).base() -
                  rev_sub.end().base(),
              Eq(4));
}

TEST(StlAlgo, CheckStlCount) {
  uint64_t data[] = {3, 6, 1, 5, 4, 3, 5, 1};
  SeqView::View view(data, 8);
  auto sub = view(SeqView::Range{0, 8, 2});
  EXPECT_THAT(std::count(sub.begin(), sub.end(), 3), Eq(1));
}
