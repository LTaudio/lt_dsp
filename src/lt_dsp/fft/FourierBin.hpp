#pragma once

#include <complex>
#include <cstddef>

namespace lt
{

template<typename T>
auto frequencyForBin(std::size_t binIndex, std::size_t fftSize, double sampleRate) -> T
{
    return static_cast<T>(binIndex) * static_cast<T>(sampleRate) / static_cast<T>(fftSize);
}

template<typename T>
struct FourierBin
{
    FourierBin() noexcept = default;
    FourierBin(T frequency, std::complex<T> value) noexcept;

    [[nodiscard]] auto value() const noexcept -> std::complex<T>;
    [[nodiscard]] auto frequency() const noexcept -> T;
    [[nodiscard]] auto magnitude() const noexcept -> T;
    [[nodiscard]] auto phase() const noexcept -> T;

private:
    T _frequency{};
    std::complex<T> _value{};
};

template<typename T>
FourierBin<T>::FourierBin(T frequency, std::complex<T> value) noexcept : _frequency{frequency}, _value{value}
{
}

template<typename T>
auto FourierBin<T>::value() const noexcept -> std::complex<T>
{
    return _value;
}

template<typename T>
auto FourierBin<T>::frequency() const noexcept -> T
{
    return _frequency;
}

template<typename T>
auto FourierBin<T>::magnitude() const noexcept -> T
{
    auto const r = real(_value);
    auto const i = imag(_value);
    return std::sqrt(r * r + i * i);
}

template<typename T>
auto FourierBin<T>::phase() const noexcept -> T
{
    return std::atan2(imag(_value), real(_value));
}

}  // namespace lt
