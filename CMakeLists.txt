cmake_minimum_required(VERSION 3.21)
project(bitvector)

set(CMAKE_CXX_STANDARD 17)

# Allow configuring how long tests may run and how long benchmarks
# should execute.  The timeout for GoogleTest-based unit tests is set via
# `BITVECTOR_TEST_TIMEOUT`.  The minimum runtime for Google Benchmark
# benchmarks is configured with `BITVECTOR_BENCHMARK_MIN_TIME`.
set(BITVECTOR_TEST_TIMEOUT 10 CACHE STRING "Timeout in seconds for each unit test")
set(BITVECTOR_BENCHMARK_MIN_TIME 0.2 CACHE STRING "Minimum time in seconds for benchmark runs")
option(BITVECTOR_NO_BOUND_CHECK "Disable bounds checking in bitvector" OFF)
if(BITVECTOR_NO_BOUND_CHECK)
  add_compile_definitions(BITVECTOR_NO_BOUND_CHECK)
endif()
if(MSVC)
  # Set compiler flags for all configurations
  #add_compile_options("/O2" "/Zi" "/DEBUG")

  # Or, specify flags based on the build type
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Zi /DEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /DEBUG")
endif()

# Optionally enable AVX-512 instructions.  Most CI runners do not support
# AVX-512, so leave it disabled by default to avoid illegal instruction
# failures at runtime.
option(ENABLE_AVX512 "Enable AVX-512 instructions" OFF)
if(ENABLE_AVX512)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-mavx512f)
    elseif(MSVC)
        message(WARNING "AVX-512 support is not available for MSVC in this configuration.")
    endif()
endif()

# Enable BMI1 support
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-mbmi)
elseif(MSVC)
    message(WARNING "BMI1 support is not available for MSVC in this configuration.")
endif()

# Optionally disable bounds checking in the bitvector implementation

# Enable testing
enable_testing()

# FetchContent for Google Test (GTest)
include(FetchContent)
FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0  # or use 'master' for latest version
)
FetchContent_Declare(
        simde
        GIT_REPOSITORY https://github.com/simd-everywhere/simde.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(simde)
include_directories(${simde_SOURCE_DIR})
add_compile_definitions(SIMDE_ENABLE_NATIVE_ALIASES)

# Google Benchmark
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
)
set(BENCHMARK_ENABLE_TESTING OFF)
FetchContent_MakeAvailable(benchmark)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Download and build Google Test
FetchContent_MakeAvailable(gtest)

add_executable(bitvector main.cpp)

# Unit tests
add_executable(bitvector_tests bitvector_test.cpp)
target_link_libraries(bitvector_tests GTest::gtest_main)

# Benchmark target
add_executable(bitvector_benchmark bitvector_benchmark.cpp)
target_link_libraries(bitvector_benchmark benchmark::benchmark)
target_compile_definitions(bitvector_benchmark PRIVATE BITVECTOR_BENCHMARK_MIN_TIME=${BITVECTOR_BENCHMARK_MIN_TIME})


# Link your project with Google Test (only for test purposes)
#add_executable(PrimeIteratorTests test_prime_iterator.cpp)

# Link the Google Test libraries with your test executable
#target_link_libraries(PrimeIteratorTests GTest::gtest_main)

# Optionally: Add your own project dependencies or source files
# target_sources(MyProjectTests PRIVATE test_main.cpp)

# Enable test discovery
include(GoogleTest)
gtest_discover_tests(bitvector_tests PROPERTIES TIMEOUT ${BITVECTOR_TEST_TIMEOUT})

