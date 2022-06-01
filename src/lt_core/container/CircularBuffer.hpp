#pragma once

namespace lt
{

namespace detail
{
auto circularBufferAtUnsafe(auto* ptr, auto size, auto writeIndex, auto readIndex) -> decltype(ptr[0])
{
    auto i = writeIndex + readIndex;
    if (i >= size) { return ptr[i - size]; }
    return ptr[i];
}

}  // namespace detail

template<typename T>
struct CircularBuffer
{
    using value_type             = T;
    using reference              = value_type&;
    using const_reference        = value_type const&;
    using pointer                = value_type*;
    using const_pointer          = value_type const*;
    using size_type              = std::uint32_t;
    using iterator               = IndexIterator<CircularBuffer<T>, false>;
    using const_iterator         = IndexIterator<CircularBuffer<T>, true>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    CircularBuffer() = default;
    explicit CircularBuffer(size_type size, value_type val = {});

    [[nodiscard]] auto empty() const noexcept -> bool;
    [[nodiscard]] auto size() const noexcept -> size_type;

    [[nodiscard]] auto begin() -> iterator;
    [[nodiscard]] auto begin() const -> const_iterator;
    [[nodiscard]] auto cbegin() const -> const_iterator;

    [[nodiscard]] auto end() -> iterator;
    [[nodiscard]] auto end() const -> const_iterator;
    [[nodiscard]] auto cend() const -> const_iterator;

    [[nodiscard]] auto operator[](size_type index) -> reference;
    [[nodiscard]] auto operator[](size_type index) const -> const_reference;

    auto push_back(const_reference val) -> void;

private:
    std::unique_ptr<value_type[]> _buffer{};
    size_type _size{0};
    size_type _writeIndex{0};
};

template<typename T>
CircularBuffer<T>::CircularBuffer(size_type size, value_type val)
    : _buffer{std::make_unique<value_type[]>(size)}, _size{size}
{
    auto* f = _buffer.get();
    std::fill(f, std::next(f, _size), val);
}

template<typename T>
auto CircularBuffer<T>::empty() const noexcept -> bool
{
    return _size == 0U;
}

template<typename T>
auto CircularBuffer<T>::size() const noexcept -> size_type
{
    return _size;
}

template<typename T>
auto CircularBuffer<T>::begin() -> iterator
{
    return iterator{this, 0};
}
template<typename T>
auto CircularBuffer<T>::end() -> iterator
{
    return iterator{this, size()};
}

template<typename T>
auto CircularBuffer<T>::begin() const -> const_iterator
{
    return const_iterator{this, 0};
}
template<typename T>
auto CircularBuffer<T>::end() const -> const_iterator
{
    return const_iterator{this, size()};
}

template<typename T>
auto CircularBuffer<T>::cbegin() const -> const_iterator
{
    return const_iterator{this, 0};
}
template<typename T>
auto CircularBuffer<T>::cend() const -> const_iterator
{
    return const_iterator{this, size()};
}

template<typename T>
auto CircularBuffer<T>::push_back(const_reference val) -> void
{
    jassert(size() != 0U);
    _buffer[_writeIndex] = val;
    if (_writeIndex++; _writeIndex >= size()) { _writeIndex = 0; }
}

template<typename T>
auto CircularBuffer<T>::operator[](size_type index) -> reference
{
    jassert(index < size());
    return detail::circularBufferAtUnsafe(_buffer.get(), size(), _writeIndex, index);
}

template<typename T>
auto CircularBuffer<T>::operator[](size_type index) const -> const_reference
{
    jassert(index < size());
    return detail::circularBufferAtUnsafe(_buffer.get(), size(), _writeIndex, index);
}
}  // namespace lt