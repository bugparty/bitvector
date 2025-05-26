cmake_minimum_required(VERSION 3.21)
project(bitvector)

set(CMAKE_CXX_STANDARD 17)
if(MSVC)
  # Set compiler flags for all configurations
  #add_compile_options("/O2" "/Zi" "/DEBUG")

  # Or, specify flags based on the build type
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Zi /DEBUG")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /DEBUG")
endif()

# Enable AVX-512 support
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-mavx512f)
elseif(MSVC)
    message(WARNING "AVX-512 support is not available for MSVC in this configuration.")
endif()

# Enable BMI1 support
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-mbmi)
elseif(MSVC)
    message(WARNING "BMI1 support is not available for MSVC in this configuration.")
endif()

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

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Download and build Google Test
FetchContent_MakeAvailable(gtest)

add_executable(bitvector main.cpp)


# Link your project with Google Test (only for test purposes)
#add_executable(PrimeIteratorTests test_prime_iterator.cpp)

# Link the Google Test libraries with your test executable
#target_link_libraries(PrimeIteratorTests GTest::gtest_main)

# Optionally: Add your own project dependencies or source files
# target_sources(MyProjectTests PRIVATE test_main.cpp)

# Enable test discovery
include(GoogleTest)
#gtest_discover_tests(PrimeIteratorTests)

