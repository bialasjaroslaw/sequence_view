#include <sequence_view/common.hpp>

#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include <numeric>

using namespace ::testing;

void init_range(uint64_t* ptr, uint64_t len){
    std::iota(ptr, ptr + len, 0);
}

TEST(Common, MakeRange)
{
    SeqView::Range rng(12, 23, 2);
    EXPECT_THAT(rng.start, Eq(12));
    EXPECT_THAT(rng.stop, Eq(23));
    EXPECT_THAT(rng.step, Eq(2));
}

TEST(Common, MakeRangeDefaultStep)
{
    SeqView::Range rng(12, 23);
    EXPECT_THAT(rng.start, Eq(12));
    EXPECT_THAT(rng.stop, Eq(23));
    EXPECT_THAT(rng.step, Eq(SeqView::STEP));
}

TEST(Common, MakeStartRange)
{
    SeqView::StartRange rng(12, 2);
    EXPECT_THAT(rng.start, Eq(12));
    EXPECT_THAT(rng.stop, Eq(SeqView::END));
    EXPECT_THAT(rng.step, Eq(2));
}

TEST(Common, MakeStartRangeDefaultStep)
{
    SeqView::StartRange rng(12);
    EXPECT_THAT(rng.start, Eq(12));
    EXPECT_THAT(rng.stop, Eq(SeqView::END));
    EXPECT_THAT(rng.step, Eq(SeqView::STEP));
}

TEST(Common, MakeStopRange)
{
    SeqView::StopRange rng(12, 2);
    EXPECT_THAT(rng.start, Eq(SeqView::START));
    EXPECT_THAT(rng.stop, Eq(12));
    EXPECT_THAT(rng.step, Eq(2));
}

TEST(Common, MakeStopRangeDefaultStep)
{
    SeqView::StopRange rng(12);
    EXPECT_THAT(rng.start, Eq(SeqView::START));
    EXPECT_THAT(rng.stop, Eq(12));
    EXPECT_THAT(rng.step, Eq(SeqView::STEP));
}

TEST(Common, MakeStepRange)
{
    SeqView::StepRange rng(2);
    EXPECT_THAT(rng.start, Eq(SeqView::START));
    EXPECT_THAT(rng.stop, Eq(SeqView::END));
    EXPECT_THAT(rng.step, Eq(2));
}

TEST(Common, Make1DView)
{
    uint64_t data[100];
    SeqView::View view(data, 100);
    EXPECT_THAT(view, SizeIs(100));
}

TEST(Common, IterateAssignOver1DView)
{
    uint64_t data[100];
    SeqView::View view(data, 100);
    uint64_t num = 0;
    for(auto& elem : view)
        elem = num++;
    EXPECT_THAT(view, ElementsAreArray(data, 100));
}

TEST(Common, Make1DSubView)
{
    uint64_t data[100];
    SeqView::View view(data, 100);
    auto sub = view(SeqView::Range{10, 20, 2});
    EXPECT_THAT(sub, SizeIs(5));
}

TEST(Common, IterateWithStep)
{
    uint64_t data[100];
    init_range(data, 100);
    SeqView::View view(data, 100);
    auto sub = view(SeqView::Range{10, 20, 2});
    EXPECT_THAT(sub, SizeIs(5));
    for(int64_t idx = 10, view_idx = 0 ; idx < 20 ; idx += 2, ++view_idx){
        EXPECT_THAT(sub[view_idx], Eq(data[idx]));
    }
}

// TODO implement iterator first
// TEST(Common, IterateWithNegativeStep)
// {
//     uint64_t data[100];
//     init_range(data, 100);
//     SeqView::View view(data, 100);
//     auto sub = view(SeqView::Range{10, 20, -2});
//     EXPECT_THAT(sub, SizeIs(5));
//     for(int64_t idx = 10, view_idx = 0 ; idx < 20 ; idx += 2, ++view_idx){
//         EXPECT_THAT(sub[view_idx], Eq(data[idx]));
//     }
// }