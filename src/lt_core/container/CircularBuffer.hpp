#pragma once

namespace lt
{

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
    auto resize(size_type newSize) -> void;
    auto clear() -> void;

private:
    std::vector<value_type> _buffer{};
    size_type _writeIndex{0};
};

template<typename T>
CircularBuffer<T>::CircularBuffer(size_type size, value_type val) : _buffer(size, val)
{
    jassert(std::size(_buffer) == size);
}

template<typename T>
auto CircularBuffer<T>::empty() const noexcept -> bool
{
    return std::empty(_buffer);
}

template<typename T>
auto CircularBuffer<T>::size() const noexcept -> size_type
{
    return narrowCast<size_type>(std::size(_buffer));
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
    auto const i = _writeIndex + index;
    if (i >= size()) { return _buffer[i - size()]; }
    return _buffer[i];
}

template<typename T>
auto CircularBuffer<T>::operator[](size_type index) const -> const_reference
{
    jassert(index < size());
    auto const i = _writeIndex + index;
    if (i >= size()) { return _buffer[i - size()]; }
    return _buffer[i];
}

template<typename T>
auto CircularBuffer<T>::resize(size_type newSize) -> void
{
    _buffer.resize(newSize);
}

template<typename T>
auto CircularBuffer<T>::clear() -> void
{
    _buffer.clear();
    _writeIndex = 0;
}

}  // namespace lt