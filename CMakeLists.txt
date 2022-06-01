cmake_minimum_required(VERSION 3.15)
project(lt_dsp VERSION 0.1.0)

option(LT_BUILD_TOOLS      "Build with warnings as errors"                     OFF)
option(LT_BUILD_COVERAGE   "Build with coverage enabled"                       OFF)
option(LT_BUILD_ASAN       "Build with address sanitizer enabled"              OFF)
option(LT_BUILD_UBSAN      "Build with undefined behavior sanitizer enabled"   OFF)
option(LT_BUILD_TSAN       "Build with thread sanitizer enabled"               OFF)
option(LT_BUILD_MSAN       "Build with memory sanitizer enabled"               OFF)
option(LT_BUILD_WERROR     "Build with warnings as errors"                     OFF)

# Only do these if this is the main project,
# and not if it is included through add_subdirectory.
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
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

    # library settings
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(BUILD_SHARED_LIBS OFF)

    # misc
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # Minimum macOS version
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14")
endif ()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(ltCompilerOptions)
include(ltCompilerWarnings)
include(ltCodeCoverage)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/Catch2/extras")
enable_testing()
add_subdirectory(3rd_party/Catch2 EXCLUDE_FROM_ALL)
include(CTest)
include(Catch)

add_subdirectory(3rd_party/concurrentqueue)
add_subdirectory(3rd_party/mdspan)
add_subdirectory(3rd_party/JUCE)

add_subdirectory(src)

juce_add_console_app(${PROJECT_NAME}_tests PRODUCT_NAME "${PROJECT_NAME}_tests" NEEDS_CURL TRUE)
catch_discover_tests(${PROJECT_NAME}_tests)

target_sources(${PROJECT_NAME}_tests
    PRIVATE
        "src/Main.test.cpp"

        "src/lt_core/container/CircularBuffer.test.cpp"
        "src/lt_core/iterator/IndexIterator.test.cpp"
        "src/lt_dsp/processor/OverlapAddProcessor.test.cpp"

)

target_compile_definitions(${PROJECT_NAME}_tests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_OGGVORBIS=0
        JUCE_DISABLE_JUCE_VERSION_PRINTING=1
)

target_link_libraries(${PROJECT_NAME}_tests
    PRIVATE
        lt::lt_core
        lt::lt_dsp
        std::mdspan
        Catch2::Catch2
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
        juce::juce_recommended_lto_flags
        lt::CompilerWarnings
        lt::CompilerOptions
        lt::CodeCoverage
)

