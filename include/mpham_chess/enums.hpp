#pragma once

#include <cassert>
#include <cstdint>
#include <utility>

namespace mpham_chess {

enum class color { white, black };

[[nodiscard]] constexpr color operator~(color c) noexcept;

enum class square {
  // clang-format off
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8,
  no_square
  // clang-format on
};

constexpr square &operator++(square &sq) noexcept;
constexpr square operator++(square &sq, int) noexcept;
constexpr square &operator--(square &sq) noexcept;
constexpr square operator--(square &sq, int) noexcept;
constexpr square &operator+=(square &sq, int shift) noexcept;
[[nodiscard]] constexpr square operator+(square sq, int shift) noexcept;
constexpr square &operator-=(square &sq, int shift) noexcept;
[[nodiscard]] constexpr square operator-(square sq, int shift) noexcept;
[[nodiscard]] constexpr bool operator!(square sq) noexcept;

enum class file {
  file_a,
  file_b,
  file_c,
  file_d,
  file_e,
  file_f,
  file_g,
  file_h
};

enum class rank {
  rank_1,
  rank_2,
  rank_3,
  rank_4,
  rank_5,
  rank_6,
  rank_7,
  rank_8
};

enum class piece_type {
  pawn,
  knight,
  bishop,
  rook,
  queen,
  king,
  no_piece_type
};

template <piece_type pt>
concept slider_pt = requires() {
  pt == piece_type::bishop || pt == piece_type::rook || pt == piece_type::queen;
};

// clang-format off
enum class piece {
  w_pawn, w_knight, w_bishop, w_rook, w_queen, w_king,
  b_pawn, b_knight, b_bishop, b_rook, b_queen, b_king,
  no_piece
};
// clang-format on

enum class castle_side { king, queen };

[[nodiscard]] constexpr castle_side operator~(castle_side cs) noexcept;

// clang-format off
enum class castle_rights : std::uint8_t {
  no_castle      = 0b0000,
  w_king         = 0b0001,
  w_queen        = 0b0010,
  w_both         = 0b0011,
  b_king         = 0b0100,
  wb_king        = 0b0101,
  w_queen_b_king = 0b0110,
  w_both_b_king  = 0b0111,
  b_queen        = 0b1000,
  w_king_b_queen = 0b1001,
  wb_queen       = 0b1010,
  w_both_b_queen = 0b1011,
  b_both         = 0b1100,
  w_king_b_both  = 0b1101,
  w_queen_b_both = 0b1110,
  wb_both        = 0b1111
};
// clang-format on

constexpr castle_rights &operator&=(castle_rights &lhs,
                                    castle_rights rhs) noexcept;
[[nodiscard]] constexpr castle_rights operator&(castle_rights lhs,
                                                castle_rights rhs) noexcept;
constexpr castle_rights &operator|=(castle_rights &lhs,
                                    castle_rights rhs) noexcept;
[[nodiscard]] constexpr castle_rights operator|(castle_rights lhs,
                                                castle_rights rhs) noexcept;
constexpr castle_rights &operator^=(castle_rights &lhs,
                                    castle_rights rhs) noexcept;
[[nodiscard]] constexpr castle_rights operator^(castle_rights lhs,
                                                castle_rights rhs) noexcept;
constexpr castle_rights operator~(castle_rights cr) noexcept;
[[nodiscard]] constexpr bool operator!(castle_rights cr) noexcept;

enum class direction {
  N = 8,
  E = 1,
  S = -N,
  W = -E,
  NE = N + E,
  SE = S + E,
  SW = S + W,
  NW = N + W,
  NNE = NE + N,
  NEE = NE + E,
  SEE = SE + E,
  SSE = SE + S,
  SSW = SW + S,
  SWW = SW + W,
  NWW = NW + W,
  NNW = NW + N
};

// clang-format off
template <direction dir>
concept ray_dir = requires() {
  dir == direction::N || dir == direction::E || dir == direction::S || dir == direction::W
  || dir == direction::NE || dir == direction::SE || dir == direction::SW || dir == direction::NW;
};
// clang-format on

enum class flip_type { vert, horiz, diag, antidiag };

constexpr color operator~(color c) noexcept {
  return (c == color::white) ? color::black : color::white;
}

constexpr square &operator++(square &sq) noexcept {
  [[maybe_unused]] const square new_sq{std::to_underlying(sq) + 1};
  assert(square::a1 <= new_sq && new_sq <= square::h8);
  sq = square{std::to_underlying(sq) + 1};
  return sq;
}

constexpr square operator++(square &sq, int) noexcept {
  square tmp{sq};
  ++sq;
  return tmp;
}

constexpr square &operator--(square &sq) noexcept {
  [[maybe_unused]] const square new_sq{std::to_underlying(sq) - 1};
  assert(square::a1 <= new_sq && new_sq <= square::h8);
  sq = square{std::to_underlying(sq) - 1};
  return sq;
}

constexpr square operator--(square &sq, int) noexcept {
  square tmp{sq};
  --sq;
  return tmp;
}

constexpr square &operator+=(square &sq, int shift) noexcept {
  [[maybe_unused]] const square new_sq{std::to_underlying(sq) + shift};
  assert(square::a1 <= new_sq && new_sq <= square::h8);
  sq = square{std::to_underlying(sq) + shift};
  return sq;
}

constexpr square operator+(square sq, int shift) noexcept {
  sq += shift;
  return sq;
}

constexpr square &operator-=(square &sq, int shift) noexcept {
  [[maybe_unused]] const square new_sq{std::to_underlying(sq) - shift};
  assert(square::a1 <= new_sq && new_sq <= square::h8);
  sq = square{std::to_underlying(sq) - shift};
  return sq;
}

constexpr square operator-(square sq, int shift) noexcept {
  sq -= shift;
  return sq;
}

constexpr bool operator!(square sq) noexcept { return sq == square::no_square; }

constexpr castle_side operator~(castle_side cs) noexcept {
  return (cs == castle_side::king) ? castle_side::queen : castle_side::king;
}

constexpr castle_rights &operator&=(castle_rights &lhs,
                                    castle_rights rhs) noexcept {
  const auto tmp{std::to_underlying(lhs) & std::to_underlying(rhs)};
  assert(tmp <= std::to_underlying(castle_rights::wb_both));
  lhs = static_cast<castle_rights>(tmp);
  return lhs;
}

constexpr castle_rights operator&(castle_rights lhs,
                                  castle_rights rhs) noexcept {
  lhs &= rhs;
  return lhs;
}

constexpr castle_rights &operator|=(castle_rights &lhs,
                                    castle_rights rhs) noexcept {
  const auto tmp{std::to_underlying(lhs) | std::to_underlying(rhs)};
  assert(tmp <= std::to_underlying(castle_rights::wb_both));
  lhs = static_cast<castle_rights>(tmp);
  return lhs;
}

constexpr castle_rights operator|(castle_rights lhs,
                                  castle_rights rhs) noexcept {
  lhs |= rhs;
  return lhs;
}

constexpr castle_rights &operator^=(castle_rights &lhs,
                                    castle_rights rhs) noexcept {
  const auto tmp{std::to_underlying(lhs) ^ std::to_underlying(rhs)};
  assert(tmp <= std::to_underlying(castle_rights::wb_both));
  lhs = static_cast<castle_rights>(tmp);
  return lhs;
}

constexpr castle_rights operator^(castle_rights lhs,
                                  castle_rights rhs) noexcept {
  lhs ^= rhs;
  return lhs;
}

constexpr castle_rights operator~(castle_rights cr) noexcept {
  return cr ^ castle_rights::wb_both;
}

constexpr bool operator!(castle_rights cr) noexcept {
  return cr == castle_rights::no_castle;
}

} // namespace mpham_chess
