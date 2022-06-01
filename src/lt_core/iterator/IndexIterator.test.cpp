#include <lt_core/lt_core.hpp>

#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"

TEMPLATE_TEST_CASE("core/iterator: IndexIterator", "[core][iterator]", short, unsigned, int, float, double)
{
    using T         = TestType;
    using Container = std::array<T, 4>;
    using Iterator  = lt::IndexIterator<Container, false>;
    // using ConstIterator = lt::IndexIterator<Container, true>;

    auto c  = Container{T{0}, T{1}, T{2}, T{3}};
    auto it = Iterator{&c, 0};

    REQUIRE(it.container() == &c);
    REQUIRE(it.index() == 0U);
    REQUIRE(*it == T{0});

    ++it;
    REQUIRE(it.container() == &c);
    REQUIRE(it.index() == 1U);
    REQUIRE(*it == T{1});

    it += 2;
    REQUIRE(it.container() == &c);
    REQUIRE(it.index() == 3U);
    REQUIRE(*it == T{3});
}