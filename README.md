# bitvector

A compact library focused on lightning-fast bit operations. This project delivers
hardware-friendly algorithms that outpace the STL `bitset`, giving your
applications measurable speed gains alongside an intuitive API.

## Running the benchmarks without bounds checking

Bounds checking is enabled by default. To benchmark without checks, configure and build with (this defines `BITVECTOR_NO_BOUND_CHECK`). The build system now detects whether the compiler and host CPU support AVX2 or other native instructions and enables them when possible:

```bash
cmake -S . -B build -DBITVECTOR_NO_BOUND_CHECK=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/bitvector_benchmark
```

## GitHub Actions Benchmark Results

The following table shows benchmark times from the latest CI run. Each test was executed with `1<<20` bits.

| My Function | Time (ns) | STL Function | Time (ns) | Speedup |
|-------------|-----------|--------------|-----------|---------|
| BM_Bowen_Set | 1826751 | BM_Std_Set | 2975545 | 1.63x |
| BM_Bowen_PushBack | 1998142 | BM_Std_PushBack | 2990558 | 1.50x |
| BM_Bowen_Access | 984200 | BM_Std_Access | 2257978 | 2.29x |
| BM_Bowen_SetBit | 2123333 | BM_Std_SetBit | 2740340 | 1.29x |
| BM_Bowen_SetBitTrueUnsafe | 1992765 | BM_Std_SetBitTrueUnsafe | 2558438 | 1.28x |
| BM_Bowen_SetBitTrue6 | 1545274 | BM_Std_SetBitTrue6 | 2601283 | 1.68x |
| BM_Bowen_QSetBitTrue6V2 | 2248553 | BM_Std_QSetBitTrue6 | 3133552 | 1.39x |
| BM_Bowen_IncrementUntilZero | 34849 | BM_Std_IncrementUntilZero | 1941798 | 55.72x |

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
## API Reference

Below is a quick guide to the main `bowen::BitVector` functions:

- **`BitVector()`** – Construct an empty vector.
- **`BitVector(size_t n, bool value = false)`** – Create a vector with `n` bits initialized to `value`.
- **`operator[]`** – Access or modify a bit by index.
- **`set_bit(size_t pos, bool value)`** – Set the bit at `pos` to `value`.
- **`set_bit_true_unsafe(size_t pos)`** – Like `set_bit(pos, true)` but without bounds checking.
- **`qset_bit_true_6_v2(size_t pos, size_t stride, size_t size)`** – Use SIMD to quickly set bits in a stride pattern.
- **`set_bit_true_6(size_t pos, size_t stride)`** – Set six bits starting from `pos` with the given `stride`.
- **`incrementUntilZero(size_t& pos)`** – Advance `pos` to the first zero bit from its current value.
- **`push_back(bool value)`** – Append a bit to the end of the vector.
- **`reserve(size_t new_capacity)`** – Ensure capacity for at least `new_capacity` bits.
- **`assign(size_t n, bool value)`** – Resize the vector to `n` bits and set each bit to `value`.
- **`data()`** – Pointer to the underlying storage.
- **`size()`** – Number of bits currently stored.
- **`empty()`** – Check whether the vector is empty.
- **`begin()` / `end()`** – Iterators for range-based loops.

Example:

```cpp
#include "bitvector.hpp"
using bowen::BitVector;

int main() {
    BitVector<> bv(8);      // all bits start as 0
    bv.set_bit(3, true);    // set the fourth bit
    bv.push_back(true);     // append one bit
    size_t pos = 0;
    bv.incrementUntilZero(pos);  // pos now points to the first zero bit
}
```
