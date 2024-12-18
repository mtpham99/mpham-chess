#pragma once

#include "mpham_chess/board.hpp"
#include "mpham_chess/move.hpp"
#include "mpham_chess/movegen.hpp"
#include "mpham_chess/movelist.hpp"

#include <array>
#include <cstddef>
#include <ostream>
#include <utility>
#include <vector>

namespace mpham_chess {

template <std::size_t perft_depth> struct perft_result;

template <std::size_t perft_depth>
std::ostream &operator<<(std::ostream &os,
                         const perft_result<perft_depth> &result) noexcept;

template <std::size_t perft_depth> struct perft_result {
  const std::size_t _depth{perft_depth};
  std::array<std::size_t, perft_depth + 1> _nodes{1};
  std::array<std::size_t, perft_depth + 1> _captures{};
  std::array<std::size_t, perft_depth + 1> _enpassants{};
  std::array<std::size_t, perft_depth + 1> _castles{};
  std::array<std::size_t, perft_depth + 1> _promotes{};
  std::array<std::size_t, perft_depth + 1> _checks{};
  std::vector<std::pair<move, std::size_t>> _divide_nodes{};

  friend std::ostream &
  operator<< <>(std::ostream &os,
                const perft_result<perft_depth> &result) noexcept;
};

template <std::size_t perft_depth>
const perft_result<perft_depth> perft(board &pos) noexcept {
  perft_result<perft_depth> result{};
  _perft<perft_depth, true>(perft_depth, pos, result);
  return result;
}

template <std::size_t perft_depth, bool is_root>
std::size_t _perft(unsigned int depth, board &pos,
                   perft_result<perft_depth> &result) noexcept {
  if (depth == 0) {
    return 1;
  }

  std::size_t nodes{0};
  move_list mvlist{};
  generate_moves<move_gen_type::pseudolegal>(pos, mvlist);

  if constexpr (is_root) {
    auto n_legal_mvs{0};
    for (auto root_mv : mvlist) {
      pos.do_move(root_mv);
      if (!pos.is_check<false>()) {
        ++n_legal_mvs;
      }
      pos.undo_move();
    }
    result._divide_nodes.reserve(n_legal_mvs);
  }

  for (auto mv : mvlist) {
    pos.do_move(mv);

    if (!pos.is_check<false>()) {
      const auto ply{result._depth - depth + 1};
      ++(result._nodes[ply]);

      if (mv.is_capture()) {
        ++(result._captures[ply]);
      }
      if (mv.is_enpassant()) {
        ++(result._enpassants[ply]);
      }
      if (mv.is_castle()) {
        ++(result._castles[ply]);
      }
      if (mv.is_promote()) {
        ++(result._promotes[ply]);
      }
      if (pos.is_check()) {
        ++(result._checks[ply]);
      }

      auto child_nodes{_perft<perft_depth, false>(depth - 1, pos, result)};
      if constexpr (is_root) {
        result._divide_nodes.emplace_back(mv, child_nodes);
      }
      nodes += child_nodes;
    }

    pos.undo_move();
  }

  return nodes;
}

template <std::size_t perft_depth>
std::ostream &operator<<(std::ostream &os,
                         const perft_result<perft_depth> &result) noexcept {
  for (auto d{0}; d <= perft_depth; d++) {
    os << "depth (" << d << "): " << result._nodes[d] << '\n';
    os << "  captures: " << result._captures[d] << '\n';
    os << "  enpassants: " << result._enpassants[d] << '\n';
    os << "  castles: " << result._castles[d] << '\n';
    os << "  promotes: " << result._promotes[d] << '\n';
    os << "  checks: " << result._checks[d] << '\n';
  }

  os << "  divide:\n";
  for (auto [root_mv, nodes] : result._divide_nodes) {
    os << "    " << root_mv << ": " << nodes << '\n';
  }

  return os;
}

} // namespace mpham_chess
