#include <catch2/catch_test_macros.hpp>

#include <array>

#include "mpham_chess/board.hpp"
#include "mpham_chess/perft.hpp"
using namespace mpham_chess;

TEST_CASE(
    "Perft(6): rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
  board pos{};

  const auto depth{6};
  const auto perft_res{perft<depth>(pos)};

  const std::array<std::size_t, depth + 1> nodes{
      1, 20, 400, 8'902, 197'281, 4'865'609, 119'060'324};
  const std::array<std::size_t, depth + 1> captures{0,     0,      0,        34,
                                                    1'576, 82'719, 2'812'008};
  const std::array<std::size_t, depth + 1> enpassants{0, 0,   0,    0,
                                                      0, 258, 5'248};
  const std::array<std::size_t, depth + 1> castles{0, 0, 0, 0, 0, 0, 0};
  const std::array<std::size_t, depth + 1> promotes{0, 0, 0, 0, 0, 0, 0};
  const std::array<std::size_t, depth + 1> checks{0,   0,      0,      12,
                                                  469, 27'351, 809'099};
  CHECK(perft_res._nodes == nodes);
  CHECK(perft_res._captures == captures);
  CHECK(perft_res._enpassants == enpassants);
  CHECK(perft_res._castles == castles);
  CHECK(perft_res._promotes == promotes);
  CHECK(perft_res._checks == checks);
}

TEST_CASE("Perft(5): r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w "
          "KQkq - 0 1",
          "[chess_programming_wiki][perft]") {
  board pos{
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"};

  const auto depth{5};
  const auto perft_res{perft<depth>(pos)};

  const std::array<std::size_t, depth + 1> nodes{
      1, 48, 2'039, 97'862, 4'085'603, 193'690'690};
  const std::array<std::size_t, depth + 1> captures{
      0, 8, 351, 17'102, 757'163, 35'043'416};
  const std::array<std::size_t, depth + 1> enpassants{0,  0,     1,
                                                      45, 1'929, 73'365};
  const std::array<std::size_t, depth + 1> castles{0,     2,       91,
                                                   3'162, 128'013, 4'993'637};
  const std::array<std::size_t, depth + 1> promotes{0, 0, 0, 0, 15'172, 8'392};
  const std::array<std::size_t, depth + 1> checks{0,   0,      3,
                                                  993, 25'523, 3'309'887};
  CHECK(perft_res._nodes == nodes);
  CHECK(perft_res._captures == captures);
  CHECK(perft_res._enpassants == enpassants);
  CHECK(perft_res._castles == castles);
  CHECK(perft_res._promotes == promotes);
  CHECK(perft_res._checks == checks);
}

TEST_CASE("Perft(7): 8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
          "[chess_programming_wiki][perft]") {
  board pos{"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"};

  const auto depth{7};
  const auto perft_res{perft<depth>(pos)};

  const std::array<std::size_t, depth + 1> nodes{
      1, 14, 191, 2'812, 43'238, 674'624, 11'030'083, 178'633'661};
  const std::array<std::size_t, depth + 1> captures{
      0, 1, 14, 209, 3'348, 52'051, 940'350, 14'519'036};
  const std::array<std::size_t, depth + 1> enpassants{
      0, 0, 0, 2, 123, 1'165, 33'325, 294'874};
  const std::array<std::size_t, depth + 1> castles{0, 0, 0, 0, 0, 0, 0, 0};
  const std::array<std::size_t, depth + 1> promotes{0, 0, 0,     0,
                                                    0, 0, 7'552, 140'024};
  const std::array<std::size_t, depth + 1> checks{
      0, 2, 10, 267, 1'680, 52'950, 452'473, 12'797'406};
  CHECK(perft_res._nodes == nodes);
  CHECK(perft_res._captures == captures);
  CHECK(perft_res._enpassants == enpassants);
  CHECK(perft_res._castles == castles);
  CHECK(perft_res._promotes == promotes);
  CHECK(perft_res._checks == checks);
}

TEST_CASE("Perft(6): r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w "
          "kq - 0 1",
          "[chess_programming_wiki][perft]") {
  board pos{"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"};

  const auto depth{6};
  const auto perft_res{perft<depth>(pos)};

  const std::array<std::size_t, depth + 1> nodes{
      1, 6, 264, 9'467, 422'333, 15'833'292, 706'045'033};
  const std::array<std::size_t, depth + 1> captures{
      0, 0, 87, 1'021, 131'393, 2'046'173, 210'369'132};
  const std::array<std::size_t, depth + 1> enpassants{0, 0,     0,  4,
                                                      0, 6'512, 212};
  const std::array<std::size_t, depth + 1> castles{0,     0, 6,         0,
                                                   7'795, 0, 10'882'006};
  const std::array<std::size_t, depth + 1> promotes{
      0, 0, 48, 120, 60'032, 329'464, 81'102'984};
  const std::array<std::size_t, depth + 1> checks{
      0, 0, 10, 38, 15'492, 200'568, 26'973'664};
  CHECK(perft_res._nodes == nodes);
  CHECK(perft_res._captures == captures);
  CHECK(perft_res._enpassants == enpassants);
  CHECK(perft_res._castles == castles);
  CHECK(perft_res._promotes == promotes);
  CHECK(perft_res._checks == checks);
}

TEST_CASE("Perft(5): rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
          "[chess_programming_wiki][perft]") {
  board pos{"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"};

  const auto depth{5};
  const auto perft_res{perft<depth>(pos)};

  const std::array<std::size_t, depth + 1> nodes{1,      44,        1'486,
                                                 62'379, 2'103'487, 89'941'194};
  CHECK(perft_res._nodes == nodes);
}

TEST_CASE(
    "Perft(5): r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w "
    "- - 0 10",
    "[chess_programming_wiki][perft]") {
  board pos{"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - "
            "- 0 10"};

  const auto depth{5};
  const auto perft_res{perft<depth>(pos)};

  const std::array<std::size_t, depth + 1> nodes{
      1, 46, 2'079, 89'890, 3'894'594, 164'075'551};
  CHECK(perft_res._nodes == nodes);
}
