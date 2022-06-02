#pragma once

template<typename T>
auto operator<<(std::ostream& out, juce::dsp::AudioBlock<T> block) -> std::ostream&
{
    if (block.getNumSamples() == 0 || block.getNumChannels() == 0) { return out << "AudioBlock{}"; }

    auto const samples    = block.getChannelPointer(0);
    auto const numSamples = block.getNumSamples();

    out << "AudioBlock{ " << samples[0];
    for (auto i{1U}; i < numSamples; ++i) { out << ", " << samples[i]; }
    out << " }";

    return out;
}

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

    [[nodiscard]] auto processor() noexcept -> ProcessorType&;
    [[nodiscard]] auto processor() const noexcept -> ProcessorType const&;

private:
    auto processWrapped() -> void;

    ProcessorType _processor;

    std::vector<CircularBuffer<value_type>> _inputBuffers{};
    std::vector<CircularBuffer<value_type>> _outputBuffers{};
    juce::AudioBuffer<value_type> _processBuffer{};

    std::uint32_t _blockSize;
    std::uint32_t _hopSize;
    std::uint32_t _samplesSinceLastHop{0};
};

template<typename FloatType, typename ProcessorType>
OverlapAddProcessor<FloatType, ProcessorType>::OverlapAddProcessor(std::uint32_t blockSize, std::uint32_t hopSize)
    : _blockSize{blockSize}, _hopSize{hopSize}
{
    jassert(hopSize < blockSize);
}

template<typename FloatType, typename ProcessorType>
auto OverlapAddProcessor<FloatType, ProcessorType>::prepare(juce::dsp::ProcessSpec const& spec) -> void
{
    _inputBuffers.resize(spec.numChannels);
    _outputBuffers.resize(spec.numChannels);
    for (auto& buffer : _inputBuffers) { buffer.resize(_blockSize); }
    for (auto& buffer : _outputBuffers) { buffer.resize(_blockSize); }

    auto blockSpec             = spec;
    blockSpec.maximumBlockSize = _blockSize;
    _processor.prepare(blockSpec);

    _processBuffer.setSize(signCast<int>(spec.numChannels), signCast<int>(_blockSize), false, true);
}

template<typename FloatType, typename ProcessorType>
template<typename ProcessContext>
auto OverlapAddProcessor<FloatType, ProcessorType>::process(ProcessContext const& context) -> void
{
    static_assert(std::is_same_v<FloatType, typename ProcessContext::SampleType>);

    auto inBlock  = context.getInputBlock();
    auto outBlock = context.getOutputBlock();

    jassert(inBlock.getNumChannels() == outBlock.getNumChannels());
    jassert(inBlock.getNumSamples() == outBlock.getNumSamples());

    auto const numSamples  = signCast<int>(inBlock.getNumSamples());
    auto const numChannels = inBlock.getNumChannels();

    for (auto i{0}; i < numSamples; ++i)
    {
        for (auto ch{0U}; ch < numChannels; ++ch)
        {
            auto sample = inBlock.getSample(signCast<int>(ch), i);
            _inputBuffers[ch].push_back(sample);
        }

        if (++_samplesSinceLastHop == _hopSize)
        {
            _samplesSinceLastHop = 0;
            processWrapped();
        }
    }

    for (auto ch{0U}; ch < numChannels; ++ch)
    {
        auto const processed = std::cbegin(_outputBuffers[ch]);
        auto* output         = outBlock.getChannelPointer(ch);
        std::copy(processed, std::next(processed, numSamples), output);
    }
}

template<typename FloatType, typename ProcessorType>
auto OverlapAddProcessor<FloatType, ProcessorType>::reset() -> void
{
    _processor.reset();
}

template<typename FloatType, typename ProcessorType>
auto OverlapAddProcessor<FloatType, ProcessorType>::processor() noexcept -> ProcessorType&
{
    return _processor;
}

template<typename FloatType, typename ProcessorType>
auto OverlapAddProcessor<FloatType, ProcessorType>::processor() const noexcept -> ProcessorType const&
{
    return _processor;
}

template<typename FloatType, typename ProcessorType>
auto OverlapAddProcessor<FloatType, ProcessorType>::processWrapped() -> void
{
    jassert(std::size(_outputBuffers) == std::size(_inputBuffers));

    for (auto ch{0U}; ch < std::size(_inputBuffers); ++ch)
    {
        auto const& input = _inputBuffers[ch];
        std::copy(std::cbegin(input), std::cend(input), _processBuffer.getWritePointer(signCast<int>(ch)));
    }

    auto block = juce::dsp::AudioBlock<value_type>(_processBuffer);
    auto ctx   = juce::dsp::ProcessContextReplacing<value_type>(block);
    _processor.process(ctx);

    auto const gain = static_cast<value_type>(_hopSize) / static_cast<value_type>(_blockSize);
    block.multiplyBy(gain);

    for (auto ch{0U}; ch < std::size(_outputBuffers); ++ch)
    {
        auto& out = _outputBuffers[ch];

        auto const* pFirst   = _processBuffer.getReadPointer(signCast<int>(ch));
        auto const* pLast    = pFirst + _processBuffer.getNumSamples();
        auto const pFirstNew = std::prev(pLast, _hopSize);
        std::copy(pFirstNew, pLast, std::back_inserter(out));
        std::transform(pFirst, pFirstNew, std::begin(out), std::begin(out), std::plus<>{});
    }
}

}  // namespace lt