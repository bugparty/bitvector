#include "bitvector.hpp"
#include <benchmark/benchmark.h>
#include <vector>

#ifndef BITVECTOR_BENCHMARK_MIN_TIME
#define BITVECTOR_BENCHMARK_MIN_TIME 0.2
#endif

using bowen::BitVector;

static void BM_Bowen_Set(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    BitVector<> bv(n);
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
    BitVector<> bv;
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
  BitVector<> bv(n);
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

static void BM_Bowen_SetBit(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    BitVector<> bv(n);
    for (size_t i=0;i<n;++i) {
      bv.set_bit(i, static_cast<bool>(i & 1));
    }
    benchmark::ClobberMemory();
  }
}

static void BM_Std_SetBit(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    std::vector<bool> bv(n);
    for (size_t i=0;i<n;++i) {
      bv[i] = static_cast<bool>(i & 1);
    }
    benchmark::ClobberMemory();
  }
}

static void BM_Bowen_SetBitTrueUnsafe(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    BitVector<> bv(n);
    for (size_t i=0;i<n;++i) {
      bv.set_bit_true_unsafe(i);
    }
    benchmark::ClobberMemory();
  }
}

static void BM_Std_SetBitTrueUnsafe(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    std::vector<bool> bv(n);
    for (size_t i=0;i<n;++i) {
      bv[i] = true;
    }
    benchmark::ClobberMemory();
  }
}


static void BM_Bowen_SetBitTrue6(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    BitVector<> bv(n);
    for (size_t pos=0; pos+5 < n; pos+=6) {
      bv.set_bit_true_6(pos, 1);
    }
    benchmark::ClobberMemory();
  }
}

static void BM_Std_SetBitTrue6(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    std::vector<bool> bv(n);
    for (size_t pos=0; pos+5 < n; pos+=6) {
      for (int i=0;i<6;++i) {
        bv[pos+i] = true;
      }
    }
    benchmark::ClobberMemory();
  }
}

static void BM_Bowen_QSetBitTrue6V2(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    BitVector<> bv(n);
    bv.qset_bit_true_6_v2(0, 1, n);
    benchmark::ClobberMemory();
  }
}

static void BM_Std_QSetBitTrue6(benchmark::State& state) {
  size_t n = state.range(0);
  for (auto _ : state) {
    std::vector<bool> bv(n);
    for (size_t i=0;i<n;++i) {
      bv[i] = true;
    }
    benchmark::ClobberMemory();
  }
}

static void BM_Bowen_IncrementUntilZero(benchmark::State& state) {
  size_t n = state.range(0);
  BitVector<> bv(n, true);
  bv.set_bit(n-1, false);
  for (auto _ : state) {
    size_t pos = 0;
    bv.incrementUntilZero(pos);
    benchmark::DoNotOptimize(pos);
  }
}

static void incrementUntilZeroStd(const std::vector<bool>& bv, size_t& pos) {
  while (pos < bv.size() && bv[pos]) ++pos;
}

static void BM_Std_IncrementUntilZero(benchmark::State& state) {
  size_t n = state.range(0);
  std::vector<bool> bv(n, true);
  bv[n-1] = false;
  for (auto _ : state) {
    size_t pos = 0;
    incrementUntilZeroStd(bv, pos);
    benchmark::DoNotOptimize(pos);
  }
}

BENCHMARK(BM_Bowen_Set)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Std_Set)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Bowen_PushBack)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Std_PushBack)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Bowen_Access)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Std_Access)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Bowen_SetBit)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Std_SetBit)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Bowen_SetBitTrueUnsafe)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Std_SetBitTrueUnsafe)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Bowen_SetBitTrue6)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Std_SetBitTrue6)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Bowen_QSetBitTrue6V2)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Std_QSetBitTrue6)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Bowen_IncrementUntilZero)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);
BENCHMARK(BM_Std_IncrementUntilZero)->Arg(1<<20)->MinTime(BITVECTOR_BENCHMARK_MIN_TIME);

BENCHMARK_MAIN();
