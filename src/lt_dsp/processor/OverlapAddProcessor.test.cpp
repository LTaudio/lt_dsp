#include <lt_dsp/lt_dsp.hpp>

#include "catch2/catch_template_test_macros.hpp"
#include "catch2/catch_test_macros.hpp"

struct TestProcessor
{
    auto prepare(juce::dsp::ProcessSpec const& s) -> void { spec = s; }

    template<typename ProcessContext>
    auto process(ProcessContext const& context) -> void
    {
        auto&& inBlock  = context.getInputBlock();
        auto&& outBlock = context.getOutputBlock();

        REQUIRE(inBlock.getNumChannels() == outBlock.getNumChannels());
        REQUIRE(inBlock.getNumSamples() == outBlock.getNumSamples());

        for (auto ch{0U}; ch < inBlock.getNumChannels(); ++ch)
        {
            auto buffer         = std::vector<float>{};
            auto const* samples = inBlock.getChannelPointer(ch);
            std::copy(samples, samples + inBlock.getNumSamples(), std::back_inserter(buffer));

            if (invocations == 0) { REQUIRE(buffer == std::vector<float>{0, 0, 0, 0, 0, 0, 1, 2}); }
            if (invocations == 1) { REQUIRE(buffer == std::vector<float>{0, 0, 0, 0, 1, 2, 3, 4}); }
            if (invocations == 2) { REQUIRE(buffer == std::vector<float>{0, 0, 1, 2, 3, 4, 5, 6}); }
            if (invocations == 3) { REQUIRE(buffer == std::vector<float>{1, 2, 3, 4, 5, 6, 7, 8}); }
            if (invocations == 4) { REQUIRE(buffer == std::vector<float>{3, 4, 5, 6, 7, 8, 9, 10}); }
            if (invocations == 5) { REQUIRE(buffer == std::vector<float>{5, 6, 7, 8, 9, 10, 11, 12}); }
        }

        ++invocations;

        if (ProcessContext::usesSeparateInputAndOutputBlocks()) { outBlock.copyFrom(inBlock); }
    }

    auto reset() -> void {}

    int invocations{0};
    juce::dsp::ProcessSpec spec{};
};
struct PassthroughProcessor
{
    auto prepare(juce::dsp::ProcessSpec const& s) -> void { (void)s; }

    template<typename ProcessContext>
    auto process(ProcessContext const& context) -> void
    {
        auto&& inBlock  = context.getInputBlock();
        auto&& outBlock = context.getOutputBlock();

        REQUIRE(inBlock.getNumChannels() == outBlock.getNumChannels());
        REQUIRE(inBlock.getNumSamples() == outBlock.getNumSamples());

        // std::cout << "passthrough: " << inBlock << '\n';
        if (ProcessContext::usesSeparateInputAndOutputBlocks()) { outBlock.copyFrom(inBlock); }
    }

    auto reset() -> void {}
};

TEMPLATE_TEST_CASE("dsp/processor: OverlapAddProcessor", "[dsp][processor]", float, double)
{
    static constexpr auto const windowSize     = 8U;
    static constexpr auto const hopSize        = 2U;
    static constexpr auto const audioBlockSize = 6U;
    static constexpr auto const numChannels    = 2U;

    auto proc = lt::OverlapAddProcessor<TestType, TestProcessor>{windowSize, hopSize};

    STATIC_REQUIRE(std::is_same_v<typename decltype(proc)::value_type, TestType>);
    STATIC_REQUIRE(std::is_same_v<typename decltype(proc)::processor_type, TestProcessor>);

    auto samples = std::array<TestType, 12>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto buffer  = juce::AudioBuffer<TestType>{2, lt::signCast<int>(std::size(samples))};
    std::copy(std::cbegin(samples), std::cend(samples), buffer.getWritePointer(0));
    std::copy(std::cbegin(samples), std::cend(samples), buffer.getWritePointer(1));

    auto block = juce::dsp::AudioBlock<TestType>{buffer};
    proc.prepare(juce::dsp::ProcessSpec{44100.0, audioBlockSize, numChannels});
    REQUIRE(proc.processor().spec == juce::dsp::ProcessSpec{44100.0, windowSize, numChannels});

    for (auto i{0U}; i < samples.size(); i += audioBlockSize)
    {
        auto subBlock = block.getSubBlock(i, audioBlockSize);
        auto ctx      = juce::dsp::ProcessContextReplacing{subBlock};
        proc.process(ctx);
    }
}

TEMPLATE_TEST_CASE("dsp/processor: OverlapAddProcessor - overlap", "[dsp][processor]", float, double)
{
    static constexpr auto const windowSize     = 16U;
    static constexpr auto const hopSize        = 8U;
    static constexpr auto const audioBlockSize = 8U;
    static constexpr auto const numChannels    = 1U;

    auto proc    = lt::OverlapAddProcessor<TestType, PassthroughProcessor>{windowSize, hopSize};
    auto samples = std::array<TestType, 32>{};
    std::fill(std::begin(samples), std::end(samples), TestType{1});

    auto buffer = juce::AudioBuffer<TestType>{int(numChannels), lt::signCast<int>(std::size(samples))};
    std::copy(std::cbegin(samples), std::cend(samples), buffer.getWritePointer(0));

    auto block = juce::dsp::AudioBlock<TestType>{buffer};
    proc.prepare(juce::dsp::ProcessSpec{44100.0, audioBlockSize, numChannels});

    for (auto i{0U}; i < samples.size(); i += audioBlockSize)
    {
        auto subBlock = block.getSubBlock(i, audioBlockSize);
        auto ctx      = juce::dsp::ProcessContextReplacing{subBlock};
        // std::cout << "process inp: " << subBlock << '\n';
        proc.process(ctx);
        // std::cout << "process out: " << subBlock << '\n' << '\n';

        auto f = subBlock.getChannelPointer(0);
        auto l = std::next(f, lt::signCast<long>(subBlock.getNumSamples()));
        REQUIRE(std::all_of(f, l, [](auto s) { return (s == TestType{0}) || (s == TestType{1}); }));
    }
}