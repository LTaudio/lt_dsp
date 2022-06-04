#pragma once

namespace lt
{

#if not(__cpp_lib_span)
inline constexpr auto DynamicExtent = std::dynamic_extent;

template<typename T, std::size_t Extent = DynamicExtent>
using Span = std::span<T, Extent>;
#else
inline constexpr auto DynamicExtent = tcb::dynamic_extent;

template<typename T, std::size_t Extent = DynamicExtent>
using Span = tcb::span<T, Extent>;
#endif

template<typename T, size_t Extent>
[[nodiscard]] auto asBytes(Span<T, Extent> s) noexcept
{
    using RetT = Span<std::byte const, Extent == DynamicExtent ? DynamicExtent : sizeof(T) * Extent>;
    return RetT{reinterpret_cast<std::byte const*>(s.data()), s.size_bytes()};
}

template<typename T, size_t Extent, std::enable_if_t<!std::is_const_v<T>, int> = 0>
[[nodiscard]] auto asWritableBytes(Span<T, Extent> s) noexcept
{
    using RetT = Span<std::byte, Extent == DynamicExtent ? DynamicExtent : sizeof(T) * Extent>;
    return RetT{reinterpret_cast<std::byte*>(s.data()), s.size_bytes()};
}

}  // namespace lt
