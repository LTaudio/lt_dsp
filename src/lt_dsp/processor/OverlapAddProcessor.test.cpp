#include <lt_dsp/lt_dsp.hpp>

#include "catch2/catch_test_macros.hpp"

TEST_CASE("dsp/processor: OverlapAddProcessor", "[dsp][processor]")
{
    struct TestProcessor
    {
        auto process(std::vector<float>& buffer)
        {
            if (invocations == 0) { REQUIRE(buffer == std::vector<float>{0, 0, 0, 0, 0, 0, 1, 2}); }
            if (invocations == 1) { REQUIRE(buffer == std::vector<float>{0, 0, 0, 0, 1, 2, 3, 4}); }
            if (invocations == 2) { REQUIRE(buffer == std::vector<float>{0, 0, 1, 2, 3, 4, 5, 6}); }
            if (invocations == 3) { REQUIRE(buffer == std::vector<float>{1, 2, 3, 4, 5, 6, 7, 8}); }
            if (invocations == 4) { REQUIRE(buffer == std::vector<float>{3, 4, 5, 6, 7, 8, 9, 10}); }
            if (invocations == 5) { REQUIRE(buffer == std::vector<float>{5, 6, 7, 8, 9, 10, 11, 12}); }
            ++invocations;
        }

        int invocations{0};
    };

    static constexpr auto const windowSize     = 8U;
    static constexpr auto const hopSize        = 2U;
    static constexpr auto const audioBlockSize = 6U;

    auto proc    = lt::OverlapAddProcessor<TestProcessor>{windowSize, hopSize};
    auto samples = std::array<float, 12>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    for (auto i{0U}; i < samples.size(); i += audioBlockSize)
    {
        auto ptr = std::next(std::begin(samples), i);
        proc.process(ptr, audioBlockSize);
    }

    SUCCEED();
}