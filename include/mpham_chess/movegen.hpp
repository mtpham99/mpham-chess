#pragma once

#include "mpham_chess/attacks.hpp"
#include "mpham_chess/bitboard.hpp"
#include "mpham_chess/board.hpp"
#include "mpham_chess/move.hpp"
#include "mpham_chess/movelist.hpp"
#include "mpham_chess/utils.hpp"

#include <utility>

namespace mpham_chess {

// TODO : legal move gen

enum class move_gen_type { quiet, capture, pseudolegal };

template <move_gen_type mgt, bool use_side_to_move = true>
std::size_t generate_moves(const board &pos, move_list &mvlist) noexcept;

template <move_gen_type mgt, color side>
std::size_t generate_moves(const board &pos, move_list &mvlist) noexcept;

template <move_gen_type mgt, color side, piece_type pt>
  requires(pt != piece_type::no_piece_type)
std::size_t generate_moves(const board &pos, move_list &mvlist) noexcept;

template <move_gen_type mgt, color side>
std::size_t generate_pawn_moves(const board &pos, move_list &mvlist) noexcept;

template <move_gen_type mgt, color side>
std::size_t generate_king_moves(const board &pos, move_list &mvlist) noexcept;

template <move_gen_type mgt, color side, piece_type pt>
  requires((pt != piece_type::pawn) && (pt != piece_type::no_piece_type))
std::size_t generate_normal_piece_moves(const board &pos,
                                        move_list &mvlist) noexcept;

template <move_gen_type mgt, bool use_side_to_move>
std::size_t generate_moves(const board &pos, move_list &mvlist) noexcept {
  const auto side{use_side_to_move ? pos.get_side_to_move()
                                   : ~pos.get_side_to_move()};
  return (side == color::white)
             ? generate_moves<mgt, color::white>(pos, mvlist)
             : generate_moves<mgt, color::black>(pos, mvlist);
}

template <move_gen_type mgt, color side>
std::size_t generate_moves(const board &pos, move_list &mvlist) noexcept {
  const auto initial_size{mvlist.size()};
  generate_pawn_moves<mgt, side>(pos, mvlist);
  generate_king_moves<mgt, side>(pos, mvlist);
  generate_normal_piece_moves<mgt, side, piece_type::knight>(pos, mvlist);
  generate_normal_piece_moves<mgt, side, piece_type::bishop>(pos, mvlist);
  generate_normal_piece_moves<mgt, side, piece_type::rook>(pos, mvlist);
  generate_normal_piece_moves<mgt, side, piece_type::queen>(pos, mvlist);
  return mvlist.size() - initial_size;
}

template <move_gen_type mgt, color side, piece_type pt>
  requires(pt != piece_type::no_piece_type)
std::size_t generate_moves(const board &pos, move_list &mvlist) noexcept {
  if constexpr (pt == piece_type::pawn) {
    return generate_pawn_moves<mgt, side>(pos, mvlist);
  } else if constexpr (pt == piece_type::king) {
    return generate_king_moves<mgt, side>(pos, mvlist);
  } else {
    return generate_normal_piece_moves<mgt, side, pt>(pos, mvlist);
  }
}

template <move_gen_type mgt, color side>
std::size_t generate_pawn_moves(const board &pos, move_list &mvlist) noexcept {
  const auto initial_size{mvlist.size()};

  const auto pawn{utils::make_piece(side, piece_type::pawn)};
  const auto pawns_bb{pos.get_piece_bb(pawn)};

  const auto forward{(side == color::white) ? direction::N : direction::S};
  const auto forward_east{(side == color::white) ? direction::NE
                                                 : direction::SE};
  const auto forward_west{(side == color::white) ? direction::NW
                                                 : direction::SW};

  const auto rank3_bb{(side == color::white) ? constants::bb::rank_3
                                             : constants::bb::rank_6};
  const auto rank7_bb{(side == color::white) ? constants::bb::rank_7
                                             : constants::bb::rank_2};
  const auto rank7_pawns_bb{pawns_bb & rank7_bb};
  const auto no_rank7_pawns_bb{pawns_bb & ~rank7_bb};

  const auto empty_bb{pos.get_unoccupied_bb()};
  const auto enemy_bb{pos.get_color_bb(~side)};

  // single and double pushes
  if constexpr ((mgt == move_gen_type::quiet) ||
                (mgt == move_gen_type::pseudolegal)) {
    auto pushes_bb{shift<forward>(no_rank7_pawns_bb) & empty_bb};
    auto double_pushes_bb{shift<forward>(pushes_bb & rank3_bb) & empty_bb};
    while (pushes_bb) {
      const auto push_sq{pushes_bb.template pop_lsb<square>()};
      const auto pawn_sq{push_sq - std::to_underlying(forward)};
      mvlist.emplace_back(pawn_sq, push_sq, constants::move::flags::quiet);
    }
    while (double_pushes_bb) {
      const auto double_push_sq{double_pushes_bb.template pop_lsb<square>()};
      const auto pawn_sq{double_push_sq - 2 * std::to_underlying(forward)};
      mvlist.emplace_back(pawn_sq, double_push_sq,
                          constants::move::flags::double_pawn_push);
    }
  }

  // normal captures, enpassant captures, and promote captures
  if constexpr ((mgt == move_gen_type::capture) ||
                (mgt == move_gen_type::pseudolegal)) {
    auto no_promote_caps_east_bb{shift<forward_east>(no_rank7_pawns_bb) &
                                 enemy_bb};
    auto no_promote_caps_west_bb{shift<forward_west>(no_rank7_pawns_bb) &
                                 enemy_bb};
    while (no_promote_caps_east_bb) {
      const auto cap_sq{no_promote_caps_east_bb.template pop_lsb<square>()};
      const auto pawn_sq{cap_sq - std::to_underlying(forward_east)};
      mvlist.emplace_back(pawn_sq, cap_sq, constants::move::flags::capture);
    }
    while (no_promote_caps_west_bb) {
      const auto cap_sq{no_promote_caps_west_bb.template pop_lsb<square>()};
      const auto pawn_sq{cap_sq - std::to_underlying(forward_west)};
      mvlist.emplace_back(pawn_sq, cap_sq, constants::move::flags::capture);
    }

    auto promote_caps_east_bb{shift<forward_east>(rank7_pawns_bb) & enemy_bb};
    auto promote_caps_west_bb{shift<forward_west>(rank7_pawns_bb) & enemy_bb};
    while (promote_caps_east_bb) {
      const auto promote_cap_sq{
          promote_caps_east_bb.template pop_lsb<square>()};
      const auto pawn_sq{promote_cap_sq - std::to_underlying(forward_east)};
      mvlist.emplace_back(pawn_sq, promote_cap_sq,
                          constants::move::flags::promote_queen_capture);
      mvlist.emplace_back(pawn_sq, promote_cap_sq,
                          constants::move::flags::promote_rook_capture);
      mvlist.emplace_back(pawn_sq, promote_cap_sq,
                          constants::move::flags::promote_bishop_capture);
      mvlist.emplace_back(pawn_sq, promote_cap_sq,
                          constants::move::flags::promote_knight_capture);
    }
    while (promote_caps_west_bb) {
      const auto promote_cap_sq{
          promote_caps_west_bb.template pop_lsb<square>()};
      const auto pawn_sq{promote_cap_sq - std::to_underlying(forward_west)};
      mvlist.emplace_back(pawn_sq, promote_cap_sq,
                          constants::move::flags::promote_queen_capture);
      mvlist.emplace_back(pawn_sq, promote_cap_sq,
                          constants::move::flags::promote_rook_capture);
      mvlist.emplace_back(pawn_sq, promote_cap_sq,
                          constants::move::flags::promote_bishop_capture);
      mvlist.emplace_back(pawn_sq, promote_cap_sq,
                          constants::move::flags::promote_knight_capture);
    }

    const auto ep_sq{pos.get_ep_sq()};
    if (ep_sq != square::no_square) {
      auto ep_cap_pawns_bb{attacks::pawn_attacks<~side>(ep_sq) & pawns_bb};
      while (ep_cap_pawns_bb) {
        const auto pawn_sq{ep_cap_pawns_bb.template pop_lsb<square>()};
        mvlist.emplace_back(pawn_sq, ep_sq, constants::move::flags::enpassant);
      }
    }
  }

  // non-capture pomotions
  if constexpr (mgt != move_gen_type::quiet) {
    auto promote_pushes_bb{shift<forward>(rank7_pawns_bb) & empty_bb};
    while (promote_pushes_bb) {
      const auto promote_sq{promote_pushes_bb.template pop_lsb<square>()};
      const auto pawn_sq{promote_sq - std::to_underlying(forward)};
      mvlist.emplace_back(pawn_sq, promote_sq,
                          constants::move::flags::promote_queen);
      mvlist.emplace_back(pawn_sq, promote_sq,
                          constants::move::flags::promote_rook);
      mvlist.emplace_back(pawn_sq, promote_sq,
                          constants::move::flags::promote_bishop);
      mvlist.emplace_back(pawn_sq, promote_sq,
                          constants::move::flags::promote_knight);
    }
  }

  return mvlist.size() - initial_size;
}

template <move_gen_type mgt, color side>
std::size_t generate_king_moves(const board &pos, move_list &mvlist) noexcept {
  const auto initial_size{mvlist.size()};

  const auto king{utils::make_piece(side, piece_type::king)};
  const auto enemy_bb{pos.get_color_bb(~side)};
  const auto occupied_bb{pos.get_occupied_bb()};
  const auto empty_bb{pos.get_unoccupied_bb()};

  // steps
  generate_normal_piece_moves<mgt, side, piece_type::king>(pos, mvlist);

  // castling
  if constexpr ((mgt == move_gen_type::quiet) ||
                (mgt == move_gen_type::pseudolegal)) {
    if (pos.can_do_castle(side, castle_side::king)) {
      const auto king_sq{pos.get_king_castle_sq(side)};
      const auto rook_sq{pos.get_rook_castle_sq(side, castle_side::king)};
      mvlist.emplace_back(king_sq, rook_sq,
                          constants::move::flags::king_castle);
    }
    if (pos.can_do_castle(side, castle_side::queen)) {
      const auto king_sq{pos.get_king_castle_sq(side)};
      const auto rook_sq{pos.get_rook_castle_sq(side, castle_side::queen)};
      mvlist.emplace_back(king_sq, rook_sq,
                          constants::move::flags::queen_castle);
    }
  }

  return mvlist.size() - initial_size;
}

template <move_gen_type mgt, color side, piece_type pt>
  requires((pt != piece_type::pawn) && (pt != piece_type::no_piece_type))
std::size_t generate_normal_piece_moves(const board &pos,
                                        move_list &mvlist) noexcept {
  const auto initial_size{mvlist.size()};

  const auto pc{utils::make_piece(side, pt)};
  const auto enemy_bb{pos.get_color_bb(~side)};
  const auto empty_bb{pos.get_unoccupied_bb()};
  const auto occupied_bb{pos.get_occupied_bb()};

  // non-captures
  if constexpr ((mgt == move_gen_type::quiet) ||
                (mgt == move_gen_type::pseudolegal)) {
    auto pc_bb{pos.get_piece_bb(pc)};
    while (pc_bb) {
      const auto pc_sq{pc_bb.template pop_lsb<square>()};
      auto no_caps_bb{attacks::attacks<pt>(pc_sq, occupied_bb) & empty_bb};
      while (no_caps_bb) {
        const auto to_sq{no_caps_bb.template pop_lsb<square>()};
        mvlist.emplace_back(pc_sq, to_sq, constants::move::flags::quiet);
      }
    }
  }

  // captures
  if constexpr ((mgt == move_gen_type::capture) ||
                (mgt == move_gen_type::pseudolegal)) {
    auto pc_bb{pos.get_piece_bb(pc)};
    while (pc_bb) {
      const auto pc_sq{pc_bb.template pop_lsb<square>()};
      auto caps_bb{attacks::attacks<pt>(pc_sq, occupied_bb) & enemy_bb};
      while (caps_bb) {
        const auto cap_sq{caps_bb.template pop_lsb<square>()};
        mvlist.emplace_back(pc_sq, cap_sq, constants::move::flags::capture);
      }
    }
  }

  return mvlist.size() - initial_size;
}

} // namespace mpham_chess
