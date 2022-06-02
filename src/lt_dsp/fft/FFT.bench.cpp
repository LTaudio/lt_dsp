#include "lt_dsp/lt_dsp.hpp"

#include "pffft.hpp"

#include <benchmark/benchmark.h>

#include <random>

template<typename T>
static auto generateData(size_t size) -> std::vector<T>
{
    // We use static in order to instantiate the random engine
    // and the distribution once only.
    // It may provoke some thread-safety issues.
    constexpr auto minimum = T(-1);
    constexpr auto maximum = T(1);

    static std::uniform_real_distribution<T> distribution(minimum, maximum);
    static std::default_random_engine generator;

    std::vector<T> data(size);
    std::generate(data.begin(), data.end(), []() { return distribution(generator); });
    return data;
}

static void juce_FFT_Roundtrip(benchmark::State& state)
{
    auto const order = static_cast<int>(state.range(0));
    auto const size  = 1 << order;
    auto fft         = juce::dsp::FFT{order};

    auto const testData = generateData<float>(state.range(0));

    auto input = std::vector<juce::dsp::Complex<float>>{};
    input.resize(size);
    auto output = std::vector<juce::dsp::Complex<float>>{};
    output.resize(size);

    for (auto _ : state)
    {
        state.PauseTiming();
        std::copy(cbegin(testData), cend(testData), begin(input));
        std::fill(begin(output), end(output), juce::dsp::Complex<float>{});
        state.ResumeTiming();

        fft.perform(data(input), data(output), false);
        fft.perform(data(output), data(input), true);

        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
}
BENCHMARK(juce_FFT_Roundtrip)->Arg(8)->Arg(9)->Arg(10)->Arg(11)->Arg(12)->Arg(13);

static void pffft_float_Roundtrip(benchmark::State& state)
{
    auto const order = static_cast<int>(state.range(0));
    auto const size  = 1 << order;
    auto fft         = pffft::Fft<std::complex<float>>(size);

    auto const testData = generateData<float>(state.range(0));

    auto input  = fft.valueVector();
    auto output = fft.spectrumVector();

    for (auto _ : state)
    {
        state.PauseTiming();
        std::copy(cbegin(testData), cend(testData), begin(input));
        std::fill(begin(output), end(output), juce::dsp::Complex<float>{});
        state.ResumeTiming();

        fft.forward(input, output);
        fft.inverse(output, input);

        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
}
BENCHMARK(pffft_float_Roundtrip)->Arg(8)->Arg(9)->Arg(10)->Arg(11)->Arg(12)->Arg(13);

static void pffft_double_Roundtrip(benchmark::State& state)
{
    auto const order = static_cast<int>(state.range(0));
    auto const size  = 1 << order;
    auto fft         = pffft::Fft<std::complex<double>>(size);

    auto const testData = generateData<double>(state.range(0));

    auto input  = fft.valueVector();
    auto output = fft.spectrumVector();

    for (auto _ : state)
    {
        state.PauseTiming();
        std::copy(cbegin(testData), cend(testData), begin(input));
        std::fill(begin(output), end(output), juce::dsp::Complex<double>{});
        state.ResumeTiming();

        fft.forward(input, output);
        fft.inverse(output, input);

        benchmark::DoNotOptimize(input.data());
        benchmark::DoNotOptimize(output.data());
        benchmark::ClobberMemory();
    }
}
BENCHMARK(pffft_double_Roundtrip)->Arg(8)->Arg(9)->Arg(10)->Arg(11)->Arg(12)->Arg(13);

BENCHMARK_MAIN();