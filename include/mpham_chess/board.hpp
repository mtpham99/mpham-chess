#pragma once

#include "mpham_chess/attacks.hpp"
#include "mpham_chess/bitboard.hpp"
#include "mpham_chess/constants.hpp"
#include "mpham_chess/enums.hpp"
#include "mpham_chess/move.hpp"
#include "mpham_chess/zobrist.hpp"

#include "detail/fixed_vector.hpp"

#include <array>
#include <concepts>
#include <ostream>
#include <string>
#include <string_view>

namespace mpham_chess {

struct state_info {
  zobrist_hash _hash{0};
  unsigned int _rule50{0};
  square _ep_sq{square::no_square};
  piece _cap_pc{piece::no_piece};
  castle_rights _castle{castle_rights::no_castle};
};

using state_history = detail::fixed_vector<state_info, constants::max_ply>;
using move_history = detail::fixed_vector<move, constants::max_ply>;

using castle_king_squares = std::array<square, constants::n_colors>;
using castle_rook_squares =
    std::array<std::array<square, constants::n_castle_sides>,
               constants::n_colors>;

class board {
private:
  std::array<bitboard, constants::n_squares> _piece_bbs{};
  std::array<bitboard, constants::n_colors> _color_bbs{};
  std::array<piece, constants::n_squares> _piece_list{};

  color _side_to_move{color::white};
  castle_rights _castle{castle_rights::no_castle};
  square _ep_sq{square::no_square};
  unsigned int _start_movenum{0};
  unsigned int _rule50{0};
  zobrist_hash _hash{0};
  state_history _state_hist{};
  move_history _move_hist{};

  castle_king_squares _castle_king_sqs{square::no_square, square::no_square};
  castle_rook_squares _castle_rook_sqs{
      {{square::no_square, square::no_square},
       {square::no_square, square::no_square}}};
  const bool _use_shredder_fen{false};

public:
  [[nodiscard]] explicit board(std::string_view fen = constants::start_pos_fen,
                               bool use_shredder_fen = false) noexcept;
  board(const board &board) = delete;
  board &operator=(const board &board) = delete;

  void load_fen(std::string_view fen) noexcept;
  [[nodiscard]] std::string to_fen() const noexcept;
  friend std::ostream &operator<<(std::ostream &os,
                                  const board &board) noexcept;

  [[nodiscard]] bitboard get_piece_bb(piece pc) const noexcept;
  [[nodiscard]] bitboard get_color_bb(color c) const noexcept;
  [[nodiscard]] bitboard get_occupied_bb() const noexcept;
  [[nodiscard]] bitboard get_unoccupied_bb() const noexcept;
  [[nodiscard]] piece get_piece_on_sq(square sq) const noexcept;
  [[nodiscard]] color get_side_to_move() const noexcept;
  [[nodiscard]] castle_rights get_castle() const noexcept;
  [[nodiscard]] square get_ep_sq() const noexcept;
  [[nodiscard]] unsigned int get_rule50() const noexcept;
  [[nodiscard]] unsigned int get_movenum() const noexcept;
  [[nodiscard]] unsigned int get_ply() const noexcept;
  [[nodiscard]] zobrist_hash get_hash() const noexcept;
  [[nodiscard]] square get_king_castle_sq(color c) const noexcept;
  [[nodiscard]] square get_rook_castle_sq(color c,
                                          castle_side cs) const noexcept;

  template <bool use_side_to_move = true>
  [[nodiscard]] bool is_check() const noexcept;
  [[nodiscard]] bool is_sq_empty(square sq) const noexcept;
  [[nodiscard]] bool can_do_castle(color c, castle_side cs) const noexcept;

  template <typename sq_or_bb>
    requires(std::same_as<sq_or_bb, square> || std::same_as<sq_or_bb, bitboard>)
  [[nodiscard]] bitboard attacks_to(sq_or_bb targets) const noexcept;
  template <color side>
  [[nodiscard]] bitboard attacks_by_color() const noexcept;

  void do_move(move move) noexcept;
  void undo_move() noexcept;

private:
  void move_piece(square from, square to) noexcept;
  void place_piece(square sq, piece pc) noexcept;
  void remove_piece(square sq) noexcept;

  [[nodiscard]] std::string castle_fen_field() const noexcept;
};

template <bool use_side_to_move> bool board::is_check() const noexcept {
  const auto side{use_side_to_move ? _side_to_move : ~_side_to_move};
  const auto king{utils::make_piece(side, piece_type::king)};
  const square king_sq{get_piece_bb(king)};

  const auto enemy_bb{get_color_bb(~side)};
  const auto checkers{attacks_to(king_sq) & enemy_bb};
  return !checkers.is_empty();
}

template <typename sq_or_bb>
  requires(std::same_as<sq_or_bb, square> || std::same_as<sq_or_bb, bitboard>)
bitboard board::attacks_to(sq_or_bb targets) const noexcept {
  const auto w_pawns{_piece_bbs[std::to_underlying(piece::w_pawn)]};
  const auto b_pawns{_piece_bbs[std::to_underlying(piece::b_pawn)]};
  const auto knights{_piece_bbs[std::to_underlying(piece::w_knight)] |
                     _piece_bbs[std::to_underlying(piece::b_knight)]};
  const auto bishops{_piece_bbs[std::to_underlying(piece::w_bishop)] |
                     _piece_bbs[std::to_underlying(piece::b_bishop)]};
  const auto rooks{_piece_bbs[std::to_underlying(piece::w_rook)] |
                   _piece_bbs[std::to_underlying(piece::b_rook)]};
  const auto queens{_piece_bbs[std::to_underlying(piece::w_queen)] |
                    _piece_bbs[std::to_underlying(piece::b_queen)]};
  const auto kings{_piece_bbs[std::to_underlying(piece::w_king)] |
                   _piece_bbs[std::to_underlying(piece::b_king)]};
  const auto blockers{get_occupied_bb()};

  return (attacks::pawn_attacks<color::white>(targets) & b_pawns) |
         (attacks::pawn_attacks<color::black>(targets) & w_pawns) |
         (attacks::knight_attacks(targets) & knights) |
         (attacks::slider_attacks<piece_type::bishop>(targets, blockers) &
          (bishops | queens)) |
         (attacks::slider_attacks<piece_type::rook>(targets, blockers) &
          (rooks | queens)) |
         (attacks::king_attacks(targets) & kings);
}

template <color side> bitboard board::attacks_by_color() const noexcept {
  // TODO : maybe store attack maps and incrementally update
  //        i.e. benchmark this

  bitboard attacks{constants::bb::empty};

  const auto pawn{(side == color::white) ? piece::w_pawn : piece::b_pawn};
  attacks |= attacks::pawn_attacks<side>(get_piece_bb(pawn));

  const auto knight{(side == color::white) ? piece::w_knight : piece::b_knight};
  attacks |= attacks::knight_attacks(get_piece_bb(knight));

  const auto bishop{(side == color::white) ? piece::w_bishop : piece::b_bishop};
  attacks |= attacks::slider_attacks<piece_type::bishop>(get_piece_bb(bishop),
                                                         get_occupied_bb());

  const auto rook{(side == color::white) ? piece::w_rook : piece::b_rook};
  attacks |= attacks::slider_attacks<piece_type::rook>(get_piece_bb(rook),
                                                       get_occupied_bb());

  const auto queen{(side == color::white) ? piece::w_queen : piece::b_queen};
  attacks |= attacks::slider_attacks<piece_type::queen>(get_piece_bb(queen),
                                                        get_occupied_bb());

  const auto king{(side == color::white) ? piece::w_king : piece::b_king};
  attacks |= attacks::king_attacks(get_piece_bb(king));

  return attacks;
}

} // namespace mpham_chess
