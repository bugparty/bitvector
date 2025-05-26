#include "bitvector.hpp"
#include "sieve3.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>
void benchOrig(){
    constexpr size_t SIZE = 10000000;  // 1000万元素大小

    // 定义一个vector<bool>
    std::vector<bool> bool_vector(SIZE);

    // 计时设置每个元素为true的时间
    auto start_set = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        bool_vector[i] = i&0x1;
    }
    auto end_set = std::chrono::high_resolution_clock::now();
    auto duration_set = std::chrono::duration_cast<std::chrono::milliseconds>(end_set - start_set);
    std::cout << "Setting all elements took " << duration_set.count() << " milliseconds.\n";

    // 计时访问每个元素的时间
    size_t true_count = 0;
    auto start_access = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        if (bool_vector[i]) {
            ++true_count;
        }
    }
    auto end_access = std::chrono::high_resolution_clock::now();
    auto duration_access = std::chrono::duration_cast<std::chrono::milliseconds>(end_access - start_access);
    std::cout << "Accessing all elements took " << duration_access.count() << " milliseconds.\n";

    // 计时遍历整个vector的时间
    auto start_traverse = std::chrono::high_resolution_clock::now();
    for (auto it = bool_vector.begin(); it != bool_vector.end(); ++it) {
        if (*it) {
            ++true_count;
        }
    }
    auto end_traverse = std::chrono::high_resolution_clock::now();
    auto duration_traverse = std::chrono::duration_cast<std::chrono::milliseconds>(end_traverse - start_traverse);
    std::cout << "Traversing all elements took " << duration_traverse.count() << " milliseconds.\n";

    std::cout << "True count: " << true_count << "\n";

}
void benchMy(){
    constexpr size_t SIZE = 10000000;  // 1000万元素大小

    // 定义一个vector<bool>
    bowen::bitvector bool_vector(SIZE);

    // 计时设置每个元素为true的时间
    auto start_set = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        bool_vector[i] = i&0x1;
    }
    auto end_set = std::chrono::high_resolution_clock::now();
    auto duration_set = std::chrono::duration_cast<std::chrono::milliseconds>(end_set - start_set);
    std::cout << "Setting all elements took " << duration_set.count() << " milliseconds.\n";

        // 计时设置每个元素为true的时间
    auto start_assign = std::chrono::high_resolution_clock::now();
    bowen::bitvector vector2;
    vector2.assign(SIZE, 1);
    auto end_assign = std::chrono::high_resolution_clock::now();
    auto duration_assign = std::chrono::duration_cast<std::chrono::milliseconds>(end_assign - start_assign);
    std::cout << "assign all elements took " << duration_assign.count() << " milliseconds.\n";
    // 计时访问每个元素的时间
    size_t true_count = 0;
    auto start_access = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        if (bool_vector[i]) {
            ++true_count;
        }
    }
    auto end_access = std::chrono::high_resolution_clock::now();
    auto duration_access = std::chrono::duration_cast<std::chrono::milliseconds>(end_access - start_access);
    std::cout << "Accessing all elements took " << duration_access.count() << " milliseconds.\n";

    // 计时遍历整个vector的时间
    auto start_traverse = std::chrono::high_resolution_clock::now();
    for (auto it = bool_vector.begin(); it != bool_vector.end(); ++it) {
        if (*it) {
            ++true_count;
        }
    }
    auto end_traverse = std::chrono::high_resolution_clock::now();
    auto duration_traverse = std::chrono::duration_cast<std::chrono::milliseconds>(end_traverse - start_traverse);
    std::cout << "Traversing all elements took " << duration_traverse.count() << " milliseconds.\n";

    std::cout << "True count: " << true_count << "\n";
}
void tests(){
    constexpr size_t SIZE = 10000000;  // 1000万元素大小

    // 定义一个vector<bool>
    bowen::bitvector bool_vector1(SIZE);
    std::vector<bool> bool_vector2(SIZE);
    // 计时设置每个元素为true的时间
    auto start_set = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        bool_vector1[i] = i&0x1;
        bool_vector2[i] = i&0x1;
    }
    auto end_set = std::chrono::high_resolution_clock::now();
    auto duration_set = std::chrono::duration_cast<std::chrono::milliseconds>(end_set - start_set);
    std::cout << "Setting all elements took " << duration_set.count() << " milliseconds.\n";
    assert(bool_vector1.size()==bool_vector2.size());
    assert(bool_vector1.empty() == bool_vector2.empty());
    // 计时访问每个元素的时间
    size_t false_count = 0;
    auto start_access = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < SIZE; ++i) {
        if (bool_vector1[i] != bool_vector2[i]) {
            ++false_count;
        }
    }
    auto end_access = std::chrono::high_resolution_clock::now();
    auto duration_access = std::chrono::duration_cast<std::chrono::milliseconds>(end_access - start_access);
    std::cout << "Accessing all elements took " << duration_access.count() << " milliseconds.\n";
    std::cout << "found "<< false_count<< " error in bitvector" <<std::endl;


}
void testincrementUntilZero(){
    constexpr size_t SIZE = 128*10;  // 1000万元素大小

    // 定义一个vector<bool>
    bowen::bitvector bool_vector1(SIZE);
    // 计时设置每个元素为true的时间
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
void speedtest() {
    uint64_t n = 4000000000;//357171682
    //n = 100;
    // 计时设置每个元素为true的时间
    auto start_set = std::chrono::high_resolution_clock::now();


    uint64_t primes;
    for (int i = 0; i < 1; i++) {
        sieve3(&primes, n, 1, 0);
    }

    auto end_set = std::chrono::high_resolution_clock::now();
    auto duration_set = std::chrono::duration_cast<std::chrono::milliseconds>(end_set - start_set);
    printf("prime count of %zd is : %zd\n", n, primes);
    std::cout << "Setting all elements took " << duration_set.count() / 3 << " milliseconds.\n";
}
void testPrePrime() {
    std::vector<uint64_t> primes;
    calPrePrimes(primes, 9999, 10000);
    for (auto p: primes) {
        printf("%zd ", p);
    }
}
int main() {
    speedtest();
    return 0;
}
