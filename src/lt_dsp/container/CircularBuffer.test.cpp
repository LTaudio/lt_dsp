#include <lt_dsp/lt_dsp.hpp>

#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"

TEMPLATE_TEST_CASE("dsp/container: CircularBuffer", "[dsp][container]", short, unsigned, int, float, double)
{
    using T = TestType;

    SECTION("default construct")
    {
        auto cb = lt::CircularBuffer<T>{};
        REQUIRE(cb.empty());
        REQUIRE(cb.size() == 0U);
    }

    SECTION("size construct")
    {
        auto cb = lt::CircularBuffer<T>{3U, T{}};
        cb.push_back(1);
        cb.push_back(2);
        cb.push_back(3);

        REQUIRE(!cb.empty());
        REQUIRE(cb.size() == 3U);

        REQUIRE(cb[0] == T{1});
        REQUIRE(cb[1] == T{2});
        REQUIRE(cb[2] == T{3});

        cb.push_back(4);  // Overwrite 1 with 4.
        REQUIRE(cb[0] == T{2});
        REQUIRE(cb[1] == T{3});
        REQUIRE(cb[2] == T{4});

        cb.push_back(5);  // Overwrite 2 with 5.
        REQUIRE(cb[0] == T{3});
        REQUIRE(cb[1] == T{4});
        REQUIRE(cb[2] == T{5});

        auto f = std::begin(cb);
        auto l = std::end(cb);
        REQUIRE(f != l);
        REQUIRE(*(f++) == T{3});
        REQUIRE(*(f++) == T{4});
        REQUIRE(*(f++) == T{5});
        REQUIRE(f == l);

        auto cf = std::cbegin(cb);
        auto cl = std::cend(cb);
        REQUIRE(cf != cl);
        REQUIRE(*(cf++) == T{3});
        REQUIRE(*(cf++) == T{4});
        REQUIRE(*(cf++) == T{5});
        REQUIRE(cf == cl);
    }
}