#include "bitvector.hpp"
#include <gtest/gtest.h>

TEST(BitvectorTest, PushBackBasic) {
    bowen::BitVector<> bv;
    EXPECT_TRUE(bv.empty());

    bv.push_back(false);
    bv.push_back(true);
    bv.push_back(true);

    ASSERT_EQ(bv.size(), 3u);
    EXPECT_FALSE(bv[0]);
    EXPECT_TRUE(bv[1]);
    EXPECT_TRUE(bv[2]);
}

TEST(BitvectorTest, IncrementUntilZero) {
    const size_t N = 20;
    bowen::BitVector<> bv(N);

    // Set bits 5 through 7 to 1
    for (size_t i = 5; i <= 7; ++i) {
        bv.set_bit(i, true);
    }

    size_t pos = 5;
    bv.incrementUntilZero(pos);
    EXPECT_EQ(pos, 8u);
}

TEST(BitvectorTest, ConstructWithValue) {
    bowen::BitVector<> bv(10, true);
    ASSERT_EQ(bv.size(), 10u);
    for (size_t i = 0; i < bv.size(); ++i) {
        EXPECT_TRUE(bv[i]);
    }
}

#ifndef BITVECTOR_NO_BOUND_CHECK
TEST(BitvectorTest, OutOfRangeThrows) {
    bowen::BitVector<> bv(5);
    EXPECT_THROW(bv[5], std::out_of_range);
    EXPECT_THROW(bv.set_bit(5, true), std::out_of_range);
}
#endif

TEST(BitvectorTest, CopyAndAssignment) {
    bowen::BitVector<> bv1;
    bv1.push_back(false);
    bv1.push_back(true);

    bowen::BitVector<> bv2(bv1);
    ASSERT_EQ(bv2.size(), bv1.size());
    EXPECT_FALSE(bv2[0]);
    EXPECT_TRUE(bv2[1]);

    bv1.set_bit(0, true);
    EXPECT_TRUE(bv1[0]);
    EXPECT_FALSE(bv2[0]);

    bowen::BitVector<> bv3;
    bv3 = bv1;
    ASSERT_EQ(bv3.size(), bv1.size());
    EXPECT_TRUE(bv3[0]);
    EXPECT_TRUE(bv3[1]);
}

TEST(BitvectorTest, AssignResizesAndSets) {
    bowen::BitVector<> bv;
    bv.assign(12, true);
    ASSERT_EQ(bv.size(), 12u);
    for (size_t i = 0; i < bv.size(); ++i)
        EXPECT_TRUE(bv[i]);

    bv.assign(4, false);
    ASSERT_EQ(bv.size(), 4u);
    for (size_t i = 0; i < bv.size(); ++i)
        EXPECT_FALSE(bv[i]);
}

TEST(BitvectorTest, IteratorTraversal) {
    bowen::BitVector<> bv;
    bv.push_back(true);
    bv.push_back(false);
    bv.push_back(true);

    std::vector<bool> values;
    auto it = bv.begin();
    for (size_t i = 0; i < bv.size(); ++i, ++it) {
        values.push_back(*it);
    }
    ASSERT_EQ(values.size(), 3u);
    EXPECT_TRUE(values[0]);
    EXPECT_FALSE(values[1]);
    EXPECT_TRUE(values[2]);
}
