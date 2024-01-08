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

TEST(SubView, MakeMaskFromComparison) {
  uint64_t data[] = {1, 3, 7, 5, 4, 2, 1, 3, 6, 8};
  uint8_t expected[] = {true, true, false, false, true,
                        true, true, true,  false, false};

  SeqView::View view(data, 10);
  auto mask = view < 5;

  for (auto idx = 0U; idx < mask.size(); ++idx)
    EXPECT_THAT(mask[idx], Eq(expected[idx]))
        << fmt::format("Failed for {}/{}", idx, data[idx]);
}

TEST(SubView, CheckValidUntil) {
  uint64_t data[] = {1, 3, 7, 5, 4, 2, 1, 3, 6, 8};
  std::vector<uint64_t> expected{0, 1, 2, 2, 2, 3, 4, 5, 6, 6, 6};

  SeqView::View view(data, 10);
  auto mask = view < 5;
  SeqView::MaskInfo info(mask);
  EXPECT_THAT(info.valid_until, ContainerEq(expected));
}

TEST(SubView, SubViewWithMask) {
  uint64_t data[] = {1, 3, 7, 5, 4, 2, 1, 3, 6, 8};
  uint64_t expected[] = {1, 3, 4, 2, 1, 3};
  std::vector<uint8_t> mask{true, true, false, false, true,
                            true, true, true,  false, false};
  SeqView::View view(data, 10);
  auto sub = view(mask);
  EXPECT_THAT(sub.size(), Eq(6));
  for (auto idx = 0U; idx < sub.size(); ++idx)
    EXPECT_THAT(sub[idx], Eq(expected[idx]))
        << fmt::format("Failed for idx {}", idx);
}

TEST(SubView, SubViewWithIteratorsAndMask) {
  using namespace SeqView;
  uint64_t data[] = {1, 3, 7, 5, 4, 2, 1, 3, 6, 8};
  uint64_t expected[] = {1, 3, 4, 2, 1, 3};
  std::vector<uint8_t> mask{true, true, false, false, true,
                            true, true, true,  false, false};
  SeqView::View view(data, 10);
  auto sub = view(mask);
  EXPECT_THAT(sub.size(), Eq(6));
  EXPECT_THAT(sub.end() - sub.begin(), Eq(6));
  uint64_t idx = 0;
  for (auto it = sub.begin(); it != sub.end(); ++it, ++idx) {
    EXPECT_THAT(*it, Eq(expected[idx]))
        << fmt::format("Failed for idx {}", idx);
  }
}