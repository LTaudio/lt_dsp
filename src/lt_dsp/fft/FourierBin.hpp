#pragma once

#include <complex>
#include <cstddef>

namespace lt
{

/// \brief Returns the centre frequency of a fft bin.
template<typename T>
auto frequencyForBin(std::size_t binIndex, std::size_t fftSize, double sampleRate) -> T
{
    return static_cast<T>(binIndex) * static_cast<T>(sampleRate) / static_cast<T>(fftSize);
}

/// \brief Bundles the fft bin frequency with its complex value.
template<typename T>
struct FourierBin
{
    /// \brief Constructs an empty FourierBin.
    FourierBin() noexcept = default;

    /// \brief Constructs a FourierBin with the given frequency and complex value.
    FourierBin(T frequency, std::complex<T> value) noexcept;

    /// \brief Returns the complex value of this bin.
    [[nodiscard]] auto value() const noexcept -> std::complex<T>;

    /// \brief Returns the centre frequency of this bin.
    [[nodiscard]] auto frequency() const noexcept -> T;

    /// \brief Returns the magnitude of this bin.
    [[nodiscard]] auto magnitude() const noexcept -> T;

    /// \brief Returns the phase in radians of this bin.
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
    return std::abs(value());
}

template<typename T>
auto FourierBin<T>::phase() const noexcept -> T
{
    return std::arg(value());
}

}  // namespace lt
