#pragma once

#include <array>
#include <cstdint>

namespace mpham_chess::rng {

class xorshift64 {
  // 64bit xorshifter
  // https://en.wikipedia.org/wiki/xorshift

public:
  enum class rng_type { normal, sparse };

private:
  std::uint64_t _state{default_seed};

public:
  [[nodiscard]] explicit inline xorshift64(
      std::uint64_t state = default_seed) noexcept;

  xorshift64(const xorshift64 &rng) = delete;
  xorshift64 &operator=(const xorshift64 &rng) = delete;

  template <rng_type rng_t = rng_type::normal>
  [[nodiscard]] std::uint64_t generate() noexcept;

  template <std::size_t n, rng_type rng_t = rng_type::normal>
  [[nodiscard]] std::array<std::uint64_t, n> generate_n() noexcept;

private:
  static constexpr std::uint64_t default_seed{84629465829};
};

template <xorshift64::rng_type rng_t>
std::uint64_t xorshift64::generate() noexcept {
  if constexpr (rng_t == xorshift64::rng_type::normal) {
    _state ^= _state >> 12;
    _state ^= _state << 25;
    _state ^= _state >> 27;
    return _state * 0x2545f4914f6cdd1dull;
  } else if constexpr (rng_t == xorshift64::rng_type::sparse) {
    return generate<xorshift64::rng_type::normal>() &
           generate<xorshift64::rng_type::normal>() &
           generate<xorshift64::rng_type::normal>();
  } else {
    static_assert(false);
  }
}

xorshift64::xorshift64(std::uint64_t state) noexcept : _state{state} {};

template <std::size_t n, xorshift64::rng_type rng_t>
std::array<std::uint64_t, n> xorshift64::generate_n() noexcept {
  std::array<std::uint64_t, n> randoms{};
  for (auto &num : randoms) {
    num = generate<rng_t>();
  }
  return randoms;
}

inline xorshift64 main_rng{};

} // namespace mpham_chess::rng
