#include <lt_core/lt_core.hpp>

#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"

TEMPLATE_TEST_CASE("core/container: Span", "[core][container]", short, unsigned, int, float, double)
{
    using T = TestType;

    SECTION("default construct - dynamic extent")
    {
        auto cb = lt::Span<T>{};
        REQUIRE(cb.empty());
        REQUIRE(cb.size() == 0U);
    }

    // SECTION("default construct - static extent")
    // {
    //     auto cb = lt::Span<T, 4>{};
    //     // REQUIRE(cb.empty());
    //     REQUIRE(cb.size() == 4U);
    // }
}