#pragma once

#include "mpham_chess/enums.hpp"
#include "mpham_chess/rng.hpp"
#include "mpham_chess/utils.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <utility>

namespace mpham_chess {

using zobrist_hash = std::uint64_t;
template <std::size_t n> using zobrist_hashes = std::array<zobrist_hash, n>;

namespace zobrist {

[[nodiscard]] inline zobrist_hash get_color_hash() noexcept;
[[nodiscard]] inline zobrist_hash get_castle_hash(castle_rights cr) noexcept;
[[nodiscard]] inline zobrist_hash get_enpassant_hash(square sq) noexcept;
[[nodiscard]] inline zobrist_hash get_square_piece_hash(square sq,
                                                        piece pc) noexcept;

namespace hashes {

inline const zobrist_hash color{rng::main_rng.generate()};

inline const zobrist_hashes<constants::n_castle_states> castle{
    rng::main_rng.generate_n<constants::n_castle_states>()};

inline const zobrist_hashes<constants::n_files> enpassant{
    rng::main_rng.generate_n<constants::n_files>()};

inline const zobrist_hashes<constants::n_squares * constants::n_pieces>
    square_piece{
        rng::main_rng.generate_n<constants::n_squares * constants::n_pieces>()};

} // namespace hashes

inline zobrist_hash get_color_hash() noexcept { return hashes::color; }

inline zobrist_hash get_castle_hash(castle_rights cr) noexcept {
  return hashes::castle[std::to_underlying(cr)];
}

inline zobrist_hash get_enpassant_hash(square sq) noexcept {
  assert(sq != square::no_square);
  const auto file{utils::file_of(sq)};
  return hashes::enpassant[std::to_underlying(file)];
}

inline zobrist_hash get_square_piece_hash(square sq, piece pc) noexcept {
  assert(sq != square::no_square && pc != piece::no_piece);
  const auto sq_ind{std::to_underlying(sq)};
  const auto pc_ind{std::to_underlying(pc)};
  return hashes::square_piece[sq_ind * constants::n_pieces + pc_ind];
}

} // namespace zobrist

} // namespace mpham_chess
