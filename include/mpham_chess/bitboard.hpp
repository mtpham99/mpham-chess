#pragma once

#include "mpham_chess/enums.hpp"

#include <bit>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <ostream>
#include <utility>

namespace mpham_chess {

class bitboard {
private:
  std::uint64_t _bb{0x0000000000000000};

public:
  [[nodiscard]] constexpr bitboard() noexcept = default;
  [[nodiscard]] explicit constexpr bitboard(std::uint64_t bb) noexcept;
  [[nodiscard]] explicit constexpr bitboard(square sq) noexcept;
  [[nodiscard]] explicit constexpr bitboard(rank r) noexcept;
  [[nodiscard]] explicit constexpr bitboard(file f) noexcept;

  template <typename... bb_t>
    requires(sizeof...(bb_t) > 1 &&
             (std::constructible_from<bitboard, bb_t> && ...))
  [[nodiscard]] explicit constexpr bitboard(bb_t... bbs) noexcept;

  [[nodiscard]] explicit constexpr operator bool() const noexcept;
  [[nodiscard]] explicit constexpr operator std::uint64_t() const noexcept;
  [[nodiscard]] explicit constexpr operator square() const noexcept;

  [[nodiscard]] constexpr bool is_empty() const noexcept;
  [[nodiscard]] constexpr unsigned int bit_count() const noexcept;

  template <typename sq_t>
    requires(std::constructible_from<sq_t, square>)
  [[nodiscard]] constexpr sq_t get_lsb() const noexcept;
  template <typename sq_t>
    requires(std::constructible_from<sq_t, square>)
  constexpr sq_t pop_lsb() noexcept;

  template <typename sq_t>
    requires(std::constructible_from<sq_t, square>)
  [[nodiscard]] constexpr sq_t get_msb() const noexcept;
  template <typename sq_t>
    requires(std::constructible_from<sq_t, square>)
  constexpr sq_t pop_msb() noexcept;

  template <direction dir> constexpr bitboard &shift() noexcept;
  template <direction dir>
  friend constexpr bitboard shift(bitboard bb) noexcept;

  template <flip_type ft> constexpr bitboard &flip() noexcept;
  template <flip_type ft> friend constexpr bitboard flip(bitboard bb) noexcept;

  template <direction dir> constexpr bitboard &fill(bitboard blockers) noexcept;
  template <direction dir>
  friend constexpr bitboard fill(bitboard bb, bitboard blockers) noexcept;

  template <std::integral int_t>
  constexpr bitboard &operator+=(int_t rhs) noexcept;
  constexpr bitboard &operator+=(bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator+(int_t lhs, bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator+(bitboard lhs, int_t rhs) noexcept;
  friend constexpr bitboard operator+(bitboard lhs, bitboard rhs) noexcept;

  template <std::integral int_t>
  constexpr bitboard &operator-=(int_t rhs) noexcept;
  constexpr bitboard &operator-=(bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator-(int_t lhs, bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator-(bitboard lhs, int_t rhs) noexcept;
  friend constexpr bitboard operator-(bitboard lhs, bitboard rhs) noexcept;

  template <std::integral int_t>
  constexpr bitboard &operator*=(int_t rhs) noexcept;
  constexpr bitboard &operator*=(bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator*(int_t lhs, bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator*(bitboard lhs, int_t rhs) noexcept;
  friend constexpr bitboard operator*(bitboard lhs, bitboard rhs) noexcept;

  template <std::integral int_t>
  constexpr bitboard &operator/=(int_t rhs) noexcept;
  constexpr bitboard &operator/=(bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator/(int_t lhs, bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator/(bitboard lhs, int_t rhs) noexcept;
  friend constexpr bitboard operator/(bitboard lhs, bitboard rhs) noexcept;

  template <std::integral int_t>
  constexpr bitboard &operator|=(int_t rhs) noexcept;
  constexpr bitboard &operator|=(bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator|(int_t lhs, bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator|(bitboard lhs, int_t rhs) noexcept;
  friend constexpr bitboard operator|(bitboard lhs, bitboard rhs) noexcept;

  template <std::integral int_t>
  constexpr bitboard &operator&=(int_t rhs) noexcept;
  constexpr bitboard &operator&=(bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator&(int_t lhs, bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator&(bitboard lhs, int_t rhs) noexcept;
  friend constexpr bitboard operator&(bitboard lhs, bitboard rhs) noexcept;

  template <std::integral int_t>
  constexpr bitboard &operator^=(int_t rhs) noexcept;
  constexpr bitboard &operator^=(bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator^(int_t lhs, bitboard rhs) noexcept;
  template <std::integral int_t>
  friend constexpr bitboard operator^(bitboard lhs, int_t rhs) noexcept;
  friend constexpr bitboard operator^(bitboard lhs, bitboard rhs) noexcept;

  constexpr bitboard &operator<<=(int shift) noexcept;
  constexpr bitboard &operator>>=(int shift) noexcept;
  friend constexpr bitboard operator<<(bitboard bb, int shift) noexcept;
  friend constexpr bitboard operator>>(bitboard bb, int shift) noexcept;

  friend constexpr bitboard operator~(bitboard bb) noexcept;

  friend constexpr bool operator<=>(bitboard lhs,
                                    bitboard rhs) noexcept = default;

  friend inline std::ostream &operator<<(std::ostream &os,
                                         bitboard rhs) noexcept;
};

constexpr bitboard::bitboard(std::uint64_t bb) noexcept : _bb{bb} {}

constexpr bitboard::bitboard(square sq) noexcept
    : _bb{std::uint64_t{1} << std::to_underlying(sq)} {}

constexpr bitboard::bitboard(rank r) noexcept
    : _bb{std::uint64_t{0x00000000000000ff} << (8 * std::to_underlying(r))} {}

constexpr bitboard::bitboard(file f) noexcept
    : _bb{std::uint64_t{0x0101010101010101} << std::to_underlying(f)} {}

template <typename... bb_t>
  requires(sizeof...(bb_t) > 1 &&
           (std::constructible_from<bitboard, bb_t> && ...))
constexpr bitboard::bitboard(bb_t... bbs) noexcept
    : _bb{(bitboard{bbs} | ...)} {}

constexpr bitboard::operator bool() const noexcept { return !is_empty(); }

constexpr bitboard::operator std::uint64_t() const noexcept { return _bb; }

constexpr bitboard::operator square() const noexcept {
  assert(std::popcount(_bb) <= 1);
  return get_lsb<square>();
}

namespace constants::bb {

inline constexpr bitboard empty{0x0000000000000000};
inline constexpr bitboard universe{0xffffffffffffffff};
inline constexpr bitboard rank_1{0x00000000000000ff};
inline constexpr bitboard rank_2{0x000000000000ff00};
inline constexpr bitboard rank_3{0x0000000000ff0000};
inline constexpr bitboard rank_4{0x00000000ff000000};
inline constexpr bitboard rank_5{0x000000ff00000000};
inline constexpr bitboard rank_6{0x0000ff0000000000};
inline constexpr bitboard rank_7{0x00ff000000000000};
inline constexpr bitboard rank_8{0xff00000000000000};
inline constexpr bitboard file_a{0x0101010101010101};
inline constexpr bitboard file_b{0x0202020202020202};
inline constexpr bitboard file_c{0x0404040404040404};
inline constexpr bitboard file_d{0x0808080808080808};
inline constexpr bitboard file_e{0x1010101010101010};
inline constexpr bitboard file_f{0x2020202020202020};
inline constexpr bitboard file_g{0x4040404040404040};
inline constexpr bitboard file_h{0x8080808080808080};
inline constexpr bitboard diag_a1h8{0x8040201008040201};
inline constexpr bitboard diag_h1a8{0x0102040810204080};

} // namespace constants::bb

constexpr bool bitboard::is_empty() const noexcept { return _bb == 0; }

constexpr unsigned int bitboard::bit_count() const noexcept {
  return std::popcount(_bb);
}

template <typename sq_t>
  requires(std::constructible_from<sq_t, square>)
constexpr sq_t bitboard::get_lsb() const noexcept {
  const square lsb_sq{std::countr_zero(_bb)};
  return sq_t{lsb_sq};
}

template <typename sq_t>
  requires(std::constructible_from<sq_t, square>)
constexpr sq_t bitboard::pop_lsb() noexcept {
  const auto lsb{get_lsb<sq_t>()};
  _bb &= _bb - 1;
  return lsb;
}

template <typename sq_t>
  requires(std::constructible_from<sq_t, square>)
constexpr sq_t bitboard::get_msb() const noexcept {
  const square msb_sq{is_empty()
                          ? square::no_square
                          : square{(UINT64_WIDTH - std::countl_zero(_bb) - 1)}};
  return sq_t{msb_sq};
}

template <typename sq_t>
  requires(std::constructible_from<sq_t, square>)
constexpr sq_t bitboard::pop_msb() noexcept {
  // TODO : better way to do this?
  const auto msb{get_msb<sq_t>()};
  if (!is_empty()) {
    _bb ^= 1 << (UINT64_WIDTH - std::countl_zero(_bb) - 1);
  }
  return msb;
}

template <direction dir> constexpr bitboard &bitboard::shift() noexcept {
  if constexpr (dir == direction::N) {
    // *this &= ~constants::bb::rank_8;
  } else if constexpr (dir == direction::E) {
    *this &= ~constants::bb::file_h;
  } else if constexpr (dir == direction::S) {
    // *this &= ~constants::bb::rank_1;
  } else if constexpr (dir == direction::W) {
    *this &= ~constants::bb::file_a;
  } else if constexpr (dir == direction::NE) {
    *this &= ~constants::bb::file_h;
    // *this &= ~constants::bb::rank_8;
  } else if constexpr (dir == direction::SE) {
    *this &= ~constants::bb::file_h;
    // *this &= ~constants::bb::rank_1;
  } else if constexpr (dir == direction::SW) {
    *this &= ~constants::bb::file_a;
    // *this &= ~constants::bb::rank_1;
  } else if constexpr (dir == direction::NW) {
    *this &= ~constants::bb::file_a;
    // *this &= ~constants::bb::rank_8;
  } else if constexpr (dir == direction::NNE) {
    *this &= ~constants::bb::file_h;
    // *this &= ~constants::bb::rank_7 & ~constants::bb::rank_8;
  } else if constexpr (dir == direction::NEE) {
    *this &= ~constants::bb::file_g & ~constants::bb::file_h;
    // *this &= ~constants::bb::rank_8;
  } else if constexpr (dir == direction::SEE) {
    *this &= ~constants::bb::file_g & ~constants::bb::file_h;
    // *this &= ~constants::bb::rank_1;
  } else if constexpr (dir == direction::SSE) {
    *this &= ~constants::bb::file_h;
    // *this &= ~constants::bb::rank_1 & ~constants::bb::rank_2;
  } else if constexpr (dir == direction::SSW) {
    *this &= ~constants::bb::file_a;
    // *this &= ~constants::bb::rank_1 & ~constants::bb::rank_2;
  } else if constexpr (dir == direction::SWW) {
    *this &= ~constants::bb::file_a & ~constants::bb::file_b;
    // *this &= ~constants::bb::rank_1;
  } else if constexpr (dir == direction::NWW) {
    *this &= ~constants::bb::file_a & ~constants::bb::file_b;
    // *this &= ~constants::bb::rank_8;
  } else if constexpr (dir == direction::NNW) {
    *this &= ~constants::bb::file_a;
    // *this &= ~constants::bb::rank_7 & ~constants::bb::rank_8;
  } else {
    static_assert(false);
  }

  constexpr auto shift{std::to_underlying(dir)};
  if constexpr (shift >= 0) {
    _bb <<= shift;
  } else {
    _bb >>= -shift;
  }
  return *this;
}

template <direction dir>
[[nodiscard]] constexpr bitboard shift(bitboard bb) noexcept {
  return bb.shift<dir>();
}

template <flip_type ft> constexpr bitboard &bitboard::flip() noexcept {
  // source:
  // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating
  if constexpr (ft == flip_type::vert) {
    // const auto k1{0x00FF00FF00FF00FF};
    // const auto k2{0x0000FFFF0000FFFF};
    // *this = ((*this >> 8) & k1) | ((*this & k1) << 8);
    // *this = ((*this >> 16) & k2) | ((*this & k2) << 16);
    // *this = (*this >> 32) | (*this << 32);
    _bb = std::byteswap(_bb);
  } else if constexpr (ft == flip_type::horiz) {
    const auto k1{0x5555555555555555};
    const auto k2{0x3333333333333333};
    const auto k4{0x0f0f0f0f0f0f0f0f};
    *this = ((*this >> 1) & k1) + 2 * (*this & k1);
    *this = ((*this >> 2) & k2) + 4 * (*this & k2);
    *this = ((*this >> 4) & k4) + 16 * (*this & k4);
  } else if constexpr (ft == flip_type::diag) {
    const auto k1{0x5500550055005500};
    const auto k2{0x3333000033330000};
    const auto k4{0x0f0f0f0f00000000};
    auto t{k4 & (*this ^ (*this << 28))};
    *this ^= t ^ (t >> 28);
    t = k2 & (*this ^ (*this << 14));
    *this ^= t ^ (t >> 14);
    t = k1 & (*this ^ (*this << 7));
    *this ^= t ^ (t >> 7);
  } else if constexpr (ft == flip_type::antidiag) {
    const auto k1{0xaa00aa00aa00aa00};
    const auto k2{0xcccc0000cccc0000};
    const auto k4{0xf0f0f0f00f0f0f0f};
    auto t{*this ^ (*this << 36)};
    *this ^= k4 & (t ^ (*this >> 36));
    t = k2 & (*this ^ (*this << 18));
    *this ^= t ^ (t >> 18);
    t = k1 & (*this ^ (*this << 9));
    *this ^= t ^ (t >> 9);
  } else {
    static_assert(false);
  }
  return *this;
}

template <flip_type ft>
[[nodiscard]] constexpr bitboard flip(bitboard bb) noexcept {
  return bb.flip<ft>();
}

template <direction dir>
constexpr bitboard &bitboard::fill(bitboard blockers) noexcept {
  // source:
  // https://www.chessprogramming.org/Kogge-Stone_Algorithm
  auto free_bb{~blockers};
  if constexpr (dir == direction::N) {
    *this |= free_bb & (*this << 8);
    free_bb &= (free_bb << 8);
    *this |= free_bb & (*this << 16);
    free_bb &= (free_bb << 16);
    *this |= free_bb & (*this << 32);
  } else if constexpr (dir == direction::E) {
    free_bb &= ~constants::bb::file_a;
    *this |= free_bb & (*this << 1);
    free_bb &= (free_bb << 1);
    *this |= free_bb & (*this << 2);
    free_bb &= (free_bb << 2);
    *this |= free_bb & (*this << 4);
  } else if constexpr (dir == direction::S) {
    *this |= free_bb & (*this >> 8);
    free_bb &= (free_bb >> 8);
    *this |= free_bb & (*this >> 16);
    free_bb &= (free_bb >> 16);
    *this |= free_bb & (*this >> 32);
  } else if constexpr (dir == direction::W) {
    free_bb &= ~constants::bb::file_h;
    *this |= free_bb & (*this >> 1);
    free_bb &= (free_bb >> 1);
    *this |= free_bb & (*this >> 2);
    free_bb &= (free_bb >> 2);
    *this |= free_bb & (*this >> 4);
  } else if constexpr (dir == direction::NE) {
    free_bb &= ~constants::bb::file_a;
    *this |= free_bb & (*this << 9);
    free_bb &= (free_bb << 9);
    *this |= free_bb & (*this << 18);
    free_bb &= (free_bb << 18);
    *this |= free_bb & (*this << 36);
  } else if constexpr (dir == direction::SE) {
    free_bb &= ~constants::bb::file_a;
    *this |= free_bb & (*this >> 7);
    free_bb &= (free_bb >> 7);
    *this |= free_bb & (*this >> 14);
    free_bb &= (free_bb >> 14);
    *this |= free_bb & (*this >> 28);
  } else if constexpr (dir == direction::SW) {
    free_bb &= ~constants::bb::file_h;
    *this |= free_bb & (*this >> 9);
    free_bb &= (free_bb >> 9);
    *this |= free_bb & (*this >> 18);
    free_bb &= (free_bb >> 18);
    *this |= free_bb & (*this >> 36);
  } else if constexpr (dir == direction::NW) {
    free_bb &= ~constants::bb::file_h;
    *this |= free_bb & (*this << 7);
    free_bb &= (free_bb << 7);
    *this |= free_bb & (*this << 14);
    free_bb &= (free_bb << 14);
    *this |= free_bb & (*this << 28);
  } else {
    static_assert(false);
  }
  return *this;
}

template <direction dir>
[[nodiscard]] constexpr bitboard fill(bitboard bb, bitboard blockers) noexcept {
  return bb.fill<dir>(blockers);
}

template <std::integral int_t>
constexpr bitboard &bitboard::operator+=(int_t rhs) noexcept {
  _bb += rhs;
  return *this;
}

constexpr bitboard &bitboard::operator+=(bitboard rhs) noexcept {
  _bb += rhs._bb;
  return *this;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator+(int_t lhs, bitboard rhs) noexcept {
  rhs += lhs;
  return rhs;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator+(bitboard lhs, int_t rhs) noexcept {
  lhs += rhs;
  return lhs;
}

[[nodiscard]] constexpr bitboard operator+(bitboard lhs,
                                           bitboard rhs) noexcept {
  lhs += rhs;
  return lhs;
}

template <std::integral int_t>
constexpr bitboard &bitboard::operator-=(int_t rhs) noexcept {
  _bb -= rhs;
  return *this;
}

constexpr bitboard &bitboard::operator-=(bitboard rhs) noexcept {
  _bb -= rhs._bb;
  return *this;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator-(int_t lhs, bitboard rhs) noexcept {
  rhs -= lhs;
  return rhs;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator-(bitboard lhs, int_t rhs) noexcept {
  lhs -= rhs;
  return lhs;
}

[[nodiscard]] constexpr bitboard operator-(bitboard lhs,
                                           bitboard rhs) noexcept {
  lhs -= rhs;
  return lhs;
}

template <std::integral int_t>
constexpr bitboard &bitboard::operator*=(int_t rhs) noexcept {
  _bb *= rhs;
  return *this;
}

constexpr bitboard &bitboard::operator*=(bitboard rhs) noexcept {
  _bb *= rhs._bb;
  return *this;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator*(int_t lhs, bitboard rhs) noexcept {
  rhs *= lhs;
  return rhs;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator*(bitboard lhs, int_t rhs) noexcept {
  lhs *= rhs;
  return lhs;
}

[[nodiscard]] constexpr bitboard operator*(bitboard lhs,
                                           bitboard rhs) noexcept {
  lhs *= rhs;
  return lhs;
}

template <std::integral int_t>
constexpr bitboard &bitboard::operator/=(int_t rhs) noexcept {
  _bb /= rhs;
  return *this;
}

constexpr bitboard &bitboard::operator/=(bitboard rhs) noexcept {
  _bb /= rhs._bb;
  return *this;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator/(int_t lhs, bitboard rhs) noexcept {
  rhs /= lhs;
  return rhs;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator/(bitboard lhs, int_t rhs) noexcept {
  lhs /= rhs;
  return lhs;
}

[[nodiscard]] constexpr bitboard operator/(bitboard lhs,
                                           bitboard rhs) noexcept {
  lhs /= rhs;
  return lhs;
}

template <std::integral int_t>
constexpr bitboard &bitboard::operator|=(int_t rhs) noexcept {
  _bb |= rhs;
  return *this;
}

constexpr bitboard &bitboard::operator|=(bitboard rhs) noexcept {
  _bb |= rhs._bb;
  return *this;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator|(int_t lhs, bitboard rhs) noexcept {
  rhs |= lhs;
  return rhs;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator|(bitboard lhs, int_t rhs) noexcept {
  lhs |= rhs;
  return lhs;
}

[[nodiscard]] constexpr bitboard operator|(bitboard lhs,
                                           bitboard rhs) noexcept {
  lhs |= rhs;
  return lhs;
}

template <std::integral int_t>
constexpr bitboard &bitboard::operator&=(int_t rhs) noexcept {
  _bb &= rhs;
  return *this;
}

constexpr bitboard &bitboard::operator&=(bitboard rhs) noexcept {
  _bb &= rhs._bb;
  return *this;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator&(int_t lhs, bitboard rhs) noexcept {
  rhs &= lhs;
  return rhs;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator&(bitboard lhs, int_t rhs) noexcept {
  lhs &= rhs;
  return lhs;
}

[[nodiscard]] constexpr bitboard operator&(bitboard lhs,
                                           bitboard rhs) noexcept {
  lhs &= rhs;
  return lhs;
}

template <std::integral int_t>
constexpr bitboard &bitboard::operator^=(int_t rhs) noexcept {
  _bb ^= rhs;
  return *this;
}

constexpr bitboard &bitboard::operator^=(bitboard rhs) noexcept {
  _bb ^= rhs._bb;
  return *this;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator^(int_t lhs, bitboard rhs) noexcept {
  rhs ^= lhs;
  return rhs;
}

template <std::integral int_t>
[[nodiscard]] constexpr bitboard operator^(bitboard lhs, int_t rhs) noexcept {
  lhs ^= rhs;
  return lhs;
}

[[nodiscard]] constexpr bitboard operator^(bitboard lhs,
                                           bitboard rhs) noexcept {
  lhs ^= rhs;
  return lhs;
}

constexpr bitboard &bitboard::operator<<=(int shift) noexcept {
  assert(shift >= 0);
  _bb <<= shift;
  return *this;
}

[[nodiscard]] constexpr bitboard operator<<(bitboard bb, int shift) noexcept {
  assert(shift >= 0);
  bb <<= shift;
  return bb;
}

constexpr bitboard &bitboard::operator>>=(int shift) noexcept {
  assert(shift >= 0);
  _bb >>= shift;
  return *this;
}

[[nodiscard]] constexpr bitboard operator>>(bitboard bb, int shift) noexcept {
  assert(shift >= 0);
  bb >>= shift;
  return bb;
}

[[nodiscard]] constexpr bitboard operator~(bitboard bb) noexcept {
  return bitboard{~bb._bb};
}

std::ostream &operator<<(std::ostream &os, bitboard bb) noexcept {
  for (auto sq_ind{0}; sq_ind < UINT64_WIDTH; sq_ind++) {
    const auto fen_sq_ind{sq_ind ^ 56};
    const auto file_ind{sq_ind % 8};
    const auto fen_sq_bb{std::uint64_t{1} << fen_sq_ind};

    os << ((bb & fen_sq_bb) ? '1' : '0');
    os << ((file_ind == 7) ? '\n' : ' ');
  }
  return os;
}

} // namespace mpham_chess
