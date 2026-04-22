# bitvector

`bitvector` is a compact C++17 bit-vector prototype for high-throughput
packed-bit workloads. It targets code that wants the memory density of
`std::vector<bool>` while reducing overhead in common set, access, append, and
scan paths.

This repository is intentionally small: one core header, focused unit tests,
Google Benchmark coverage, CMake configuration, and CI workflows. It is best
read as a performance-engineering proof point rather than a finished production
library.

## Why It Matters

Packed bitmaps show up anywhere large boolean state needs to move quickly:
search and filtering, indexes, compression, graph traversal, allocation maps,
schedulers, feature flags, and analytics engines. In these systems, memory
traffic and per-bit branching can dominate runtime.

For an investor, this project demonstrates that focused low-level optimization
can unlock measurable gains in infrastructure components where small per-bit
improvements compound at scale. For an interviewer, it shows hands-on systems
work: custom storage, proxy references, iterators, bounds-check tradeoffs,
SIMD-aware operations, benchmark design, and CI validation.

## Proof Points

- Header-first C++17 implementation in `bitvector.hpp`.
- Benchmarked against `std::vector<bool>` with Google Benchmark.
- Unit-tested with GoogleTest and exercised in GitHub Actions.
- Uses packed word storage, fast word indexing, BMI/TZCNT scanning, and
  SIMD-aware specialized setters.
- CMake detects native compiler/CPU flags where possible and can enable AVX2 or
  optional AVX-512 paths.

## Benchmark Snapshot

The table below is a CI snapshot recorded for `1 << 20` bits. It is useful as a
directional proof point, not a universal performance guarantee. Hardware,
compiler, flags, bit density, access pattern, and safety settings can all change
the result, so target workloads should rerun the benchmark locally.

| BitVector Benchmark | Time (ns) | `std::vector<bool>` Benchmark | Time (ns) | Speedup |
|---------------------|-----------|-------------------------------|-----------|---------|
| BM_Bowen_Set | 1826751 | BM_Std_Set | 2975545 | 1.63x |
| BM_Bowen_PushBack | 1998142 | BM_Std_PushBack | 2990558 | 1.50x |
| BM_Bowen_Access | 984200 | BM_Std_Access | 2257978 | 2.29x |
| BM_Bowen_SetBit | 2123333 | BM_Std_SetBit | 2740340 | 1.29x |
| BM_Bowen_SetBitTrueUnsafe | 1992765 | BM_Std_SetBitTrueUnsafe | 2558438 | 1.28x |
| BM_Bowen_SetBitTrue6 | 1545274 | BM_Std_SetBitTrue6 | 2601283 | 1.68x |
| BM_Bowen_QSetBitTrue6V2 | 2248553 | BM_Std_QSetBitTrue6 | 3133552 | 1.39x |
| BM_Bowen_IncrementUntilZero | 34849 | BM_Std_IncrementUntilZero | 1941798 | 55.72x |

The largest speedup comes from `incrementUntilZero`, a specialized scan that
skips whole machine words by counting trailing zeros in the inverted word.

Benchmark environment for the snapshot above:

```text
Architecture: x86_64
CPU(s): 5
Model name: Intel(R) Xeon(R) Platinum 8272CL CPU @ 2.60GHz
```

## Design Highlights

- **Packed storage:** bits are stored in `unsigned long` words, with word and
  bit offsets computed through shifts and masks.
- **Fast indexing:** `WORD_SHIFT` is computed at compile time and checked with a
  `static_assert` so word indexing can use shifts instead of division.
- **Proxy references:** mutable `operator[]` returns a bit reference object, so
  callers can write `bv[i] = true` while the vector remains packed.
- **Specialized scan:** `incrementUntilZero(size_t& pos)` advances through runs
  of set bits and uses `_tzcnt_u64` to skip full words efficiently.
- **SIMD-aware setters:** functions such as `set_bit_true_6` and
  `qset_bit_true_6_v2` explore faster paths for structured write patterns.
- **Allocator flexibility:** the implementation is allocator-templated and
  includes an aligned allocator option backed by `_mm_malloc`.
- **Build-time tuning:** CMake enables BMI support, attempts native/AVX2
  compiler flags, includes SIMDe, and leaves AVX-512 behind an explicit option.

## Engineering Tradeoffs

This project is optimized for experiments and benchmarks, so it exposes both
safe and unsafe paths:

- The current CMake default sets `BITVECTOR_NO_BOUND_CHECK=ON`, which compiles
  out bounds checks for performance-oriented builds.
- Set `-DBITVECTOR_NO_BOUND_CHECK=OFF` when validating caller behavior or
  debugging out-of-range access.
- `set_bit_true_unsafe` and SIMD-style setters assume the caller has already
  proven valid positions and sizes.
- `data()` exposes raw storage for low-level integration, but that also means
  callers can bypass invariants.
- The current implementation is x86/x64-oriented because it directly includes
  native intrinsic headers; ARM support is a clear portability follow-up.
- The repository does not currently include packaging metadata or a license
  file, so external adoption should start by resolving those basics.

## Quick Start

Requirements:

- CMake 3.21 or newer.
- A C++17 compiler.
- An x86 or x64 target for the current prototype. The header includes native
  x86 intrinsic headers, so ARM builds need portability work before they can
  compile cleanly.
- Network access on first configure, because CMake fetches GoogleTest, SIMDe,
  and Google Benchmark.

If you are using CMake 4.x, the pinned GoogleTest release may require adding
`-DCMAKE_POLICY_VERSION_MINIMUM=3.5` to the configure command until that
dependency is upgraded.

Build, test, and run benchmarks:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure
./build/bitvector_benchmark
```

Run a debug-oriented build with runtime bounds checks enabled:

```bash
cmake -S . -B build-safe -DBITVECTOR_NO_BOUND_CHECK=OFF -DCMAKE_BUILD_TYPE=Debug
cmake --build build-safe --config Debug
ctest --test-dir build-safe --output-on-failure
```

Run the shorter benchmark configuration used by CI:

```bash
./build/bitvector_benchmark --benchmark_min_time=0.01s
```

## Usage Example

```cpp
#include "bitvector.hpp"
#include <cstddef>

int main() {
    bowen::BitVector<> bits(1024, false);

    bits.set_bit(3, true);
    bits.push_back(true);

    bowen::BitVector<> dense(1024, true);
    dense.set_bit(511, false);

    std::size_t pos = 0;
    dense.incrementUntilZero(pos);
    // pos now holds 511, the first zero bit in the dense vector.
}
```

## API Surface

Core `bowen::BitVector` operations:

- `BitVector()` creates an empty vector.
- `BitVector(size_t n, bool value = false)` creates `n` initialized bits.
- `operator[]` reads or writes a bit by index.
- `set_bit(size_t pos, bool value)` sets a specific bit.
- `set_bit_true_unsafe(size_t pos)` sets a bit without bounds checking.
- `set_bit_true_6(size_t pos, size_t stride)` sets six strided bits.
- `qset_bit_true_6_v2(size_t pos, size_t stride, size_t size)` explores a
  SIMD-style path for repeated strided writes.
- `incrementUntilZero(size_t& pos)` advances `pos` to the next zero bit.
- `push_back(bool value)` appends one bit.
- `reserve(size_t new_capacity)` reserves capacity measured in bits.
- `assign(size_t n, bool value)` resizes and fills the vector.
- `data()` returns the underlying word storage.
- `size()` returns the number of logical bits.
- `empty()` reports whether the vector has no bits.
- `begin()` and `end()` provide iterator access for traversal.

## Validation And CI

The repository includes two GitHub Actions workflows:

- `unit_tests.yml` configures a release build, builds the project, and runs
  GoogleTest through `ctest`.
- `performance.yml` builds the benchmark target, runs Google Benchmark, and
  dumps assembly for selected benchmark functions.

Local tests cover construction, `push_back`, copy and assignment, resizing,
iterator traversal, bounds-check behavior when enabled, and
`incrementUntilZero`.

## Repository Map

- `bitvector.hpp` contains the core implementation.
- `bitvector_test.cpp` contains GoogleTest unit coverage.
- `bitvector_benchmark.cpp` contains Google Benchmark comparisons against
  `std::vector<bool>`.
- `CMakeLists.txt` defines build options, dependencies, tests, and benchmarks.
- `.github/workflows/` contains CI validation and benchmark workflows.
- `scripts/dump_benchmark_asm.sh` helps inspect generated assembly for selected
  benchmark functions.

## Roadmap

- Add a license and packaging metadata before external distribution.
- Stabilize API naming and document safe versus unsafe entry points.
- Broaden portability and benchmarks across CPUs, compilers, operating systems,
  and real bitmap-heavy workloads.
- Expand STL compatibility, including const traversal and clearer iterator
  semantics.
- Add fuzz or property-based tests for randomized bit patterns and boundary
  conditions.
