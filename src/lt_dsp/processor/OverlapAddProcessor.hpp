#pragma once

namespace lt
{

template<typename ProcessorType>
struct OverlapAddProcessor
{
    OverlapAddProcessor(std::uint32_t blockSize, std::uint32_t hopSize)
        : _inputBuffer{blockSize, 0.0f}
        , _outputBuffer{blockSize, 0.0f}
        , _processBuffer(blockSize, 0.0f)
        , _hopSize{hopSize}
    {
        jassert(hopSize < blockSize);
    }

    auto process(float* buffer, std::uint32_t count) -> void
    {
        auto pushSample = [this](auto val)
        {
            _inputBuffer.push_back(val);
            if (++_samplesSinceLastHop == _hopSize)
            {
                _samplesSinceLastHop = 0;
                std::copy(std::cbegin(_inputBuffer), std::cend(_inputBuffer), std::begin(_processBuffer));
                _processor.process(_processBuffer);
                std::copy(std::cbegin(_processBuffer), std::cend(_processBuffer), std::back_inserter(_outputBuffer));
            }
        };

        std::for_each(buffer, std::next(buffer, count), pushSample);
        std::fill(buffer, std::next(buffer, count), 0.0f);
        for (auto i{0U}; i < count; ++i) { buffer[i] = _outputBuffer[i]; }
    }

private:
    ProcessorType _processor;

    CircularBuffer<float> _inputBuffer;
    CircularBuffer<float> _outputBuffer;
    std::vector<float> _processBuffer;

    std::uint32_t _hopSize;
    std::uint32_t _samplesSinceLastHop{0};
};
}  // namespace lt