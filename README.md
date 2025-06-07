# bitvector

This repository provides a small bit vector implementation along with tests and benchmarks.

## Running the benchmarks without bounds checking

Bounds checking is enabled by default. To benchmark without checks, configure and build with (this defines `BITVECTOR_NO_BOUND_CHECK`):

```bash
cmake -S . -B build -DBV_BOUNDS_CHECK=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/bitvector_benchmark
```
