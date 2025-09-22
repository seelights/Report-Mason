#pragma once

#include <array>
#include <initializer_list>
#include <vector>

namespace std {

/**
 * @brief A view for a contiguous sequence of objects
 *
 * This class is usually passed by value.
 */
template <typename T>
class span
{
public:
    constexpr span() noexcept = default;
    constexpr span(T* d, size_t size) noexcept : _d(d), _size(size) {}

    template <class U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    constexpr span(const span<U>& other) noexcept : _d(other.begin()), _size(other.size())
    {
    }

    // Constructor for std::array
    template <size_t N>
    constexpr span(std::array<T, N>& arr) noexcept : _d(arr.data()), _size(N) {}

    template <size_t N>
    constexpr span(const std::array<T, N>& arr) noexcept : _d(const_cast<T*>(arr.data())), _size(N) {}

    // Constructor for std::vector
    constexpr span(std::vector<T>& vec) noexcept : _d(vec.data()), _size(vec.size()) {}

    constexpr span(const std::vector<T>& vec) noexcept : _d(const_cast<T*>(vec.data())), _size(vec.size()) {}

    /**
     * @see https://en.cppreference.com/w/cpp/container/span/subspan
     */
    constexpr span<T> subspan(const size_t offset) const noexcept
    {
        return {begin() + offset, size() - offset};
    }

    template <typename U>
    constexpr span<U> cast() const noexcept
    {
        return {reinterpret_cast<U*>(_d), size()};
    }

    constexpr size_t size() const noexcept { return _size; }
    constexpr bool empty() const noexcept { return size() == 0; }

    const T& operator[](size_t i) const { return _d[i]; }
    T& operator[](size_t i) { return _d[i]; }

    const T* cbegin() const noexcept { return _d; }
    const T* cend() const noexcept { return _d + _size; }
    const T* begin() const noexcept { return cbegin(); }
    const T* end() const noexcept { return cend(); }
    T* begin() noexcept { return _d; }
    T* end() noexcept { return _d + _size; }

private:
    T* const _d = nullptr;
    const size_t _size = 0;
};

template <typename Container>
constexpr auto asSpan(const Container& c) noexcept
    -> span<std::remove_pointer_t<decltype(std::data(c))>>
{
    return {std::data(c), std::size(c)};
}

template <typename Container>
constexpr auto asSpan(Container& c) noexcept -> span<std::remove_pointer_t<decltype(std::data(c))>>
{
    return {std::data(c), std::size(c)};
}

template <typename T>
constexpr span<const T> asSpan(std::initializer_list<T> l) noexcept
{
    return {l.begin(), l.size()};
}

} // namespace std