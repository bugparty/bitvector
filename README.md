# bitvector

This repository provides a small bit vector implementation along with tests and benchmarks.

## Running the benchmarks without bounds checking

Bounds checking is enabled by default. To benchmark without checks, configure and build with (this defines `BITVECTOR_NO_BOUND_CHECK`). The build system now detects whether the compiler and host CPU support AVX2 or other native instructions and enables them when possible:

```bash
cmake -S . -B build -DBITVECTOR_NO_BOUND_CHECK=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/bitvector_benchmark
```

## GitHub Actions Benchmark Results

The following table shows benchmark times from the latest CI run. Each test was executed with `1<<20` bits.

| Benchmark | Time (ns) |
|-----------|-----------|
| BM_Bowen_Set | 1826751 |
| BM_Std_Set | 2975545 |
| BM_Bowen_PushBack | 1998142 |
| BM_Std_PushBack | 2990558 |
| BM_Bowen_Access | 984200 |
| BM_Std_Access | 2257978 |
| BM_Bowen_SetBit | 2123333 |
| BM_Std_SetBit | 2740340 |
| BM_Bowen_SetBitTrueUnsafe | 1992765 |
| BM_Std_SetBitTrueUnsafe | 2558438 |
| BM_Bowen_SetBitTrue6 | 1545274 |
| BM_Std_SetBitTrue6 | 2601283 |
| BM_Bowen_QSetBitTrue6V2 | 2248553 |
| BM_Std_QSetBitTrue6 | 3133552 |
| BM_Bowen_IncrementUntilZero | 34849 |
| BM_Std_IncrementUntilZero | 1941798 |

`BM_Bowen_IncrementUntilZero` is the fastest benchmark, showing a substantial improvement over the standard approach.

## Using `incrementUntilZero`

`incrementUntilZero` scans the bit vector starting at the given position until it reaches the first zero bit. The position is updated in place:

```cpp
BitVector<> bv(1024, true);
bv.set_bit(1023, false); // ensure there is a zero bit
size_t pos = 0;
bv.incrementUntilZero(pos);
// `pos` now holds the index of the first zero bit
```


## CPU Info

The benchmarks above were run on the following machine:

```
Architecture: x86_64
CPU(s): 5
Model name: Intel(R) Xeon(R) Platinum 8272CL CPU @ 2.60GHz
```

