#include "mpham_chess/board.hpp"

#include "mpham_chess/bitboard.hpp"
#include "mpham_chess/constants.hpp"
#include "mpham_chess/enums.hpp"
#include "mpham_chess/utils.hpp"
#include "mpham_chess/zobrist.hpp"

#include <cassert>
#include <cctype>
#include <charconv>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

namespace mpham_chess {

board::board(std::string_view fen, bool use_shredder_fen) noexcept
    : _use_shredder_fen{use_shredder_fen} {
  load_fen(fen);
}

void board::load_fen(std::string_view fen) noexcept {
  for (auto &bb : _piece_bbs) {
    bb = bitboard{0x0000000000000000};
  }
  for (auto &bb : _color_bbs) {
    bb = bitboard{0x0000000000000000};
  }
  for (auto &pc : _piece_list) {
    pc = piece::no_piece;
  }
  _hash = 0;
  _state_hist.clear();
  _move_hist.clear();
  _castle_king_sqs = {square::no_square, square::no_square};
  _castle_rook_sqs = {{{square::no_square, square::no_square},
                       {square::no_square, square::no_square}}};

  const std::vector<std::string_view> fen_fields{
      utils::str::split_string<" ">(fen)};
  assert(fen_fields.size() == constants::n_fen_fields);
  const auto pos_field{fen_fields[0]};
  const auto color_field{fen_fields[1]};
  const auto castle_field{fen_fields[2]};
  const auto ep_field{fen_fields[3]};
  const auto rule50_field{fen_fields[4]};
  const auto movenum_field{fen_fields[5]};

  bitboard fen_sq_bb{square::h8};
  const std::vector<std::string_view> pos_rank_fields{
      utils::str::split_string<"/">(pos_field)};
  assert(pos_rank_fields.size() == constants::n_ranks);
  for (auto rank_field : pos_rank_fields) {
    for (auto it{rank_field.rbegin()}; it != rank_field.rend(); it++) {
      if (std::isdigit(*it)) {
        const auto n_empty_sqs{*it - '0'};
        fen_sq_bb >>= n_empty_sqs;
      } else if (std::isalpha(*it)) {
        const auto pc{utils::char_to_piece(*it)};
        const auto c{utils::color_of(pc)};

        _piece_bbs[std::to_underlying(pc)] |= fen_sq_bb;
        _color_bbs[std::to_underlying(c)] |= fen_sq_bb;
        _piece_list[std::to_underlying(square{fen_sq_bb})] = pc;
        if (pc != piece::no_piece) {
          _hash ^= zobrist::get_square_piece_hash(square{fen_sq_bb}, pc);
        }

        fen_sq_bb >>= 1;
      } else {
        assert(false);
      }
    }
  }
  _castle_king_sqs = {
      square{_piece_bbs[std::to_underlying(
          utils::make_piece(color::white, piece_type::king))]},
      square{_piece_bbs[std::to_underlying(
          utils::make_piece(color::black, piece_type::king))]},
  };

  if (color_field == "w") {
    _side_to_move = color::white;
  } else {
    _side_to_move = color::black;
    _hash ^= zobrist::get_color_hash();
  }

  auto rook_file_of_castle = [this](color c, castle_side cs) -> file {
    // TODO : replace with inbetween_sqs to ensure rook on correct side
    const auto rook{utils::make_piece(c, piece_type::rook)};
    const auto rooks_bb{_piece_bbs[std::to_underlying(rook)]};
    const auto castle_rank{(c == color::white) ? rank::rank_1 : rank::rank_8};
    const auto rook_mask{rooks_bb & bitboard{castle_rank}};
    const auto castle_rook_sq{cs == castle_side::king
                                  ? rook_mask.get_msb<square>()
                                  : rook_mask.get_lsb<square>()};
    return utils::file_of(castle_rook_sq);
  };

  auto side_of_castle_rook = [this](color c, file rook_file) -> castle_side {
    const auto castle_rank{(c == color::white) ? rank::rank_1 : rank::rank_8};
    const auto rook_sq{utils::make_square(rook_file, castle_rank)};
    const auto king{utils::make_piece(c, piece_type::king)};
    const auto king_bb{_piece_bbs[std::to_underlying(king)]};
    const square king_sq{king_bb & bitboard{castle_rank}};
    return (rook_sq < king_sq) ? castle_side::queen : castle_side::king;
  };

  _castle = castle_rights::no_castle;
  if (castle_field != "-") {
    for (auto castle_char : castle_field) {
      const auto castle_color{(('A' <= castle_char) && (castle_char <= 'Z'))
                                  ? color::white
                                  : color::black};
      castle_char = std::toupper(castle_char);

      file castle_rook_file;
      castle_side castle_rook_side;
      if (castle_char == 'K') {
        castle_rook_file = rook_file_of_castle(castle_color, castle_side::king);
        castle_rook_side = castle_side::king;
      } else if (castle_char == 'Q') {
        castle_rook_file =
            rook_file_of_castle(castle_color, castle_side::queen);
        castle_rook_side = castle_side::queen;
      } else {
        assert('A' <= castle_char && castle_char <= 'H');
        castle_rook_file = file{castle_char - 'A'};
        castle_rook_side = side_of_castle_rook(castle_color, castle_rook_file);
      }

      const auto castle_rook_sq{utils::make_square(
          castle_rook_file,
          (castle_color == color::white) ? rank::rank_1 : rank::rank_8)};
      _castle_rook_sqs[std::to_underlying(castle_color)]
                      [std::to_underlying(castle_rook_side)] = castle_rook_sq;
      _castle |= utils::make_castle_rights(castle_color, castle_rook_side);
    }
  }
  _hash ^= zobrist::get_castle_hash(_castle);

  _ep_sq = utils::str_to_sq(ep_field);
  if (_ep_sq != square::no_square) {
    _hash ^= zobrist::get_enpassant_hash(_ep_sq);
  }

  std::from_chars(rule50_field.begin(), rule50_field.end(), _rule50);
  std::from_chars(movenum_field.begin(), movenum_field.end(), _start_movenum);
}

std::string board::to_fen() const noexcept {
  std::string fen{""};

  auto n_empty_sqs{0};
  auto write_empty_sqs = [&n_empty_sqs, &fen]() -> void {
    if (n_empty_sqs > 0) {
      fen += std::to_string(n_empty_sqs);
      n_empty_sqs = 0;
    }
  };

  const auto sqs_fen_order = [] consteval {
    std::array<square, constants::n_squares> sqs_fen_order{};
    for (auto sq_ind{0}; sq_ind < constants::n_squares; sq_ind++) {
      sqs_fen_order[sq_ind] = utils::flip<flip_type::vert>(square{sq_ind});
    }
    return sqs_fen_order;
  }();
  for (auto fen_sq : sqs_fen_order) {
    const auto pc{get_piece_on_sq(fen_sq)};

    if (pc == piece::no_piece) {
      n_empty_sqs++;
    } else {
      write_empty_sqs();
      fen += utils::piece_to_char(pc);
    }

    if (utils::file_of(fen_sq) == file::file_h) {
      write_empty_sqs();
      if (fen_sq != square::h1) {
        fen += '/';
      }
    }
  }

  fen += (_side_to_move == color::white) ? " w " : " b ";
  fen += castle_fen_field() + ' ';
  fen += utils::sq_to_str(_ep_sq) + ' ';
  fen += std::to_string(_rule50) + ' ';
  fen += std::to_string(get_movenum());

  return fen;
}

bitboard board::get_piece_bb(piece pc) const noexcept {
  assert(pc != piece::no_piece);
  return _piece_bbs[std::to_underlying(pc)];
}

bitboard board::get_color_bb(color c) const noexcept {
  return _color_bbs[std::to_underlying(c)];
}

bitboard board::get_occupied_bb() const noexcept {
  return _color_bbs[std::to_underlying(color::white)] |
         _color_bbs[std::to_underlying(color::black)];
}

bitboard board::get_unoccupied_bb() const noexcept {
  return ~get_occupied_bb();
}

piece board::get_piece_on_sq(square sq) const noexcept {
  assert(sq != square::no_square);
  return _piece_list[std::to_underlying(sq)];
}

color board::get_side_to_move() const noexcept { return _side_to_move; }

castle_rights board::get_castle() const noexcept { return _castle; }

square board::get_ep_sq() const noexcept { return _ep_sq; }

unsigned int board::get_rule50() const noexcept { return _rule50; }

unsigned int board::get_movenum() const noexcept {
  const auto start_color{(get_ply() % 2 == 0) ? _side_to_move : ~_side_to_move};
  auto total_ply{utils::full_to_ply(_start_movenum, start_color) + get_ply()};
  return utils::ply_to_full(total_ply);
}

unsigned int board::get_ply() const noexcept { return _move_hist.size(); }

zobrist_hash board::get_hash() const noexcept { return _hash; }

void board::do_move(move move) noexcept {
  const auto side{_side_to_move}, enemy{~side};
  const auto from{move.get_from_square()};
  const auto to{move.get_to_square()};
  const auto pc{get_piece_on_sq(from)};
  const auto cap_pc{move.is_enpassant()
                        ? utils::make_piece(~side, piece_type::pawn)
                        : get_piece_on_sq(to)};
  assert((pc != piece::no_piece) && (utils::color_of(pc) == side));

  if (move.is_capture()) {
    const auto backward{(side == color::white) ? direction::S : direction::N};
    const auto cap_sq{move.is_enpassant() ? (to + std::to_underlying(backward))
                                          : to};
    assert(get_piece_on_sq(cap_sq) == cap_pc);
    assert(utils::color_of(cap_pc) == enemy);

    remove_piece(cap_sq);

    const auto enemy_cr{(enemy == color::white) ? castle_rights::w_both
                                                : castle_rights::b_both};
    const auto enemy_can_castle{!!(_castle & enemy_cr)};
    const auto is_cap_pc_rook{cap_pc ==
                              utils::make_piece(enemy, piece_type::rook)};
    if (enemy_can_castle && is_cap_pc_rook) {
      for (auto cs : {castle_side::king, castle_side::queen}) {
        if (_castle_rook_sqs[std::to_underlying(enemy)]
                            [std::to_underlying(cs)] == cap_sq) {
          _castle &= ~utils::make_castle_rights(enemy, cs);
        }
      }
    }
  }

  if (move.is_promote()) {
    assert(pc == utils::make_piece(side, piece_type::pawn));

    const auto promote_pc{
        utils::make_piece(side, move.get_promote_piece_type())};
    remove_piece(from);
    place_piece(to, promote_pc);
  } else if (move.is_castle()) {
    assert(pc == utils::make_piece(side, piece_type::king));
    assert(cap_pc == utils::make_piece(side, piece_type::rook));

    const auto is_king_castle{move.is_king_castle()};
    assert(is_king_castle ==
           (std::to_underlying(from) < std::to_underlying(to)));

    const auto rook_from{to};
    const auto rook_to{
        is_king_castle ? ((side == color::white) ? square::f1 : square::f8)
                       : ((side == color::white) ? square::d1 : square::d8)};
    const auto king_from{from};
    const auto king_to{
        is_king_castle ? ((side == color::white) ? square::g1 : square::g8)
                       : ((side == color::white) ? square::c1 : square::c8)};

    move_piece(rook_from, rook_to);
    move_piece(king_from, king_to);
  } else {
    move_piece(from, to);
  }

  _state_hist.emplace_back(_hash, _rule50, _ep_sq, cap_pc, _castle);

  _side_to_move = ~_side_to_move;
  _hash ^= zobrist::get_color_hash();
  _rule50 = (move.is_capture() || (utils::piecetype_of(pc) == piece_type::pawn))
                ? 0
                : _rule50 + 1;

  if (_ep_sq != square::no_square) {
    _hash ^= zobrist::get_enpassant_hash(_ep_sq);
  }
  _ep_sq = square::no_square;
  if (move.is_double_pawn_push()) {
    assert(pc == utils::make_piece(side, piece_type::pawn));

    const auto enemy_pawn{utils::make_piece(enemy, piece_type::pawn)};
    const auto neighbor_bb{shift<direction::E>(bitboard{to}) |
                           shift<direction::W>(bitboard{to})};
    const auto enemy_pawn_bb{_piece_bbs[std::to_underlying(enemy_pawn)]};

    if (neighbor_bb & enemy_pawn_bb) {
      const auto backward{(side == color::white) ? direction::S : direction::N};
      _ep_sq = to + std::to_underlying(backward);
      _hash ^= zobrist::get_enpassant_hash(_ep_sq);
    }
  }

  const auto side_cr{(side == color::white) ? castle_rights::w_both
                                            : castle_rights::b_both};
  const auto can_castle{!!(_castle & side_cr)};
  if (can_castle && (pc == utils::make_piece(side, piece_type::king))) {
    _castle &= ~side_cr;
  } else if (can_castle && (pc == utils::make_piece(side, piece_type::rook))) {
    for (auto cs : {castle_side::king, castle_side::queen}) {
      if (_castle_rook_sqs[std::to_underlying(side)][std::to_underlying(cs)] ==
          from) {
        _castle &= ~utils::make_castle_rights(side, cs);
      }
    }
  }

  _move_hist.emplace_back(move);
}

void board::undo_move() noexcept {
  assert(_move_hist.size() > 0 && _state_hist.size() > 0);

  const auto prev_state{_state_hist.back()};
  _state_hist.pop_back();

  _side_to_move = ~_side_to_move;
  _hash = prev_state._hash;
  _rule50 = prev_state._rule50;
  _ep_sq = prev_state._ep_sq;
  _castle = prev_state._castle;

  const auto prev_move{_move_hist.back()};
  _move_hist.pop_back();

  const auto side{_side_to_move};
  const auto from{prev_move.get_from_square()};
  const auto to{prev_move.get_to_square()};
  const auto cap_pc{prev_state._cap_pc};

  if (prev_move.is_promote()) {
    remove_piece(to);
    place_piece(from, utils::make_piece(side, piece_type::pawn));
  } else if (prev_move.is_castle()) {
    const auto is_king_castle{prev_move.is_king_castle()};
    const auto cs{is_king_castle ? castle_side::king : castle_side::queen};

    const auto rook_from{
        _castle_rook_sqs[std::to_underlying(side)][std::to_underlying(cs)]};
    const auto rook_to{
        is_king_castle ? ((side == color::white) ? square::f1 : square::f8)
                       : ((side == color::white) ? square::d1 : square::d8)};
    const auto king_from{_castle_king_sqs[std::to_underlying(side)]};
    const auto king_to{
        is_king_castle ? ((side == color::white) ? square::g1 : square::g8)
                       : ((side == color::white) ? square::c1 : square::c8)};

    move_piece(rook_to, rook_from);
    move_piece(king_to, king_from);
  } else {
    move_piece(to, from);
  }

  if (prev_move.is_capture()) {
    const auto backward{(side == color::white) ? direction::S : direction::N};
    const auto cap_sq{
        prev_move.is_enpassant() ? (to + std::to_underlying(backward)) : to};
    place_piece(cap_sq, cap_pc);
  }
}

void board::move_piece(square from, square to) noexcept {
  assert(from != to && from != square::no_square && to != square::no_square);
  const auto pc{get_piece_on_sq(from)};
  [[maybe_unused]] const auto to_pc{get_piece_on_sq(to)};
  assert(pc != piece::no_piece && to_pc == piece::no_piece);

  const auto c{utils::color_of(pc)};
  const bitboard fromto_bb{from, to};

  _color_bbs[std::to_underlying(c)] ^= fromto_bb;
  _piece_bbs[std::to_underlying(pc)] ^= fromto_bb;
  _hash ^= zobrist::get_square_piece_hash(from, pc) ^
           zobrist::get_square_piece_hash(to, pc);

  _piece_list[std::to_underlying(from)] = piece::no_piece;
  _piece_list[std::to_underlying(to)] = pc;
}

void board::place_piece(square sq, piece pc) noexcept {
  assert(sq != square::no_square && pc != piece::no_piece);
  assert(get_piece_on_sq(sq) == piece::no_piece);

  const auto c{utils::color_of(pc)};
  const bitboard sq_bb{sq};

  _color_bbs[std::to_underlying(c)] ^= sq_bb;
  _piece_bbs[std::to_underlying(pc)] ^= sq_bb;
  _hash ^= zobrist::get_square_piece_hash(sq, pc);

  _piece_list[std::to_underlying(sq)] = pc;
}

void board::remove_piece(square sq) noexcept {
  assert(sq != square::no_square);
  const auto pc{get_piece_on_sq(sq)};
  assert(pc != piece::no_piece);

  const auto c{utils::color_of(pc)};
  const bitboard sq_bb{sq};

  _color_bbs[std::to_underlying(c)] ^= sq_bb;
  _piece_bbs[std::to_underlying(pc)] ^= sq_bb;
  _hash ^= zobrist::get_square_piece_hash(sq, pc);

  _piece_list[std::to_underlying(sq)] = piece::no_piece;
}

std::string board::castle_fen_field() const noexcept {
  if (!_castle) {
    return "-";
  }

  auto get_castle_char = [this](color c, castle_side cs) -> char {
    const auto c_ind{std::to_underlying(c)};
    const auto cs_ind{std::to_underlying(cs)};

    char castle_char;
    if (_use_shredder_fen) {
      const auto rook_sq{_castle_rook_sqs[c_ind][cs_ind]};
      const auto rook_file{utils::file_of(rook_sq)};
      castle_char = 'A' + std::to_underlying(rook_file);
    } else {
      const auto castle_rank{(c == color::white) ? rank::rank_1 : rank::rank_8};
      const auto rook{utils::make_piece(c, piece_type::rook)};
      const auto rook_bb{_piece_bbs[std::to_underlying(rook)]};

      // TODO : replace with inbetween_sqs to ensure rook on correct side
      const auto castle_rank_rooks_bb{rook_bb & bitboard{castle_rank}};
      const auto outer_rook_sq{(cs == castle_side::king)
                                   ? castle_rank_rooks_bb.get_msb<square>()
                                   : castle_rank_rooks_bb.get_lsb<square>()};
      if (outer_rook_sq != _castle_rook_sqs[c_ind][cs_ind]) {
        const auto castle_rook_file{
            utils::file_of(_castle_rook_sqs[c_ind][cs_ind])};
        castle_char = 'A' + std::to_underlying(castle_rook_file);
      } else {
        castle_char = (cs == castle_side::king) ? 'K' : 'Q';
      }
    }

    return (c == color::white) ? std::toupper(castle_char)
                               : std::tolower(castle_char);
  };

  std::string fen{""};
  if (!!(_castle & castle_rights::w_king)) {
    fen += get_castle_char(color::white, castle_side::king);
  }
  if (!!(_castle & castle_rights::w_queen)) {
    fen += get_castle_char(color::white, castle_side::queen);
  }
  if (!!(_castle & castle_rights::b_king)) {
    fen += get_castle_char(color::black, castle_side::king);
  }
  if (!!(_castle & castle_rights::b_queen)) {
    fen += get_castle_char(color::black, castle_side::queen);
  }
  return fen;
}

std::ostream &operator<<(std::ostream &os, const board &board) noexcept {
  const auto side_str{
      ((board.get_side_to_move() == color::white) ? "white" : "black")};
  const auto move_num{board.get_movenum()};

  os << "Move #" << move_num << '\n';
  os << "Rule50: " << board.get_rule50() << '\n';
  os << "Color: " << side_str << '\n';

  os << "  ===================\n";
  const auto sqs_fen_order = [] consteval {
    std::array<square, constants::n_squares> sqs_fen_order{};
    for (auto sq_ind{0}; sq_ind < constants::n_squares; sq_ind++) {
      sqs_fen_order[sq_ind] = utils::flip<flip_type::vert>(square{sq_ind});
    }
    return sqs_fen_order;
  }();
  for (auto sq : sqs_fen_order) {
    const auto sq_file{utils::file_of(sq)};
    const auto sq_rank{utils::rank_of(sq)};
    const bitboard sq_bb{sq};

    if (sq_file == file::file_a) {
      const char file_label = '1' + std::to_underlying(sq_rank);
      os << file_label << " | ";
    }

    const auto pc{board.get_piece_on_sq(sq)};
    const auto pc_char{(pc == piece::no_piece) ? '.'
                                               : utils::piece_to_char(pc)};
    os << pc_char << ' ';

    if (sq_file == file::file_h) {
      os << "|\n";
    }
  }
  os << "  ===================\n";
  os << "    A B C D E F G H\n";

  const auto ep_sq{board.get_ep_sq()};
  const auto ep_str{(ep_sq == square::no_square) ? "-"
                                                 : utils::sq_to_str(ep_sq)};
  os << "FEN: \"" << board.to_fen() << "\"\n";
  os << "Enpassant: " << ep_str << '\n';
  os << "Castle: " << board.castle_fen_field() << '\n';
  os << "Hash: " << board.get_hash() << '\n';

  return os;
}

} // namespace mpham_chess
