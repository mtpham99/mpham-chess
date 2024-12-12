#include "mpham_chess/move.hpp"

#include "mpham_chess/enums.hpp"
#include "mpham_chess/utils.hpp"

#include <cassert>
#include <cstdint>
#include <ostream>
#include <utility>

namespace mpham_chess {

move::move(std::uint16_t data) noexcept : _data{data} {
  assert(is_valid_flags());
}

move::move(square from, square to, move_flags flags) noexcept
    : _data{static_cast<std::uint16_t>(
          (std::to_underlying(from) << constants::move::from_sq_bit_index) |
          (std::to_underlying(to) << constants::move::to_sq_bit_index) |
          (flags << constants::move::flags_bit_index))} {
  assert(from != square::no_square && to != square::no_square);
  assert(is_valid_flags());
}

move_flags move::get_flags() const noexcept {
  return (_data & constants::move::masks::flags) >>
         constants::move::flags_bit_index;
}

square move::get_from_square() const noexcept {
  return static_cast<square>((_data & constants::move::masks::from_sq) >>
                             constants::move::from_sq_bit_index);
}

square move::get_to_square() const noexcept {
  return static_cast<square>((_data & constants::move::masks::to_sq) >>
                             constants::move::to_sq_bit_index);
}

piece_type move::get_promote_piece_type() const noexcept {
  const auto flags{get_flags() & ~constants::move::flags::capture};
  if (flags == constants::move::flags::promote_knight) {
    return piece_type::knight;
  } else if (flags == constants::move::flags::promote_bishop) {
    return piece_type::bishop;
  } else if (flags == constants::move::flags::promote_rook) {
    return piece_type::rook;
  } else if (flags == constants::move::flags::promote_queen) {
    return piece_type::queen;
  } else {
    return piece_type::no_piece_type;
  }
}

bool move::is_quiet() const noexcept {
  const auto flags{get_flags()};
  return flags == constants::move::flags::quiet;
}

bool move::is_capture() const noexcept {
  return _data & constants::move::masks::capture;
}

bool move::is_king_castle() const noexcept {
  const auto flags{get_flags()};
  return flags == constants::move::flags::king_castle;
}

bool move::is_queen_castle() const noexcept {
  const auto flags{get_flags()};
  return flags == constants::move::flags::queen_castle;
}

bool move::is_castle() const noexcept {
  const auto flags{get_flags()};
  return (flags == constants::move::flags::king_castle ||
          flags == constants::move::flags::queen_castle);
}

bool move::is_promote() const noexcept {
  return _data & constants::move::masks::promote;
}

bool move::is_enpassant() const noexcept {
  const auto flags{get_flags()};
  return flags == constants::move::flags::enpassant;
}

bool move::is_double_pawn_push() const noexcept {
  const auto flags{get_flags()};
  return flags == constants::move::flags::double_pawn_push;
}

bool move::is_valid_flags() const noexcept {
  const auto flags{get_flags()};
  return (flags != constants::move::flags::invalid_flag_1 &&
          flags != constants::move::flags::invalid_flag_2);
}

std::ostream &operator<<(std::ostream &os, const move &mv) noexcept {
  os << utils::sq_to_str(mv.get_from_square())
     << utils::sq_to_str(mv.get_to_square());
  const auto promote_pt{mv.get_promote_piece_type()};
  if (promote_pt != piece_type::no_piece_type) {
    os << utils::piecetype_to_char(promote_pt);
  }
  return os;
}

} // namespace mpham_chess
