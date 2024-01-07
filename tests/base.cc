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
  int array[2];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 2);
  EXPECT_THAT(begin, Ne(end));
  EXPECT_THAT(begin, Lt(end));
}

TEST(Iterator, PostIncrement) {
  int array[2];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 2);
  auto it = begin;
  auto other = it++;
  EXPECT_THAT(other, Eq(begin));
  EXPECT_THAT(it, Ne(begin));
  EXPECT_THAT(it, Gt(begin));
  EXPECT_THAT(it, Ne(end));
  EXPECT_THAT(it, Lt(end));

  it++;

  EXPECT_THAT(it, Ne(begin));
  EXPECT_THAT(it, Gt(begin));
  EXPECT_THAT(it, Eq(end));
}

TEST(Iterator, PreIncrement) {
  int array[2];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 2);
  auto it = begin;
  auto other = ++it;
  EXPECT_THAT(other, Eq(it));
  EXPECT_THAT(it, Ne(begin));
  EXPECT_THAT(it, Gt(begin));
  EXPECT_THAT(it, Ne(end));
  EXPECT_THAT(it, Lt(end));

  ++it;

  EXPECT_THAT(it, Ne(begin));
  EXPECT_THAT(it, Gt(begin));
  EXPECT_THAT(it, Eq(end));
}

TEST(Iterator, PostDecrement) {
  int array[2];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 2);
  auto it = end;
  auto other = it--;
  EXPECT_THAT(other, Eq(end));
  EXPECT_THAT(it, Ne(end));
  EXPECT_THAT(it, Lt(end));
  EXPECT_THAT(it, Ne(begin));
  EXPECT_THAT(it, Gt(begin));

  it--;

  EXPECT_THAT(it, Ne(end));
  EXPECT_THAT(it, Lt(end));
  EXPECT_THAT(it, Eq(begin));
}

TEST(Iterator, PreDecrement) {
  int array[2];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 2);
  auto it = end;
  auto other = --it;
  EXPECT_THAT(other, Eq(it));
  EXPECT_THAT(it, Ne(end));
  EXPECT_THAT(it, Lt(end));
  EXPECT_THAT(it, Ne(begin));
  EXPECT_THAT(it, Gt(begin));

  --it;

  EXPECT_THAT(it, Ne(end));
  EXPECT_THAT(it, Lt(end));
  EXPECT_THAT(it, Eq(begin));
}
