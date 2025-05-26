//
// Created by fancy on 26/11/2024.
//
#include "gtest/gtest.h"
#include "calculatePrimes.hpp"  // Assuming Counter class is in counter.h

// Test the basic functionality of the Counter class
TEST(CounterTest, TestInitialization) {
    // Create a Counter from 0 to 5
    PrimerCalculator primerCalculator(99, 100);

    // Check that the iterator begins at 0 and ends at 5
    auto it = primerCalculator.begin();
    EXPECT_EQ(*it, 3);  // The first value should be 0
    ++it;
    EXPECT_EQ(*it, 5);  // The second value should be 1
    ++it;
    EXPECT_EQ(*it, 7);  // The third value should be 2
    ++it;
    EXPECT_EQ(*it, 11);  // The fourth value should be 3
    ++it;
    EXPECT_EQ(it,primerCalculator.end());  // The fifth value should be 4
    ++it;
    EXPECT_EQ(it, primerCalculator.end());  // The sixth value should be 5

}

TEST(CounterTest, TestEmptyRange) {
    // Test when the start and end are the same
    PrimerCalculator primerCalculator(4, 17);

    // Iterator should only produce one element: 5
    auto it = primerCalculator.begin();
    EXPECT_EQ(*it, 0);
    EXPECT_EQ(it, primerCalculator.end());  // It should be at the end
}



TEST(CounterTest, TestIteratorComparison) {
    PrimerCalculator primerCalculator(99, 100);

    auto it1 = primerCalculator.begin();
    auto it2 = primerCalculator.begin();
    EXPECT_TRUE(it1 == it2);  // They should be equal initially
    ++it2;
    EXPECT_TRUE(it1 != it2);  // They should not be equal after incrementing one iterator

    auto it3 = primerCalculator.end();
    EXPECT_TRUE(it3 != it2);  // Iterator at the end should not be equal to the second iterator
}

