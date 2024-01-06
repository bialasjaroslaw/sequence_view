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

TEST(Range, MakeRange) {
  SeqView::Range rng(12, 23, 2);
  EXPECT_THAT(rng.start(), Eq(12));
  EXPECT_THAT(rng.stop(), Eq(23));
  EXPECT_THAT(rng.step(), Eq(2));
}

TEST(Range, MakeRangeReversedArgs) {
  SeqView::Range rng(23, 12, 2);
  EXPECT_THAT(rng.start(), Eq(12));
  EXPECT_THAT(rng.stop(), Eq(23));
  EXPECT_THAT(rng.step(), Eq(2));
}

TEST(Range, MakeRangeDefaultStep) {
  SeqView::Range rng(12, 23);
  EXPECT_THAT(rng.start(), Eq(12));
  EXPECT_THAT(rng.stop(), Eq(23));
  EXPECT_THAT(rng.step(), Eq(SeqView::STEP));
}

TEST(Range, MakeStartRange) {
  SeqView::StartRange rng(12, 2);
  EXPECT_THAT(rng.start(), Eq(12));
  EXPECT_THAT(rng.stop(), Eq(SeqView::END));
  EXPECT_THAT(rng.step(), Eq(2));
}

TEST(Range, MakeStartRangeDefaultStep) {
  SeqView::StartRange rng(12);
  EXPECT_THAT(rng.start(), Eq(12));
  EXPECT_THAT(rng.stop(), Eq(SeqView::END));
  EXPECT_THAT(rng.step(), Eq(SeqView::STEP));
}

TEST(Range, MakeStopRange) {
  SeqView::StopRange rng(12, 2);
  EXPECT_THAT(rng.start(), Eq(SeqView::START));
  EXPECT_THAT(rng.stop(), Eq(12));
  EXPECT_THAT(rng.step(), Eq(2));
}

TEST(Range, MakeStopRangeDefaultStep) {
  SeqView::StopRange rng(12);
  EXPECT_THAT(rng.start(), Eq(SeqView::START));
  EXPECT_THAT(rng.stop(), Eq(12));
  EXPECT_THAT(rng.step(), Eq(SeqView::STEP));
}

TEST(Range, MakeStepRange) {
  SeqView::StepRange rng(2);
  EXPECT_THAT(rng.start(), Eq(SeqView::START));
  EXPECT_THAT(rng.stop(), Eq(SeqView::END));
  EXPECT_THAT(rng.step(), Eq(2));
}
