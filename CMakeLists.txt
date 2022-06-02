cmake_minimum_required(VERSION 3.15)
project(lt_dsp VERSION 0.1.0)


# Only do these if this is the main project,
# and not if it is included through add_subdirectory.
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(LT_BUILD_BENCHMARK  "Build benchmarks"                                  ON)
    option(LT_BUILD_COVERAGE   "Build with coverage enabled"                       OFF)
    option(LT_BUILD_ASAN       "Build with address sanitizer enabled"              OFF)
    option(LT_BUILD_UBSAN      "Build with undefined behavior sanitizer enabled"   OFF)
    option(LT_BUILD_TSAN       "Build with thread sanitizer enabled"               OFF)
    option(LT_BUILD_MSAN       "Build with memory sanitizer enabled"               OFF)
    option(LT_BUILD_WERROR     "Build with warnings as errors"                     OFF)

    # Caches build artifacts for faster builds
    find_program(CCACHE ccache)
    if (CCACHE)
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
    endif ()

    # Set C11 with no extensions enabled. Fail if the
    # standard is not supported.
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)

    # Set C++20 with no extensions enabled. Fail if the
    # standard is not supported.
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14")

    include(CheckCXXCompilerFlag)
    if(MSVC)
        check_cxx_compiler_flag("/arch:AVX2" COMPILER_AVX2_SUPPORTED)
        if(COMPILER_AVX2_SUPPORTED)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
        endif()
    else()
        check_cxx_compiler_flag("-mavx2" COMPILER_AVX2_SUPPORTED)
        if(COMPILER_AVX2_SUPPORTED)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
        endif()
    endif()
endif ()

set(USE_TYPE_FLOAT      ON CACHE INTERNAL "")
set(USE_TYPE_DOUBLE     ON CACHE INTERNAL "")
set(USE_FFTPACK         OFF CACHE INTERNAL "")
set(USE_BENCH_GREEN     OFF CACHE INTERNAL "")
set(USE_BENCH_KISS      OFF CACHE INTERNAL "")
set(USE_BENCH_POCKET    OFF CACHE INTERNAL "")
add_subdirectory(3rd_party/pffft EXCLUDE_FROM_ALL)

add_subdirectory(3rd_party/concurrentqueue EXCLUDE_FROM_ALL)
add_subdirectory(3rd_party/mdspan EXCLUDE_FROM_ALL)
add_subdirectory(3rd_party/JUCE EXCLUDE_FROM_ALL)

add_library(lt_external INTERFACE)
add_library(lt::lt_external ALIAS lt_external)
target_compile_definitions(lt_external INTERFACE PFFFT_ENABLE_DOUBLE=1 PFFFT_ENABLE_FLOAT=1)
target_link_libraries(lt_external INTERFACE std::mdspan PFFFT)

juce_add_modules(src/lt_core ALIAS_NAMESPACE lt)
juce_add_modules(src/lt_dsp ALIAS_NAMESPACE lt)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    include(ltCompilerOptions)
    include(ltCompilerWarnings)
    include(ltCodeCoverage)

    list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/Catch2/extras")
    enable_testing()
    add_subdirectory(3rd_party/Catch2 EXCLUDE_FROM_ALL)
    include(CTest)
    include(Catch)

    juce_add_console_app(${PROJECT_NAME}_tests PRODUCT_NAME "${PROJECT_NAME}_tests")
    catch_discover_tests(${PROJECT_NAME}_tests)

    target_sources(${PROJECT_NAME}_tests
        PRIVATE
            "src/lt_core/container/CircularBuffer.test.cpp"
            "src/lt_core/iterator/IndexIterator.test.cpp"
            "src/lt_dsp/processor/OverlapAddProcessor.test.cpp"

    )

    target_compile_definitions(${PROJECT_NAME}_tests
        PRIVATE
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_DISABLE_JUCE_VERSION_PRINTING=1
    )

    target_link_libraries(${PROJECT_NAME}_tests
        PRIVATE
            lt::lt_external
            lt::lt_core
            lt::lt_dsp
            juce::juce_events
            Catch2::Catch2WithMain
            juce::juce_recommended_config_flags
            juce::juce_recommended_warning_flags
            lt::CompilerWarnings
            lt::CompilerOptions
            lt::CodeCoverage
    )

    if(${LT_BUILD_BENCHMARK} AND NOT ${LT_BUILD_COVERAGE})
        set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")
        add_subdirectory(3rd_party/benchmark)

        juce_add_console_app(${PROJECT_NAME}_benchmark PRODUCT_NAME "${PROJECT_NAME}_benchmark")

        target_sources(${PROJECT_NAME}_benchmark
            PRIVATE
                "src/lt_dsp/fft/FFT.bench.cpp"
        )

        target_compile_definitions(${PROJECT_NAME}_benchmark
            PRIVATE
                JUCE_WEB_BROWSER=0
                JUCE_USE_CURL=0
                JUCE_DISABLE_JUCE_VERSION_PRINTING=1
        )

        target_link_libraries(${PROJECT_NAME}_benchmark
            PRIVATE
                lt::lt_external
                lt::lt_core
                lt::lt_dsp
                benchmark::benchmark
        )
    endif()
endif ()
