#pragma once

#include "mpham_chess/bitboard.hpp"
#include "mpham_chess/constants.hpp"
#include "mpham_chess/enums.hpp"
#include "mpham_chess/rng.hpp"
#include "mpham_chess/utils.hpp"

#include <utility>

namespace mpham_chess::attacks {

template <typename dtype>
using two_dim_square_table =
    std::array<std::array<dtype, constants::n_squares>, constants::n_squares>;
using attack_table = std::array<bitboard, constants::n_squares>;
using pawn_attack_table = std::array<attack_table, constants::n_colors>;

template <piece_type pt, typename sq_or_bb>
  requires((pt != piece_type::pawn) && (pt != piece_type::no_piece_type) &&
           (std::same_as<sq_or_bb, bitboard> || std::same_as<sq_or_bb, square>))
[[nodiscard]] constexpr bitboard
attacks(sq_or_bb sq_bb, bitboard blockers = constants::bb::empty) noexcept;

template <color c>
[[nodiscard]] constexpr bitboard pawn_attacks(bitboard pawns) noexcept;
template <color c>
[[nodiscard]] constexpr bitboard pawn_attacks(square pawn) noexcept;

[[nodiscard]] constexpr bitboard knight_attacks(bitboard knights) noexcept;
[[nodiscard]] constexpr bitboard knight_attacks(square knight) noexcept;

[[nodiscard]] constexpr bitboard king_attacks(bitboard kings) noexcept;
[[nodiscard]] constexpr bitboard king_attacks(square king) noexcept;

template <direction dir>
  requires ray_dir<dir>
[[nodiscard]] constexpr bitboard
ray_attacks(bitboard origins,
            bitboard blockers = constants::bb::empty) noexcept;

template <piece_type pt>
  requires slider_pt<pt>
[[nodiscard]] constexpr bitboard
slider_attacks(bitboard sliders,
               bitboard blockers = constants::bb::empty) noexcept;

template <piece_type pt>
  requires slider_pt<pt>
[[nodiscard]] inline bitboard
slider_attacks(square slider,
               bitboard blockers = constants::bb::empty) noexcept;

[[nodiscard]] constexpr bitboard inbetween_squares(square sq_1,
                                                   square sq_2) noexcept;

[[nodiscard]] constexpr unsigned int square_distances(square sq_1,
                                                      square sq_2) noexcept;

namespace magics {
// good explanation of magic bitboards
// https://analog-hors.github.io/site/magic-bitboards
// https://www.chessprogramming.org/Magic_Bitboards

// A `magic` number/bitboard is a number that can map
// (square, blockers) => attack bitboard.
// The mapping/hashing function is: (magic_bb * blocker_bb) >> key_size
//
// Relevant blockers are blockers located on squares that are attacked by the
// slider on an empty board. I.e. a rook on a1 doesn't care about a piece
// on b2, because the rook only attacks the a-file and 1-rank;
//
// Note that multiple blocker sets can map to the same attack map.
// E.g. rook on a1 with a blocker on e1 means any combo of blockers
// located `behind e1` (i.e. f1, g1, or h1) will map to the same
// attack bitboard.
//
// The cardinality of the blocker space is 2^(number of relevant blocker
// squares). E.g. A rook on a1 attacks [a2-a7] and [b1-g1] => cardinality = 2
// ^ 12. Note a8 and h1 are not included because nothing can be `behind` the
// edge.
//
// However, because multiple blocker sets can map to the same attack bitboard
// (i.e. constructive collisions), it is in principle possible to use a hash
// table with a size smaller than the cardinality of the blocker space. This
// is the purpose of the `key_shift`. It determines the size of the table/key
// (`key_size`) needed to map all blocker sets to the attack map.
//
// The `table_offset` is an index offset used to find the correct
// starting index for a given square in the final/combined attack table.
// Note that different squares have a different blocker space cardinality.
// E.g. cardinality(rook on a1) = 2^12 vs cardinality(rook on e4) = 2^10
// `Plain magic bitboards` assumes every square has the same (worst case)
// blocker space cardinality (i.e. 2^12 for rooks regardless of square).
//
// In order to reduce the final/combined table size, one needs to use
// `Fancy magic bitboards`, i.e. each square should only have an attack
// table with a max size of the blocker space cardinality, but can be
// even smaller if optimized magics (constructive collisions) are found.
// `Table_offset` is used to index the correct square table when using
// `fancy magic bitboards`.

struct magic_entry {
  bitboard _relevant_blockers{constants::bb::empty};
  bitboard _magic{constants::bb::empty};
  unsigned int _table_offset{0};
  unsigned int _key_shift{0};

  [[nodiscard]] inline std::size_t
  get_attack_table_key(bitboard blockers) const noexcept {
    blockers &= _relevant_blockers;
    const auto hash_key{(_magic * blockers) >> _key_shift};
    return std::size_t{hash_key + _table_offset};
  }
};
using magics_table = std::array<magic_entry, constants::n_squares>;

template <piece_type pt>
  requires slider_pt<pt>
[[nodiscard]] constexpr bitboard relevant_blocker_mask(square sq) noexcept;

template <piece_type pt>
  requires(pt == piece_type::bishop || pt == piece_type::rook)
[[nodiscard]] magic_entry
find_magic(square sq, rng::xorshift64 &rng = rng::main_rng) noexcept;

template <piece_type pt>
  requires(pt == piece_type::bishop || pt == piece_type::rook)
[[nodiscard]] magic_entry get_slider_magic(square sq) noexcept;

template <piece_type pt>
  requires slider_pt<pt>
constexpr bitboard relevant_blocker_mask(square sq) noexcept {
  assert(sq != square::no_square);

  const auto file{utils::file_of(sq)};
  const auto rank{utils::rank_of(sq)};

  const bitboard file_bb{file};
  const bitboard rank_bb{rank};
  const bitboard sq_bb{sq};

  const auto attacks_bb{slider_attacks<pt>(sq_bb)};
  const auto irrelevant_blockers =
      (((constants::bb::file_a | constants::bb::file_h) & ~file_bb) |
       ((constants::bb::rank_1 | constants::bb::rank_8) & ~rank_bb));

  return attacks_bb & ~irrelevant_blockers;
}

template <piece_type pt>
  requires(pt == piece_type::bishop || pt == piece_type::rook)
magic_entry find_magic(square sq, rng::xorshift64 &rng) noexcept {
  assert(sq != square::no_square);

  const bitboard sq_bb{sq};
  const auto relevant_blockers{relevant_blocker_mask<pt>(sq)};
  const auto n_blockers{relevant_blockers.bit_count()};
  const auto max_tbl_size{1 << n_blockers};
  const auto key_shift{UINT64_WIDTH - n_blockers};

  const auto blk_atk_subsets = [&] {
    std::vector<std::pair<bitboard, bitboard>> blk_atk_subsets{};
    blk_atk_subsets.reserve(max_tbl_size);

    auto block_subset{constants::bb::empty};
    do {
      // Carry-Rippler subet traversal
      // iterating over all possible subsets of `relevant_blockers`
      block_subset = (block_subset - relevant_blockers) & relevant_blockers;

      const auto attack_subset{slider_attacks<pt>(sq_bb, block_subset)};
      blk_atk_subsets.emplace_back(block_subset, attack_subset);
    } while (block_subset != constants::bb::empty);

    return blk_atk_subsets;
  }();

  auto hash_key_fn = [](bitboard magic, bitboard blockers,
                        unsigned int key_shift) -> std::size_t {
    return std::size_t{(magic * blockers) >> key_shift};
  };

  std::vector<bitboard> mapped_atks(max_tbl_size, constants::bb::empty);
  while (true) {
    for (auto &bb : mapped_atks) {
      bb = constants::bb::empty;
    }

    const bitboard try_magic{rng.generate<rng::xorshift64::rng_type::sparse>()};
    bool is_valid_magic{true};
    for (const auto &[block_subset, attack_subset] : blk_atk_subsets) {
      const auto hash_key{hash_key_fn(try_magic, block_subset, key_shift)};

      if (mapped_atks[hash_key] == constants::bb::empty) {
        // unmapped attack bitboard
        mapped_atks[hash_key] = attack_subset;
        continue;
      } else if (mapped_atks[hash_key] == attack_subset) {
        // constructive collision:
        // multiple blocker subsets with the same
        // attack bitboard map to the same index
        continue;
      } else {
        // destructive collision:
        // multiple blocker subsets with different
        // attack bitboards map to the same index
        is_valid_magic = false;
        break;
      }
    }

    if (is_valid_magic) {
      return magic_entry{._relevant_blockers = relevant_blockers,
                         ._magic = try_magic,
                         ._key_shift = key_shift};
    }
  }
}

template <piece_type pt>
  requires(pt == piece_type::bishop || pt == piece_type::rook)
magic_entry get_slider_magic(square sq) noexcept {
  assert(sq != square::no_square);

  static const auto magic_tbl = [] {
    magics_table magic_tbl{};

    auto offset{0};
    for (auto sq_ind{0}; sq_ind < constants::n_squares; sq_ind++) {
      const square sq{sq_ind};

      auto slider_magic_entry{find_magic<pt>(sq)};

      const auto key_size{UINT64_WIDTH - slider_magic_entry._key_shift};
      const auto sq_atk_tbl_size{1 << key_size};

      slider_magic_entry._table_offset = offset;
      offset += sq_atk_tbl_size;

      magic_tbl[sq_ind] = slider_magic_entry;
    }

    return magic_tbl;
  }();

  return magic_tbl[std::to_underlying(sq)];
}

} // namespace magics

template <color c> constexpr bitboard pawn_attacks(bitboard pawns) noexcept {
  if constexpr (c == color::white) {
    return shift<direction::NE>(pawns) | shift<direction::NW>(pawns);
  } else {
    return shift<direction::SE>(pawns) | shift<direction::SW>(pawns);
  }
}

template <color c> constexpr bitboard pawn_attacks(square pawn) noexcept {
  assert(pawn != square::no_square);

  static constexpr auto pawn_atk_tbl = [] consteval {
    pawn_attack_table pawn_atk_tbl{};

    for (auto sq_ind{0}; sq_ind < constants::n_squares; sq_ind++) {
      const square sq{sq_ind};
      const bitboard sq_bb{sq};

      pawn_atk_tbl[std::to_underlying(color::white)][sq_ind] =
          pawn_attacks<color::white>(sq_bb);
      pawn_atk_tbl[std::to_underlying(color::black)][sq_ind] =
          pawn_attacks<color::black>(sq_bb);
    }

    return pawn_atk_tbl;
  }();

  return pawn_atk_tbl[std::to_underlying(c)][std::to_underlying(pawn)];
}

constexpr bitboard knight_attacks(bitboard knights) noexcept {
  // TODO : 8-shift vs shift + mirror
  return shift<direction::NNE>(knights) | shift<direction::NEE>(knights) |
         shift<direction::SEE>(knights) | shift<direction::SSE>(knights) |
         shift<direction::SSW>(knights) | shift<direction::SWW>(knights) |
         shift<direction::NWW>(knights) | shift<direction::NNW>(knights);
}

constexpr bitboard knight_attacks(square knight) noexcept {
  assert(knight != square::no_square);

  static constexpr auto knight_atk_tbl = [] consteval {
    attack_table knight_atk_tbl{};

    for (auto sq_ind{0}; sq_ind < constants::n_squares; sq_ind++) {
      const square sq{sq_ind};
      const bitboard sq_bb{sq};

      knight_atk_tbl[sq_ind] = knight_attacks(sq_bb);
    }

    return knight_atk_tbl;
  }();

  return knight_atk_tbl[std::to_underlying(knight)];
}

constexpr bitboard king_attacks(bitboard kings) noexcept {
  // TODO : 8-shift vs shift + mirror
  return shift<direction::N>(kings) | shift<direction::E>(kings) |
         shift<direction::S>(kings) | shift<direction::W>(kings) |
         shift<direction::NE>(kings) | shift<direction::SE>(kings) |
         shift<direction::SW>(kings) | shift<direction::NW>(kings);
}

constexpr bitboard king_attacks(square king) noexcept {
  assert(king != square::no_square);

  static constexpr auto king_atk_tbl = [] consteval {
    attack_table king_atk_tbl{};

    for (auto sq_ind{0}; sq_ind < constants::n_squares; sq_ind++) {
      const square sq{sq_ind};
      const bitboard sq_bb{sq};

      king_atk_tbl[sq_ind] = king_attacks(sq_bb);
    }

    return king_atk_tbl;
  }();

  return king_atk_tbl[std::to_underlying(king)];
}

template <direction dir>
  requires ray_dir<dir>
constexpr bitboard ray_attacks(bitboard origins, bitboard blockers) noexcept {
  origins.fill<dir>(blockers);
  return origins.shift<dir>();
}

template <piece_type pt>
  requires slider_pt<pt>
constexpr bitboard slider_attacks(bitboard sliders,
                                  bitboard blockers) noexcept {
  if constexpr (pt == piece_type::bishop) {
    return ray_attacks<direction::NE>(sliders, blockers) |
           ray_attacks<direction::SE>(sliders, blockers) |
           ray_attacks<direction::SW>(sliders, blockers) |
           ray_attacks<direction::NW>(sliders, blockers);
  } else if constexpr (pt == piece_type::rook) {
    return ray_attacks<direction::N>(sliders, blockers) |
           ray_attacks<direction::E>(sliders, blockers) |
           ray_attacks<direction::S>(sliders, blockers) |
           ray_attacks<direction::W>(sliders, blockers);
  } else if constexpr (pt == piece_type::queen) {
    return slider_attacks<piece_type::bishop>(sliders, blockers) |
           slider_attacks<piece_type::rook>(sliders, blockers);
  }
}

template <>
inline bitboard slider_attacks<piece_type::queen>(square slider,
                                                  bitboard blockers) noexcept {
  return slider_attacks<piece_type::bishop>(slider, blockers) |
         slider_attacks<piece_type::rook>(slider, blockers);
}

template <piece_type pt>
  requires slider_pt<pt>
inline bitboard slider_attacks(square slider, bitboard blockers) noexcept {
  assert(slider != square::no_square);

  static const auto slider_atk_tbl = [] {
    std::vector<bitboard> slider_atk_tbl{};

    auto tbl_size{0};
    for (auto sq_ind{0}; sq_ind < constants::n_squares; sq_ind++) {
      const square sq{sq_ind};
      const bitboard sq_bb{sq};

      const auto magic{magics::get_slider_magic<pt>(sq)};
      const auto sq_key_size{UINT64_WIDTH - magic._key_shift};
      const auto sq_atk_tbl_size{1 << sq_key_size};

      tbl_size += sq_atk_tbl_size;
    }
    slider_atk_tbl.resize(tbl_size);

    for (auto sq_ind{0}; sq_ind < constants::n_squares; sq_ind++) {
      const square sq{sq_ind};
      const bitboard sq_bb{sq};

      const auto magic{magics::get_slider_magic<pt>(sq)};
      const auto relevant_blockers{magic._relevant_blockers};

      auto block_subset{constants::bb::empty};
      do {
        // Carry-Rippler subet traversal
        // iterating over all possible subsets of `relevant_blockers`
        block_subset = (block_subset - relevant_blockers) & relevant_blockers;

        const auto key{magic.get_attack_table_key(block_subset)};
        slider_atk_tbl[key] = slider_attacks<pt>(sq_bb, block_subset);
      } while (!block_subset.is_empty());
    }

    assert(slider_atk_tbl.size() == slider_atk_tbl.capacity());
    return slider_atk_tbl;
  }();

  const auto magic{magics::get_slider_magic<pt>(slider)};
  const auto key{magic.get_attack_table_key(blockers)};
  return slider_atk_tbl[key];
}

constexpr bitboard inbetween_squares(square sq_1, square sq_2) noexcept {
  assert(sq_1 != square::no_square && sq_2 != square::no_square);

  static constexpr auto inbetween_squares = [] consteval {
    two_dim_square_table<bitboard> inbetween_squares{};

    for (auto sq_ind_1{0}; sq_ind_1 < constants::n_squares; sq_ind_1++) {
      const square sq_1{sq_ind_1};
      const bitboard sq_bb_1{sq_1};

      for (auto sq_ind_2{sq_ind_1}; sq_ind_2 < constants::n_squares;
           sq_ind_2++) {
        const square sq_2{sq_ind_2};
        const bitboard sq_bb_2{sq_2};

        const auto bishop_1{
            slider_attacks<piece_type::bishop>(sq_bb_1, sq_bb_2)};
        const auto rook_1{slider_attacks<piece_type::rook>(sq_bb_1, sq_bb_2)};
        const auto bishop_2{
            slider_attacks<piece_type::bishop>(sq_bb_2, sq_bb_1)};
        const auto rook_2{slider_attacks<piece_type::rook>(sq_bb_2, sq_bb_1)};

        const bool is_same_diag{bishop_1 & sq_bb_2};
        const auto diag_overlap{(is_same_diag) ? (bishop_1 & bishop_2)
                                               : constants::bb::empty};

        const bool is_same_rowcol{rook_1 & sq_bb_2};
        const auto rowcol_overlap{(is_same_rowcol) ? (rook_1 & rook_2)
                                                   : constants::bb::empty};

        inbetween_squares[sq_ind_1][sq_ind_2] = diag_overlap | rowcol_overlap;
        inbetween_squares[sq_ind_2][sq_ind_1] =
            inbetween_squares[sq_ind_1][sq_ind_2];
      }
    }

    return inbetween_squares;
  }();

  return inbetween_squares[std::to_underlying(sq_1)][std::to_underlying(sq_2)];
}

constexpr unsigned int square_distances(square sq_1, square sq_2) noexcept {
  assert(sq_1 != square::no_square && sq_2 != square::no_square);

  static constexpr auto sq_dist_tbl = [] consteval {
    two_dim_square_table<unsigned int> square_distances{};

    for (auto sq_ind_1{0}; sq_ind_1 < constants::n_squares; sq_ind_1++) {
      const auto file_ind_1{sq_ind_1 % constants::n_ranks};
      const auto rank_ind_1{sq_ind_1 / constants::n_files};

      for (auto sq_ind_2{sq_ind_1}; sq_ind_2 < constants::n_squares;
           sq_ind_2++) {
        const auto file_ind_2{sq_ind_2 % constants::n_ranks};
        const auto rank_ind_2{sq_ind_2 / constants::n_files};

        const auto file_dist{std::max(file_ind_1, file_ind_2) -
                             std::min(file_ind_1, file_ind_2)};
        const auto rank_dist{std::max(rank_ind_1, rank_ind_2) -
                             std::min(rank_ind_1, rank_ind_2)};

        square_distances[sq_ind_1][sq_ind_2] = std::max(file_dist, rank_dist);
        square_distances[sq_ind_2][sq_ind_1] =
            square_distances[sq_ind_1][sq_ind_2];
      }
    }

    return square_distances;
  }();

  return sq_dist_tbl[std::to_underlying(sq_1)][std::to_underlying(sq_2)];
}

template <piece_type pt, typename sq_or_bb>
  requires((pt != piece_type::pawn) && (pt != piece_type::no_piece_type) &&
           (std::same_as<sq_or_bb, bitboard> || std::same_as<sq_or_bb, square>))
constexpr bitboard attacks(sq_or_bb sq_bb, bitboard blockers) noexcept {
  // TODO : compare bitboard compute vs iterative table lookup
  if constexpr (pt == piece_type::knight) {
    return knight_attacks(sq_bb);
  } else if constexpr (pt == piece_type::king) {
    // return king_attacks(sq_bb);
    return king_attacks(square{sq_bb});
  } else if constexpr (slider_pt<pt>) {
    return slider_attacks<pt>(sq_bb, blockers);
  }
}

} // namespace mpham_chess::attacks
