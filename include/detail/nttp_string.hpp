#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <string_view>

namespace detail {

template <std::size_t n_chars, typename char_t = char> struct nttp_string {
  using char_type = char_t;
  using string_storage = std::array<char_type, n_chars + 1>;
  using const_iterator = string_storage::const_iterator;
  using const_reverse_iterator = string_storage::const_reverse_iterator;
  using const_reference = string_storage::const_reference;
  using const_pointer = string_storage::const_pointer;

  string_storage _data{};

  [[nodiscard]] constexpr nttp_string(
      const char_type (&str)[n_chars + 1]) noexcept;

  [[nodiscard]] constexpr std::size_t size() const noexcept;
  [[nodiscard]] constexpr std::size_t capacity() const noexcept;
  [[nodiscard]] constexpr std::size_t max_size() const noexcept;
  [[nodiscard]] constexpr bool empty() const noexcept;

  [[nodiscard]] constexpr const_reference at(std::size_t pos) const noexcept;
  [[nodiscard]] constexpr const_reference
  operator[](std::size_t pos) const noexcept;
  [[nodiscard]] constexpr const char_type *c_str() const noexcept;
  [[nodiscard]] constexpr const_pointer data() const noexcept;

  [[nodiscard]] constexpr const_iterator begin() const noexcept;
  [[nodiscard]] constexpr const_iterator end() const noexcept;
  [[nodiscard]] constexpr const_iterator cbegin() const noexcept;
  [[nodiscard]] constexpr const_iterator cend() const noexcept;
  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept;
  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept;
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept;
  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept;
};

template <std::size_t n_chars, typename char_t>
nttp_string(const char_t (&str)[n_chars]) -> nttp_string<n_chars - 1, char_t>;

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::nttp_string(
    const char_t (&str)[n_chars + 1]) noexcept {
  std::copy(std::begin(str), std::end(str), _data.begin());
}

template <std::size_t n_chars, typename char_t>
constexpr std::size_t nttp_string<n_chars, char_t>::size() const noexcept {
  return n_chars;
}

template <std::size_t n_chars, typename char_t>
constexpr std::size_t nttp_string<n_chars, char_t>::capacity() const noexcept {
  return n_chars;
}

template <std::size_t n_chars, typename char_t>
constexpr std::size_t nttp_string<n_chars, char_t>::max_size() const noexcept {
  return n_chars;
}

template <std::size_t n_chars, typename char_t>
constexpr bool nttp_string<n_chars, char_t>::empty() const noexcept {
  return n_chars == 0;
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_reference
nttp_string<n_chars, char_t>::at(std::size_t pos) const noexcept {
  assert(pos < n_chars);
  return _data.at(pos);
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_reference
nttp_string<n_chars, char_t>::operator[](std::size_t pos) const noexcept {
  assert(pos < n_chars);
  return _data[pos];
}

template <std::size_t n_chars, typename char_t>
constexpr const char_t *nttp_string<n_chars, char_t>::c_str() const noexcept {
  return _data.data();
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_pointer
nttp_string<n_chars, char_t>::data() const noexcept {
  return _data.data();
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_iterator
nttp_string<n_chars, char_t>::begin() const noexcept {
  return _data.begin();
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_iterator
nttp_string<n_chars, char_t>::end() const noexcept {
  return _data.end() - 1;
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_iterator
nttp_string<n_chars, char_t>::cbegin() const noexcept {
  return _data.cbegin();
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_iterator
nttp_string<n_chars, char_t>::cend() const noexcept {
  return _data.cend() - 1;
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_reverse_iterator
nttp_string<n_chars, char_t>::rbegin() const noexcept {
  return _data.rbegin() + 1;
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_reverse_iterator
nttp_string<n_chars, char_t>::rend() const noexcept {
  return _data.rend();
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_reverse_iterator
nttp_string<n_chars, char_t>::crbegin() const noexcept {
  return _data.crbegin() + 1;
}

template <std::size_t n_chars, typename char_t>
constexpr nttp_string<n_chars, char_t>::const_reverse_iterator
nttp_string<n_chars, char_t>::crend() const noexcept {
  return _data.crend();
}

} // namespace detail
