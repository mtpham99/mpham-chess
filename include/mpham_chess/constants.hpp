#pragma once

namespace mpham_chess::constants {

inline constexpr int n_colors{2};

inline constexpr int n_files{8};
inline constexpr int n_ranks{8};
inline constexpr int n_squares{64};

inline constexpr int n_piece_types{6};
inline constexpr int n_pieces{12};

inline constexpr int n_castle_sides{2};
inline constexpr int n_castle_states{16};

inline constexpr int n_fen_fields{6};
inline constexpr auto start_pos_fen{
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};

inline constexpr int max_ply{1024};

} // namespace mpham_chess::constants
