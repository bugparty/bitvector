#include "bitvector.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>

// Benchmarks the performance of std::vector<bool> for setting, accessing, and traversing elements.
void benchmarkStdVectorBool(){
    constexpr size_t SIZE = 10000000;  // 10 million elements

    // Define a vector<bool>
    std::vector<bool> bool_vector(SIZE);

    // Time setting each element to true
    auto start_set = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        bool_vector[i] = i&0x1;
    }
    auto end_set = std::chrono::high_resolution_clock::now();
    auto duration_set = std::chrono::duration_cast<std::chrono::milliseconds>(end_set - start_set);
    std::cout << "Setting all elements took " << duration_set.count() << " milliseconds.\\n";

    // Time accessing each element
    size_t true_count = 0;
    auto start_access = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        if (bool_vector[i]) {
            ++true_count;
        }
    }
    auto end_access = std::chrono::high_resolution_clock::now();
    auto duration_access = std::chrono::duration_cast<std::chrono::milliseconds>(end_access - start_access);
    std::cout << "Accessing all elements took " << duration_access.count() << " milliseconds.\\n";

    // Time traversing the entire vector
    auto start_traverse = std::chrono::high_resolution_clock::now();
    for (auto it = bool_vector.begin(); it != bool_vector.end(); ++it) {
        if (*it) {
            ++true_count;
        }
    }
    auto end_traverse = std::chrono::high_resolution_clock::now();
    auto duration_traverse = std::chrono::duration_cast<std::chrono::milliseconds>(end_traverse - start_traverse);
    std::cout << "Traversing all elements took " << duration_traverse.count() << " milliseconds.\\n";

    std::cout << "True count: " << true_count << "\\\\n";

}

// Benchmarks the performance of bowen::bitvector for setting, assigning, accessing, and traversing elements.
void benchmarkBowenBitvector(){
    constexpr size_t SIZE = 10000000;  // 10 million elements

    // Define a bitvector
    bowen::bitvector bool_vector(SIZE);

    // Time setting each element
    auto start_set = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        bool_vector[i] = i&0x1;
    }
    auto end_set = std::chrono::high_resolution_clock::now();
    auto duration_set = std::chrono::duration_cast<std::chrono::milliseconds>(end_set - start_set);
    std::cout << "Setting all elements took " << duration_set.count() << " milliseconds.\\n";

        // Time setting each element using assign
    auto start_assign = std::chrono::high_resolution_clock::now();
    bowen::bitvector vector2;
    vector2.assign(SIZE, 1);
    auto end_assign = std::chrono::high_resolution_clock::now();
    auto duration_assign = std::chrono::duration_cast<std::chrono::milliseconds>(end_assign - start_assign);
    std::cout << "assign all elements took " << duration_assign.count() << " milliseconds.\\n";
    // Time accessing each element
    size_t true_count = 0;
    auto start_access = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        if (bool_vector[i]) {
            ++true_count;
        }
    }
    auto end_access = std::chrono::high_resolution_clock::now();
    auto duration_access = std::chrono::duration_cast<std::chrono::milliseconds>(end_access - start_access);
    std::cout << "Accessing all elements took " << duration_access.count() << " milliseconds.\\n";

    // Time traversing the entire vector
    auto start_traverse = std::chrono::high_resolution_clock::now();
    for (auto it = bool_vector.begin(); it != bool_vector.end(); ++it) {
        if (*it) {
            ++true_count;
        }
    }
    auto end_traverse = std::chrono::high_resolution_clock::now();
    auto duration_traverse = std::chrono::duration_cast<std::chrono::milliseconds>(end_traverse - start_traverse);
    std::cout << "Traversing all elements took " << duration_traverse.count() << " milliseconds.\\n";

    std::cout << "True count: " << true_count << "\\n";
}

// Compares the behavior of bowen::bitvector against std::vector<bool> for basic operations.
void testBitvectorAgainstStdVectorBool(){
    constexpr size_t SIZE = 10000000;  // 10 million elements

    // Define a bitvector and a vector<bool>
    bowen::bitvector bool_vector1(SIZE);
    std::vector<bool> bool_vector2(SIZE);
    // Time setting each element
    auto start_set = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        bool_vector1[i] = i&0x1;
        bool_vector2[i] = i&0x1;
    }
    auto end_set = std::chrono::high_resolution_clock::now();
    auto duration_set = std::chrono::duration_cast<std::chrono::milliseconds>(end_set - start_set);
    std::cout << "Setting all elements took " << duration_set.count() << " milliseconds.\\n";
    assert(bool_vector1.size()==bool_vector2.size());
    assert(bool_vector1.empty() == bool_vector2.empty());
    // Time accessing each element and comparing
    size_t false_count = 0;
    auto start_access = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        if (bool_vector1[i] != bool_vector2[i]) {
            ++false_count;
        }
    }
    auto end_access = std::chrono::high_resolution_clock::now();
    auto duration_access = std::chrono::duration_cast<std::chrono::milliseconds>(end_access - start_access);
    std::cout << "Accessing all elements took " << duration_access.count() << " milliseconds.\\n";
    std::cout << "found "<< false_count<< " error in bitvector" <<std::endl;


}

// Tests the incrementUntilZero method of bowen::bitvector.
// This method is expected to increment a counter starting from a given position
// as long as it encounters '1's in the bitvector, stopping at the first '0' or the end of the vector.
void testBitvectorIncrementUntilZero(){
    constexpr size_t SIZE = 128*10;  // Size of the bitvector

    // Define a bitvector
    bowen::bitvector bool_vector1(SIZE);
    // Time setting some elements to true
    int start = rand()%(SIZE-2);
    int correct_count=start;
    int end = start+ rand()%(SIZE-start);
    for (size_t i = start; i < end; ++i) {
        bool_vector1[i] = 1;
        correct_count+=1;
    }
    size_t count=start;

    bool_vector1.incrementUntilZero(count);
    int index = start;
    while (index < bool_vector1.size() && bool_vector1[index] == 1 )
                index++;
    if(count != index){
        printf("start:%d end:%d expect:%zd val:%d\n", start, end, count, correct_count);
    }

    assert(count == correct_count);
}

void runAllTests() {
    testBitvectorAgainstStdVectorBool();
    testBitvectorIncrementUntilZero();
}

int main() {
    benchmarkStdVectorBool();
    benchmarkBowenBitvector();
    runAllTests();
    return 0;
}
