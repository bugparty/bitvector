#include "bitvector.hpp"
#include <gtest/gtest.h>

TEST(BitvectorTest, PushBackBasic) {
    bowen::bitvector<> bv;
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
    bowen::bitvector<> bv(N);

    // Set bits 5 through 7 to 1
    for (size_t i = 5; i <= 7; ++i) {
        bv.set_bit(i, true);
    }

    size_t pos = 5;
    bv.incrementUntilZero(pos);
    EXPECT_EQ(pos, 8u);
}
