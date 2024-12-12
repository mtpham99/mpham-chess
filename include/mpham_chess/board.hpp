#pragma once

#include "mpham_chess/bitboard.hpp"
#include "mpham_chess/constants.hpp"
#include "mpham_chess/enums.hpp"
#include "mpham_chess/move.hpp"
#include "mpham_chess/zobrist.hpp"

#include "detail/fixed_vector.hpp"

#include <array>
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

  [[nodiscard]] bool is_sq_empty(square sq) const noexcept;

  void do_move(move move) noexcept;
  void undo_move() noexcept;

private:
  void move_piece(square from, square to) noexcept;
  void place_piece(square sq, piece pc) noexcept;
  void remove_piece(square sq) noexcept;

  [[nodiscard]] std::string castle_fen_field() const noexcept;
};

} // namespace mpham_chess
