#include <gmock/gmock-more-matchers.h>
#include <gtest/gtest-matchers.h>
#include <gtest/gtest.h>

#include <sequence_view/base-iterator.hpp>
#include <sequence_view/mask.hpp>
#include <sequence_view/range.hpp>
#include <sequence_view/view.hpp>

using namespace ::testing;

TEST(Iterator, DefaultCtor) {
  EXPECT_THAT(SeqView::BaseIterator<int>(), Eq(SeqView::BaseIterator<int>()));
}

TEST(Iterator, ComparisonChecksOfTwoDifferentIterators) {
  int array[4];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 4);
  EXPECT_THAT(begin, Ne(end));
  EXPECT_THAT(begin, Lt(end));
}