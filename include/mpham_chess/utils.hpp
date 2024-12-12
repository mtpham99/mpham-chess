#pragma once

#include "mpham_chess/constants.hpp"
#include "mpham_chess/enums.hpp"

#include "detail/common.hpp"
#include "detail/nttp_string.hpp"

#include <concepts>
#include <string_view>
#include <utility>
#include <vector>

namespace mpham_chess::utils {

[[nodiscard]] constexpr int full_to_ply(unsigned int movenum,
                                        color side_to_move) noexcept;
[[nodiscard]] constexpr int ply_to_full(unsigned int ply) noexcept;

[[nodiscard]] constexpr file file_of(square sq) noexcept;
[[nodiscard]] constexpr rank rank_of(square sq) noexcept;
[[nodiscard]] constexpr square str_to_sq(std::string_view sq_str) noexcept;
[[nodiscard]] constexpr std::string_view sq_to_str(square sq) noexcept;
[[nodiscard]] constexpr square make_square(file f, rank r) noexcept;

[[nodiscard]] constexpr color color_of(piece pc) noexcept;
[[nodiscard]] constexpr piece_type piecetype_of(piece pc) noexcept;
[[nodiscard]] constexpr piece char_to_piece(char c) noexcept;
[[nodiscard]] constexpr char piecetype_to_char(piece_type pt) noexcept;
[[nodiscard]] constexpr char piece_to_char(piece pc) noexcept;
[[nodiscard]] constexpr piece make_piece(color c, piece_type pt) noexcept;

[[nodiscard]] constexpr castle_rights
make_castle_rights(color c, castle_side cs) noexcept;

template <flip_type ft> [[nodiscard]] constexpr square flip(square sq) noexcept;

namespace str {

template <detail::nttp_string allowed_chars>
[[nodiscard]] constexpr bool contains_only(std::string_view sv) noexcept;

template <detail::nttp_string delimiter = " ",
          class vector = std::vector<std::string_view>>
  requires(detail::common::is_vector_like<vector> &&
           std::same_as<typename vector::value_type, std::string_view>)
[[nodiscard]] vector constexpr split_string(std::string_view sv) noexcept;

} // namespace str

constexpr int full_to_ply(unsigned int movenum, color side_to_move) noexcept {
  return (side_to_move == color::white) ? 2 * (movenum - 1)
                                        : 2 * (movenum - 1) + 1;
}

constexpr int ply_to_full(unsigned int ply) noexcept { return ply / 2 + 1; }

constexpr file file_of(square sq) noexcept {
  assert(sq != square::no_square);
  return file{std::to_underlying(sq) % constants::n_files};
}

constexpr rank rank_of(square sq) noexcept {
  assert(sq != square::no_square);
  return rank{std::to_underlying(sq) / constants::n_files};
}

constexpr square str_to_sq(std::string_view sq_str) noexcept {
  if (sq_str == "-") {
    return square::no_square;
  }
  assert(sq_str.size() == 2);
  const int file_ind{sq_str[0] - 'a'};
  const int rank_ind{sq_str[1] - '1'};
  assert(0 <= file_ind && file_ind < constants::n_files);
  assert(0 <= rank_ind && rank_ind < constants::n_ranks);
  return square{file_ind + rank_ind * constants::n_files};
}

constexpr std::string_view sq_to_str(square sq) noexcept {
  const std::array<const char *, constants::n_squares + 1> all_sq_strs{
      "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2",
      "d2", "e2", "f2", "g2", "h2", "a3", "b3", "c3", "d3", "e3", "f3",
      "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "a5",
      "b5", "c5", "d5", "e5", "f5", "g5", "h5", "a6", "b6", "c6", "d6",
      "e6", "f6", "g6", "h6", "a7", "b7", "c7", "d7", "e7", "f7", "g7",
      "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "-"};

  return all_sq_strs[std::to_underlying(sq)];
}

constexpr square make_square(file f, rank r) noexcept {
  const auto file_ind{std::to_underlying(f)};
  const auto rank_ind{std::to_underlying(r)};
  return square{file_ind + rank_ind * constants::n_files};
}

constexpr color color_of(piece pc) noexcept {
  assert(pc != piece::no_piece);
  return color{std::to_underlying(pc) / constants::n_piece_types};
}

constexpr piece_type piecetype_of(piece pc) noexcept {
  assert(pc != piece::no_piece);
  return piece_type{std::to_underlying(pc) % constants::n_piece_types};
}

constexpr piece char_to_piece(char c) noexcept {
  switch (c) {
  case 'P':
    return piece::w_pawn;
  case 'N':
    return piece::w_knight;
  case 'B':
    return piece::w_bishop;
  case 'R':
    return piece::w_rook;
  case 'Q':
    return piece::w_queen;
  case 'K':
    return piece::w_king;
  case 'p':
    return piece::b_pawn;
  case 'n':
    return piece::b_knight;
  case 'b':
    return piece::b_bishop;
  case 'r':
    return piece::b_rook;
  case 'q':
    return piece::b_queen;
  case 'k':
    return piece::b_king;
  default:
    assert(false);
  }
}

constexpr char piecetype_to_char(piece_type pt) noexcept {
  assert(pt != piece_type::no_piece_type);
  const auto piece_chars{"pnbrqk"};
  return piece_chars[std::to_underlying(pt)];
}

constexpr char piece_to_char(piece pc) noexcept {
  assert(pc != piece::no_piece);
  const auto piece_chars{"PNBRQKpnbrqk"};
  return piece_chars[std::to_underlying(pc)];
}

constexpr piece make_piece(color c, piece_type pt) noexcept {
  assert(pt != piece_type::no_piece_type);
  const auto c_ind{std::to_underlying(c)};
  const auto pt_ind{std::to_underlying(pt)};
  return piece{c_ind * constants::n_piece_types + pt_ind};
}

constexpr castle_rights make_castle_rights(color c, castle_side cs) noexcept {
  auto castle_flag{castle_rights::wb_both};
  castle_flag &=
      (c == color::white) ? castle_rights::w_both : castle_rights::b_both;
  castle_flag &= (cs == castle_side::king) ? castle_rights::wb_king
                                           : castle_rights::wb_queen;
  return castle_flag;
}

template <flip_type ft> constexpr square flip(square sq) noexcept {
  // source:
  // https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#Flip_and_Mirror
  assert(sq != square::no_square);
  auto sq_ind{std::to_underlying(sq)};
  if constexpr (ft == flip_type::vert) {
    sq_ind ^= 56;
  } else if constexpr (ft == flip_type::horiz) {
    sq_ind ^= 7;
  } else if constexpr (ft == flip_type::diag) {
    sq_ind = ((sq_ind >> 3) | (sq_ind << 3)) & 63;
    sq_ind = (sq_ind * 0x20800000) >> 26;
  } else if constexpr (ft == flip_type::antidiag) {
    sq_ind = (((sq_ind >> 3) | (sq_ind << 3)) & 63) ^ 63;
    sq_ind = ((sq_ind * 0x20800000) >> 26) ^ 63;
  } else {
    static_assert(false);
  }
  return square{sq_ind};
}

namespace str {

template <detail::nttp_string allowed_chars>
constexpr bool contains_only(std::string_view sv) noexcept {
  if constexpr (allowed_chars.empty()) {
    return false;
  }
  if (sv.empty()) {
    return true;
  }
  return sv.find_first_not_of(allowed_chars.c_str()) != std::string_view::npos;
}

template <detail::nttp_string delimiter, class vector>
  requires(detail::common::is_vector_like<vector> &&
           std::same_as<typename vector::value_type, std::string_view>)
constexpr vector split_string(std::string_view sv) noexcept {
  if constexpr (delimiter.empty()) {
    return vector{sv};
  }

  vector chunks{};
  std::size_t start{0};
  while (start != std::string_view::npos) {
    const std::size_t end{sv.find(delimiter.c_str(), start)};
    if (end - start > 0) {
      chunks.emplace_back(sv.substr(start, end - start));
    }
    if (end != std::string_view::npos) {
      start = end + delimiter.size();
    } else {
      break;
    }
  }
  return chunks;
}

} // namespace str

} // namespace mpham_chess::utils
