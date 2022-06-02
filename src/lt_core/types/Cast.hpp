#pragma once

namespace lt
{

template<typename T, typename U>
[[nodiscard]] inline constexpr auto narrowCast(U u) noexcept -> T
{
    static_assert((std::is_signed_v<T>) == (std::is_signed_v<U>),
                  "narrowCast: types must be both signed or both unsigned");
    jassert(std::in_range<T>(u));
    return static_cast<T>(u);
}

template<typename T, typename U>
[[nodiscard]] inline constexpr auto signCast(U u) noexcept -> T
{
    static_assert((std::is_signed_v<T>) != (std::is_signed_v<U>), "signCast: types must be differ in signedness");
    jassert(std::in_range<T>(u));
    return static_cast<T>(u);
}

}  // namespace lt
