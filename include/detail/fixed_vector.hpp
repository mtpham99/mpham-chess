#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <utility>

namespace detail {

template <typename dtype, std::size_t n_max> class fixed_vector {
public:
  using value_type = dtype;
  using iterator = std::array<value_type, n_max>::iterator;
  using reverse_iterator = std::array<value_type, n_max>::reverse_iterator;
  using const_iterator = std::array<value_type, n_max>::const_iterator;
  using const_reverse_iterator =
      std::array<value_type, n_max>::const_reverse_iterator;
  using reference = std::array<value_type, n_max>::reference;
  using const_reference = std::array<value_type, n_max>::const_reference;
  using pointer = std::array<value_type, n_max>::pointer;
  using const_pointer = std::array<value_type, n_max>::const_pointer;

private:
  std::array<value_type, n_max> _arr{};
  std::size_t _count{0};

public:
  explicit constexpr fixed_vector() noexcept = default;
  explicit constexpr fixed_vector(
      std::size_t count, const value_type &value = value_type{}) noexcept;
  explicit constexpr fixed_vector(
      std::initializer_list<value_type> values) noexcept;

  template <typename input_iterator>
    requires(std::input_iterator<input_iterator>)
  explicit constexpr fixed_vector(input_iterator first_it,
                                  input_iterator last_it) noexcept;

  template <typename... types>
  constexpr reference emplace_back(types &&...args) noexcept;

  constexpr void push_back(const value_type &value) noexcept;
  constexpr void push_back(value_type &&value) noexcept;
  constexpr void pop_back() noexcept;

  [[nodiscard]] constexpr reference operator[](std::size_t pos) noexcept;
  [[nodiscard]] constexpr const_reference
  operator[](std::size_t pos) const noexcept;

  [[nodiscard]] constexpr reference front() noexcept;
  [[nodiscard]] constexpr const_reference front() const noexcept;
  [[nodiscard]] constexpr reference back() noexcept;
  [[nodiscard]] constexpr const_reference back() const noexcept;

  [[nodiscard]] constexpr std::size_t size() const noexcept;
  [[nodiscard]] constexpr std::size_t capacity() const noexcept;
  [[nodiscard]] constexpr std::size_t max_size() const noexcept;
  [[nodiscard]] constexpr bool empty() const noexcept;

  constexpr void clear() noexcept;
  constexpr void resize(std::size_t count,
                        const value_type &value = value_type{}) noexcept;
  constexpr void swap(fixed_vector<value_type, n_max> &rhs) noexcept;

  [[nodiscard]] constexpr pointer data() noexcept;
  [[nodiscard]] constexpr const_pointer data() const noexcept;

  [[nodiscard]] constexpr iterator begin() noexcept;
  [[nodiscard]] constexpr iterator end() noexcept;
  [[nodiscard]] constexpr iterator begin() const noexcept;
  [[nodiscard]] constexpr iterator end() const noexcept;
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept;
  [[nodiscard]] constexpr const_iterator cend() const noexcept;
  [[nodiscard]] constexpr reverse_iterator rbegin() noexcept;
  [[nodiscard]] constexpr reverse_iterator rend() noexcept;
  [[nodiscard]] constexpr reverse_iterator rbegin() const noexcept;
  [[nodiscard]] constexpr reverse_iterator rend() const noexcept;
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept;
  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept;
};

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::fixed_vector(
    std::size_t count, const value_type &value) noexcept
    : _count{count} {
  assert(_count <= n_max);
  std::fill_n(_arr.begin(), count, value);
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::fixed_vector(
    std::initializer_list<value_type> values) noexcept
    : _count{values.size()} {
  assert(_count <= n_max);
  std::move(values.begin(), values.end(), _arr.begin());
}

template <typename value_type, std::size_t n_max>
template <typename input_iterator>
  requires(std::input_iterator<input_iterator>)
constexpr fixed_vector<value_type, n_max>::fixed_vector(
    input_iterator first_it, input_iterator last_it) noexcept
    : _count{static_cast<std::size_t>(std::distance(first_it, last_it))} {
  assert(_count <= n_max);
  std::move(first_it, last_it, _arr.begin());
}

template <typename value_type, std::size_t n_max>
template <typename... types>
constexpr fixed_vector<value_type, n_max>::reference
fixed_vector<value_type, n_max>::emplace_back(types &&...args) noexcept {
  assert(_count < n_max);
  _arr[_count++] = value_type{std::forward<types>(args)...};
  return _arr[_count];
}

template <typename value_type, std::size_t n_max>
constexpr void
fixed_vector<value_type, n_max>::push_back(const value_type &value) noexcept {
  assert(_count < n_max);
  _arr[_count++] = value;
}

template <typename value_type, std::size_t n_max>
constexpr void
fixed_vector<value_type, n_max>::push_back(value_type &&value) noexcept {
  assert(_count < n_max);
  _arr[_count++] = std::move(value);
}

template <typename value_type, std::size_t n_max>
constexpr void fixed_vector<value_type, n_max>::pop_back() noexcept {
  assert(_count > 0);
  _count--;
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::reference
fixed_vector<value_type, n_max>::operator[](std::size_t pos) noexcept {
  assert(pos < _count);
  return _arr[pos];
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::const_reference
fixed_vector<value_type, n_max>::operator[](std::size_t pos) const noexcept {
  assert(pos < _count);
  return _arr[pos];
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::reference
fixed_vector<value_type, n_max>::front() noexcept {
  assert(_count > 0);
  return _arr[0];
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::const_reference
fixed_vector<value_type, n_max>::front() const noexcept {
  assert(_count > 0);
  return _arr[0];
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::reference
fixed_vector<value_type, n_max>::back() noexcept {
  assert(0 < _count && _count <= n_max);
  return _arr[_count - 1];
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::const_reference
fixed_vector<value_type, n_max>::back() const noexcept {
  assert(0 < _count && _count <= n_max);
  return _arr[_count - 1];
}

template <typename value_type, std::size_t n_max>
constexpr std::size_t fixed_vector<value_type, n_max>::size() const noexcept {
  return _count;
}

template <typename value_type, std::size_t n_max>
constexpr std::size_t
fixed_vector<value_type, n_max>::capacity() const noexcept {
  return n_max;
}

template <typename value_type, std::size_t n_max>
constexpr std::size_t
fixed_vector<value_type, n_max>::max_size() const noexcept {
  return n_max;
}

template <typename value_type, std::size_t n_max>
constexpr bool fixed_vector<value_type, n_max>::empty() const noexcept {
  return _count == 0;
}

template <typename value_type, std::size_t n_max>
constexpr void fixed_vector<value_type, n_max>::clear() noexcept {
  _count = 0;
}

template <typename value_type, std::size_t n_max>
constexpr void
fixed_vector<value_type, n_max>::resize(std::size_t count,
                                        const value_type &value) noexcept {
  assert(count <= n_max);
  if (count > _count) {
    const std::size_t n_new = (count - _count);
    std::fill_n(this->end(), n_new, value);
  }
  _count = count;
}

template <typename value_type, std::size_t n_max>
constexpr void fixed_vector<value_type, n_max>::swap(
    fixed_vector<value_type, n_max> &rhs) noexcept {
  std::swap(*this, rhs);
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::pointer
fixed_vector<value_type, n_max>::data() noexcept {
  return _arr.data();
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::const_pointer
fixed_vector<value_type, n_max>::data() const noexcept {
  return _arr.data();
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::iterator
fixed_vector<value_type, n_max>::begin() noexcept {
  return _arr.begin();
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::iterator
fixed_vector<value_type, n_max>::end() noexcept {
  assert(_count <= n_max);
  return _arr.end() - (n_max - _count);
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::iterator
fixed_vector<value_type, n_max>::begin() const noexcept {
  return _arr.begin();
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::iterator
fixed_vector<value_type, n_max>::end() const noexcept {
  assert(_count <= n_max);
  return _arr.end() - (n_max - _count);
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::const_iterator
fixed_vector<value_type, n_max>::cbegin() const noexcept {
  return _arr.cbegin();
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::const_iterator
fixed_vector<value_type, n_max>::cend() const noexcept {
  assert(_count <= n_max);
  return _arr.cend() - (n_max - _count);
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::reverse_iterator
fixed_vector<value_type, n_max>::rbegin() noexcept {
  assert(_count <= n_max);
  return _arr.rbegin() + (n_max - _count);
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::reverse_iterator
fixed_vector<value_type, n_max>::rend() noexcept {
  return _arr.rend();
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::reverse_iterator
fixed_vector<value_type, n_max>::rbegin() const noexcept {
  assert(_count <= n_max);
  return _arr.rbegin() + (n_max - _count);
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::reverse_iterator
fixed_vector<value_type, n_max>::rend() const noexcept {
  return _arr.rend();
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::const_reverse_iterator
fixed_vector<value_type, n_max>::crbegin() const noexcept {
  assert(_count <= n_max);
  return _arr.crbegin() + (n_max - _count);
}

template <typename value_type, std::size_t n_max>
constexpr fixed_vector<value_type, n_max>::const_reverse_iterator
fixed_vector<value_type, n_max>::crend() const noexcept {
  return _arr.crend();
}

} // namespace detail
