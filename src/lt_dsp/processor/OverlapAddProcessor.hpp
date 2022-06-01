#pragma once

namespace lt
{

/// \brief Wraps any type of processor and calls its process
/// function in fixed size blocks with a specified overlap.
///
/// \details Useful for FFT-based effects that require a constant
/// window size, often much larger than the audio interface block
/// size used by the system. The latency is equal to the hop size.
template<typename FloatType, typename ProcessorType>
struct OverlapAddProcessor
{
    using value_type     = FloatType;
    using processor_type = ProcessorType;

    OverlapAddProcessor(std::uint32_t blockSize, std::uint32_t hopSize);

    auto prepare(juce::dsp::ProcessSpec const& spec) -> void;

    template<typename ProcessContext>
    auto process(ProcessContext const& context) -> void;

    auto reset() -> void;

private:
    auto processSample(float sample) -> void;

    ProcessorType _processor;

    CircularBuffer<float> _inputBuffer;
    CircularBuffer<float> _outputBuffer;
    juce::AudioBuffer<float> _processBuffer;

    std::uint32_t _blockSize;
    std::uint32_t _hopSize;
    std::uint32_t _samplesSinceLastHop{0};
};

template<typename FloatType, typename ProcessorType>
OverlapAddProcessor<FloatType, ProcessorType>::OverlapAddProcessor(std::uint32_t blockSize, std::uint32_t hopSize)
    : _inputBuffer{blockSize, 0.0f}
    , _outputBuffer{blockSize, 0.0f}
    , _processBuffer{1, static_cast<int>(blockSize)}
    , _blockSize{blockSize}
    , _hopSize{hopSize}
{
    jassert(hopSize < blockSize);
}

template<typename FloatType, typename ProcessorType>
auto OverlapAddProcessor<FloatType, ProcessorType>::prepare(juce::dsp::ProcessSpec const& spec) -> void
{
    // Only mono supported at the moment.
    // TODO(tobi): Multi-channel support
    jassert(spec.numChannels == 1U);

    auto blockSpec             = spec;
    blockSpec.maximumBlockSize = _blockSize;
    _processor.prepare(blockSpec);

    _processBuffer.setSize(static_cast<int>(spec.numChannels), static_cast<int>(_blockSize), false, true);
}

template<typename FloatType, typename ProcessorType>
template<typename ProcessContext>
auto OverlapAddProcessor<FloatType, ProcessorType>::process(ProcessContext const& context) -> void
{
    static_assert(std::is_same_v<FloatType, typename ProcessContext::SampleType>);

    auto&& inBlock  = context.getInputBlock();
    auto&& outBlock = context.getOutputBlock();

    jassert(inBlock.getNumChannels() == 1U);
    jassert(inBlock.getNumChannels() == outBlock.getNumChannels());
    jassert(inBlock.getNumSamples() == outBlock.getNumSamples());

    auto input      = inBlock.getChannelPointer(0);
    auto numSamples = static_cast<int>(inBlock.getNumSamples());

    std::for_each(input, std::next(input, numSamples), [this](auto s) { processSample(s); });

    auto const processed = std::cbegin(_outputBuffer);
    std::copy(processed, std::next(processed, numSamples), outBlock.getChannelPointer(0));
}

template<typename FloatType, typename ProcessorType>
auto OverlapAddProcessor<FloatType, ProcessorType>::reset() -> void
{
    _processor.reset();
}

template<typename FloatType, typename ProcessorType>
auto OverlapAddProcessor<FloatType, ProcessorType>::processSample(float sample) -> void
{
    _inputBuffer.push_back(sample);
    if (++_samplesSinceLastHop == _hopSize)
    {
        _samplesSinceLastHop = 0;

        std::copy(std::cbegin(_inputBuffer), std::cend(_inputBuffer), _processBuffer.getWritePointer(0));

        auto block = juce::dsp::AudioBlock<float>(_processBuffer);
        auto ctx   = juce::dsp::ProcessContextReplacing<float>(block);
        _processor.process(ctx);

        auto const* processed = _processBuffer.getReadPointer(0);
        std::copy(processed, processed + _processBuffer.getNumSamples(), std::back_inserter(_outputBuffer));
    }
}
}  // namespace lt