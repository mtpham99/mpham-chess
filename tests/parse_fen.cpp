#include <catch2/catch_test_macros.hpp>

#include <array>
#include <fstream>
#include <string>
#include <utility>

#include "mpham_chess/board.hpp"
#include "mpham_chess/enums.hpp"
#include "mpham_chess/move.hpp"
using namespace mpham_chess;

TEST_CASE("Standard FEN Parsing") {
  std::ifstream fen_file{"lichess_db_puzzle_fens_01dec24.txt"};
  REQUIRE(fen_file.is_open());

  board board{};
  std::string fen_str{};
  while (std::getline(fen_file, fen_str)) {
    board.load_fen(fen_str);
    CHECK(fen_str == board.to_fen());
  }
}

TEST_CASE("Basic X-FEN Castle Field Test") {
  const auto start_fen{"1Rb1kb1R/8/8/8/8/3R4/8/2R1K1R1 w KQ - 0 1"};

  const std::array<std::pair<move, std::string>, 5> move_fen_pairs{
      {{move{square::h8, square::h1, constants::move::flags::quiet},
        "1Rb1kb2/8/8/8/8/3R4/8/2R1K1RR b GQ - 1 1"},
       {move{square::f8, square::e7, constants::move::flags::quiet},
        "1Rb1k3/4b3/8/8/8/3R4/8/2R1K1RR w GQ - 2 2"},
       {move{square::d3, square::d1, constants::move::flags::quiet},
        "1Rb1k3/4b3/8/8/8/8/8/2RRK1RR b GQ - 3 2"},
       {move{square::e7, square::d8, constants::move::flags::quiet},
        "1Rbbk3/8/8/8/8/8/8/2RRK1RR w GQ - 4 3"},
       {move{square::b8, square::b1, constants::move::flags::quiet},
        "2bbk3/8/8/8/8/8/8/1RRRK1RR b GC - 5 3"}}};

  board board{start_fen};
  CHECK(start_fen == board.to_fen());
  for (const auto &[move, fen] : move_fen_pairs) {
    board.do_move(move);
    CHECK(fen == board.to_fen());
  }

  for (auto it = move_fen_pairs.rbegin(); it != move_fen_pairs.rend(); it++) {
    const auto &[_, fen] = *it;
    CHECK(fen == board.to_fen());
    board.undo_move();
  }
  CHECK(start_fen == board.to_fen());
}
