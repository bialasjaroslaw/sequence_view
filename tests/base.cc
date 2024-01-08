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
  EXPECT_THAT(begin, Le(end));
  EXPECT_THAT(end, Ge(begin));
  EXPECT_THAT(end, Gt(begin));
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

TEST(Iterator, PlusEqual) {
  int array[2];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 2);

  auto it = begin;
  it += 1;
  EXPECT_THAT(it, Lt(end));
  it = begin;
  it += 2;
  EXPECT_THAT(it, Eq(end));
  it = begin;
  it += 3;
  EXPECT_THAT(it, Gt(end));
}

TEST(Iterator, MinusEqual) {
  int array[2];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 2);

  auto it = end;
  it -= 1;
  EXPECT_THAT(it, Gt(begin));
  it = end;
  it -= 2;
  EXPECT_THAT(it, Eq(begin));
  it = end;
  it -= 3;
  EXPECT_THAT(it, Lt(begin));
}

TEST(Iterator, PlusCopy) {
  int array[2];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 2);

  auto it = begin + 2;
  EXPECT_THAT(it, Eq(end));
}

TEST(Iterator, ValuePlusCopy) {
  int array[2];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 2);

  auto it = 2 + begin;
  EXPECT_THAT(it, Eq(end));
}

TEST(Iterator, MinusCopy) {
  int array[2];
  auto begin = SeqView::BaseIterator(array);
  auto end = SeqView::BaseIterator(array + 2);

  auto it = end - 2;
  EXPECT_THAT(it, Eq(begin));
}

TEST(Iterator, BracketOperator) {
  int array[] = {13, 22, 18, 44, 15};
  auto it = SeqView::BaseIterator(array);
  it += 2;
  for (int idx = 0; idx < 2; ++idx) EXPECT_THAT(it[idx], Eq(array[idx + 2]));
}

TEST(Iterator, WithMaskForward) {
  int array[] = {13, 22, 18, 44, 15};
  std::vector<uint8_t> mask{true, true, false, false, true};
  auto begin = SeqView::BaseIterator(array, SeqView::MASK, mask);
  auto end = begin + 3;
  auto it = begin;
  EXPECT_THAT(it, Eq(begin));
  EXPECT_THAT(*it++, Eq(13));
  EXPECT_THAT(*it++, Eq(22));
  EXPECT_THAT(*it++, Eq(15));
  EXPECT_THAT(it, Eq(end));
}

TEST(Iterator, WithMaskBackward) {
  int array[] = {13, 22, 18, 44, 15};
  std::vector<uint8_t> mask{true, true, false, false, true};
  auto begin = SeqView::BaseIterator(array, SeqView::MASK, mask);
  auto end = begin + 3;
  auto it = end;
  EXPECT_THAT(it, Eq(end));
  EXPECT_THAT(*--it, Eq(15));
  EXPECT_THAT(*--it, Eq(22));
  EXPECT_THAT(*--it, Eq(13));
  EXPECT_THAT(it, Eq(begin));
}