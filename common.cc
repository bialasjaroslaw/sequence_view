#include <sequence_view/common.hpp>

#include <gtest/gtest.h>
#include <gtest/gtest-matchers.h>
#include <gmock/gmock-more-matchers.h>

#include <numeric>
#include <fmt/format.h>

using namespace ::testing;

void init_range(uint64_t* ptr, uint64_t len){
    std::iota(ptr, ptr + len, 0);
}

TEST(ViewHelpers, ValidateStep)
{
    std::vector<int64_t> input{-2, -1, 0, 1, 2};
    std::vector<int64_t> expected{-2, -1, 1, 1, 2};

    for(auto idx = 0 ; idx < input.size() ; ++idx)
        EXPECT_THAT(SeqView::View<uint64_t>::validate_step(input[idx]), Eq(expected[idx])) << fmt::format("Failed for {}", input[idx]);
}

TEST(ViewHelpers, Elements)
{
    std::vector<uint64_t> input_size{0, 0, 1, 1, 9, 10, 11, 12, 13, 9, 10, 11, 12,  9,  9, 10, 11, 12};
    std::vector<int64_t>  input_step{0, 1, 1, 2, 3,  3,  3,  3,  3, 2,  2,  2,  2, -2, -3, -3, -3, -3};
    std::vector<int64_t>    expected{0, 0, 1, 1, 3,  4,  4,  4,  5, 5,  5,  6,  6,  5,  3,  4,  4,  4};

    for(auto idx = 0 ; idx < input_size.size() ; ++idx)
        EXPECT_THAT(SeqView::View<uint64_t>::elements(input_size[idx], input_step[idx]), Eq(expected[idx])) << fmt::format("Failed for {}/{}", input_size[idx], input_step[idx]);
}

TEST(ViewHelpers, LastPtr)
{
    uint16_t data = 0;
    auto ptr = &data;
    auto PC = [=](uint16_t value){
        return ptr + value;
    };
    std::vector<uint64_t> input_size{    0,     0,        1,      1,      9,    10, 
                                        11,    12,       13,      9,     10,    11, 
                                        12,     9,        9,     10,     11,    12};
    std::vector<int64_t>  input_step{    0,     1,        1,      2,      3,     3,  
                                         3,     3,        3,      2,      2,     2,  
                                         2,    -2,       -3,     -3,     -3,    -3};
                                         // expected is smallest multiple of step larger or equal size
    std::vector<uint16_t*>  expected{PC(0),   PC(0),  PC(1),  PC(2),  PC(9), PC(12),
                                     PC(12), PC(12), PC(15), PC(10), PC(10), PC(12),
                                     PC(12), PC(10),  PC(9), PC(12), PC(12), PC(12)};

    for(auto idx = 0 ; idx < input_size.size() ; ++idx){
        EXPECT_THAT(SeqView::View<uint16_t>::last_ptr(ptr, input_size[idx], input_step[idx]), Eq(expected[idx])) << fmt::format("Failed for {}/{}/{}", fmt::ptr(ptr), input_size[idx], input_step[idx]);
    }
}

TEST(ViewHelpers, BasePtr)
{
    uint16_t data = 0;
    auto ptr = &data;
    auto PC = [=](uint16_t value){
        return ptr + value;
    };
    std::vector<uint64_t> input_size{    0,     0,        1,      1,      9,    10, 
                                        11,    12,       13,      9,     10,    11, 
                                        12,     9,        9,     10,     11,    12};
    std::vector<int64_t>  input_step{    0,     1,        1,      2,      3,     3,  
                                         3,     3,        3,      2,      2,     2,  
                                         2,    -2,       -3,     -3,     -3,    -3};
                                         // expected is smallest multiple of step larger or equal size
    std::vector<uint16_t*>  expected{PC(0),   PC(0),  PC(0),  PC(0),  PC(0), PC(0),
                                     PC(0),   PC(0),  PC(0),  PC(0),  PC(0), PC(0),
                                     PC(0),   PC(10), PC(9), PC(12), PC(12), PC(12)};

    for(auto idx = 0 ; idx < input_size.size() ; ++idx){
        EXPECT_THAT(SeqView::View<uint16_t>::base_ptr(ptr, input_size[idx], input_step[idx]), Eq(expected[idx])) << fmt::format("Failed for {}/{}/{}", fmt::ptr(ptr), input_size[idx], input_step[idx]);
    }
}

TEST(ViewHelpers, EndPtr)
{
    uint16_t data = 0;
    auto ptr = &data;
    auto PC = [=](uint16_t value){
        return ptr + value;
    };
    std::vector<uint64_t> input_size{    0,     0,        1,      1,      9,    10, 
                                        11,    12,       13,      9,     10,    11, 
                                        12,     9,        9,     10,     11,    12};
    std::vector<int64_t>  input_step{    0,     1,        1,      2,      3,     3,  
                                         3,     3,        3,      2,      2,     2,  
                                         2,    -2,       -3,     -3,     -3,    -3};
                                         // expected is smallest multiple of step larger or equal size
    std::vector<uint16_t*>  expected{PC(0),   PC(0),  PC(1),  PC(2),  PC(9), PC(12),
                                     PC(12), PC(12), PC(15), PC(10), PC(10), PC(12),
                                     PC(12),  PC(0),  PC(0),  PC(0),  PC(0),  PC(0)};

    for(auto idx = 0 ; idx < input_size.size() ; ++idx){
        EXPECT_THAT(SeqView::View<uint16_t>::end_ptr(ptr, input_size[idx], input_step[idx]), Eq(expected[idx])) << fmt::format("Failed for {}/{}/{}", fmt::ptr(ptr), input_size[idx], input_step[idx]);
    }
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

TEST(Common, IterateWithNegativeStep)
{
    uint64_t data[100];
    init_range(data, 100);
    SeqView::View view(data, 100);
    auto sub = view(SeqView::Range{10, 20, -2});
    EXPECT_THAT(sub, SizeIs(5));
    for(int64_t idx = 18, view_idx = 0 ; idx > 8 ; idx -= 2, ++view_idx){
        EXPECT_THAT(sub[view_idx], Eq(data[idx]));
    }
}