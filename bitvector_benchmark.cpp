#include "bitvector.hpp"
#include <benchmark/benchmark.h>
#include <vector>

using bowen::bitvector;

static void BM_Bowen_Set(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    bitvector<> bv(n);
    for (size_t i=0;i<n;++i) {
      bv[i] = static_cast<bool>(i & 1);
    }
    benchmark::ClobberMemory();
  }
}

static void BM_Std_Set(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    std::vector<bool> bv(n);
    for (size_t i=0;i<n;++i) {
      bv[i] = static_cast<bool>(i & 1);
    }
    benchmark::ClobberMemory();
  }
}

static void BM_Bowen_PushBack(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    bitvector<> bv;
    bv.reserve(n);
    for (size_t i=0;i<n;++i) {
      bv.push_back(static_cast<bool>(i & 1));
    }
    benchmark::ClobberMemory();
  }
}

static void BM_Std_PushBack(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    std::vector<bool> bv;
    bv.reserve(n);
    for (size_t i=0;i<n;++i) {
      bv.push_back(static_cast<bool>(i & 1));
    }
    benchmark::ClobberMemory();
  }
}

static void BM_Bowen_Access(benchmark::State& state) {
  size_t n = state.range(0);
  bitvector<> bv(n);
  for (size_t i=0;i<n;++i) bv[i] = static_cast<bool>(i & 1);
  for (auto _ : state) {
    size_t sum=0;
    for (size_t i=0;i<n;++i) sum += bv[i];
    benchmark::DoNotOptimize(sum);
  }
}

static void BM_Std_Access(benchmark::State& state) {
  size_t n = state.range(0);
  std::vector<bool> bv(n);
  for (size_t i=0;i<n;++i) bv[i] = static_cast<bool>(i & 1);
  for (auto _ : state) {
    size_t sum=0;
    for (size_t i=0;i<n;++i) sum += bv[i];
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(BM_Bowen_Set)->Arg(1<<20);
BENCHMARK(BM_Std_Set)->Arg(1<<20);
BENCHMARK(BM_Bowen_PushBack)->Arg(1<<20);
BENCHMARK(BM_Std_PushBack)->Arg(1<<20);
BENCHMARK(BM_Bowen_Access)->Arg(1<<20);
BENCHMARK(BM_Std_Access)->Arg(1<<20);

BENCHMARK_MAIN();
