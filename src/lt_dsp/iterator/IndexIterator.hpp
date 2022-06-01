#pragma once

namespace lt
{

template<typename Container, bool IsConst>
struct IndexIterator
{
    using iterator_category = std::random_access_iterator_tag;
    using container_type    = std::conditional_t<IsConst, Container const, Container>;
    using index_type        = typename Container::size_type;
    using value_type        = typename Container::value_type;
    using difference_type   = std::ptrdiff_t;
    using pointer           = std::conditional_t<IsConst, value_type const*, value_type*>;
    using reference         = std::conditional_t<IsConst, value_type const&, value_type&>;

    IndexIterator(container_type* container, index_type index);

    auto operator*() const -> reference;
    auto operator->() -> pointer;

    auto operator++() -> IndexIterator&;
    auto operator++(int) -> IndexIterator;

    auto operator--() -> IndexIterator&;
    auto operator--(int) -> IndexIterator;

    [[nodiscard]] auto container() const -> container_type*;
    [[nodiscard]] auto index() const -> index_type;

private:
    container_type* _container;
    index_type _index;
};

template<typename Container, bool IsConst>
IndexIterator<Container, IsConst>::IndexIterator(container_type* container, index_type index)
    : _container{container}, _index{index}
{
}

template<typename Container, bool IsConst>
auto IndexIterator<Container, IsConst>::operator*() const -> reference
{
    return (*_container)[_index];
}

template<typename Container, bool IsConst>
auto IndexIterator<Container, IsConst>::operator->() -> pointer
{
    return &(*_container)[_index];
}

template<typename Container, bool IsConst>
auto IndexIterator<Container, IsConst>::operator++() -> IndexIterator&
{
    _index++;
    return *this;
}

template<typename Container, bool IsConst>
auto IndexIterator<Container, IsConst>::operator++(int) -> IndexIterator
{
    auto tmp = IndexIterator{*this};
    ++(*this);
    return tmp;
}

template<typename Container, bool IsConst>
auto IndexIterator<Container, IsConst>::operator--() -> IndexIterator&
{
    _index--;
    return *this;
}

template<typename Container, bool IsConst>
auto IndexIterator<Container, IsConst>::operator--(int) -> IndexIterator
{
    auto tmp = IndexIterator{*this};
    --(*this);
    return tmp;
}

template<typename Container, bool IsConst>
auto IndexIterator<Container, IsConst>::container() const -> container_type*
{
    return _container;
}

template<typename Container, bool IsConst>
auto IndexIterator<Container, IsConst>::index() const -> index_type
{
    return _index;
}

template<typename Container, bool IsConst>
auto operator==(IndexIterator<Container, IsConst> const& lhs, IndexIterator<Container, IsConst> const& rhs) -> bool

{
    return lhs.index() == rhs.index();
};

template<typename Container, bool IsConst>
auto operator!=(IndexIterator<Container, IsConst> const& lhs, IndexIterator<Container, IsConst> const& rhs) -> bool
{
    return !(lhs == rhs);
};

template<typename Container, bool IsConst>
auto operator<(IndexIterator<Container, IsConst> const& lhs, IndexIterator<Container, IsConst> const& rhs) -> bool
{
    return lhs.index() < rhs.index();
};

template<typename Container, bool IsConst>
auto operator<=(IndexIterator<Container, IsConst> const& lhs, IndexIterator<Container, IsConst> const& rhs) -> bool
{
    return lhs.index() <= rhs.index();
};

template<typename Container, bool IsConst>
auto operator>(IndexIterator<Container, IsConst> const& lhs, IndexIterator<Container, IsConst> const& rhs) -> bool
{
    return lhs.index() > rhs.index();
};

template<typename Container, bool IsConst>
auto operator>=(IndexIterator<Container, IsConst> const& lhs, IndexIterator<Container, IsConst> const& rhs) -> bool
{
    return lhs.index() >= rhs.index();
};

template<typename Container, bool IsConst>
auto operator-(IndexIterator<Container, IsConst> const& lhs, IndexIterator<Container, IsConst> const& rhs) ->
    typename IndexIterator<Container, IsConst>::difference_type
{
    return lhs.index() - rhs.index();
};

template<typename Container, bool IsConst>
auto operator-(IndexIterator<Container, IsConst> it, typename IndexIterator<Container, IsConst>::difference_type n)
    -> IndexIterator<Container, IsConst>
{
    return {it.container(), static_cast<typename IndexIterator<Container, IsConst>::index_type>(it.index() - n)};
};

template<typename Container, bool IsConst>
auto operator+(IndexIterator<Container, IsConst> it, typename IndexIterator<Container, IsConst>::difference_type n)
    -> IndexIterator<Container, IsConst>
{
    return {it.container(), static_cast<typename IndexIterator<Container, IsConst>::index_type>(it.index() + n)};
};

template<typename Container, bool IsConst>
auto operator-=(IndexIterator<Container, IsConst>& it, typename IndexIterator<Container, IsConst>::difference_type n)
    -> IndexIterator<Container, IsConst>&
{
    auto tmp = it - n;
    it       = tmp;
    return it;
};

template<typename Container, bool IsConst>
auto operator+=(IndexIterator<Container, IsConst>& it, typename IndexIterator<Container, IsConst>::difference_type n)
    -> IndexIterator<Container, IsConst>&
{
    auto tmp = it - n;
    it       = tmp;
    return it;
};

}  // namespace lt