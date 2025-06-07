#include "bitvector.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>
constexpr size_t SIZE = 1000000000;  // 1 billion elements
// Benchmarks the performance of std::vector<bool> for setting, accessing, and traversing elements.
void benchmarkStdVectorBool(){

    std::cout << "Benchmarking std::vector<bool>..." << std::endl;

    // Define a vector<bool>
    std::vector<bool> bool_vector(SIZE);

    // Time setting each element to true
    auto start_set = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        bool_vector[i] = i & 0x1;
    }
    auto end_set = std::chrono::high_resolution_clock::now();
    auto duration_set = std::chrono::duration_cast<std::chrono::milliseconds>(end_set - start_set);
    std::cout << "[std::vector<bool>] Setting all elements took " << duration_set.count() << " milliseconds." << std::endl;

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
    std::cout << "[std::vector<bool>] Accessing all elements took " << duration_access.count() << " milliseconds." << std::endl;

    // Time traversing the entire vector
    auto start_traverse = std::chrono::high_resolution_clock::now();
    for (auto it = bool_vector.begin(); it != bool_vector.end(); ++it) {
        if (*it) {
            ++true_count;
        }
    }
    auto end_traverse = std::chrono::high_resolution_clock::now();
    auto duration_traverse = std::chrono::duration_cast<std::chrono::milliseconds>(end_traverse - start_traverse);
    std::cout << "[std::vector<bool>] Traversing all elements took " << duration_traverse.count() << " milliseconds." << std::endl;

    std::cout << "[std::vector<bool>] True count: " << true_count << std::endl;
}

// Benchmarks the performance of bowen::bitvector for setting, assigning, accessing, and traversing elements.
void benchmarkBowenBitvector(){

    std::cout << "Benchmarking bowen::bitvector..." << std::endl;

    // Define a bitvector
    bowen::bitvector bool_vector(SIZE);

    // Time setting each element
    auto start_set = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        bool_vector[i] = i & 0x1;
    }
    auto end_set = std::chrono::high_resolution_clock::now();
    auto duration_set = std::chrono::duration_cast<std::chrono::milliseconds>(end_set - start_set);
    std::cout << "[bowen::bitvector] Setting all elements took " << duration_set.count() << " milliseconds." << std::endl;

    // Time setting each element using assign
    auto start_assign = std::chrono::high_resolution_clock::now();
    bowen::bitvector vector2;
    vector2.assign(SIZE, 1);
    auto end_assign = std::chrono::high_resolution_clock::now();
    auto duration_assign = std::chrono::duration_cast<std::chrono::milliseconds>(end_assign - start_assign);
    std::cout << "[bowen::bitvector] Assigning all elements took " << duration_assign.count() << " milliseconds." << std::endl;

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
    std::cout << "[bowen::bitvector] Accessing all elements took " << duration_access.count() << " milliseconds." << std::endl;

    // Time traversing the entire vector
    auto start_traverse = std::chrono::high_resolution_clock::now();
    for (auto it = bool_vector.begin(); it != bool_vector.end(); ++it) {
        if (*it) {
            ++true_count;
        }
    }
    auto end_traverse = std::chrono::high_resolution_clock::now();
    auto duration_traverse = std::chrono::duration_cast<std::chrono::milliseconds>(end_traverse - start_traverse);
    std::cout << "[bowen::bitvector] Traversing all elements took " << duration_traverse.count() << " milliseconds." << std::endl;

    std::cout << "[bowen::bitvector] True count: " << true_count << std::endl;
}

// Compares the behavior of bowen::bitvector against std::vector<bool> for basic operations.
void testBitvectorAgainstStdVectorBool(){

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
    std::cout << "Setting all elements took " << duration_set.count() << " milliseconds." << std::endl;
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
    std::cout << "Accessing all elements took " << duration_access.count() << " milliseconds." << std::endl;
    std::cout << "found " << false_count << " error in bitvector" << std::endl;


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

void calculateSpeedup(double std_time, double bowen_time) {
    double speedup = std_time / bowen_time;
    std::cout << "Speedup (std::vector<bool> vs bowen::bitvector): " << speedup << "x" << std::endl;
}

int main() {
    // Benchmark std::vector<bool>
    auto start_std = std::chrono::high_resolution_clock::now();
    benchmarkStdVectorBool();
    auto end_std = std::chrono::high_resolution_clock::now();
    double std_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_std - start_std).count();

    // Benchmark bowen::bitvector
    auto start_bowen = std::chrono::high_resolution_clock::now();
    benchmarkBowenBitvector();
    auto end_bowen = std::chrono::high_resolution_clock::now();
    double bowen_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_bowen - start_bowen).count();

    // Calculate and print speedup
    calculateSpeedup(std_time, bowen_time);

    // Run all tests

    return 0;
}
