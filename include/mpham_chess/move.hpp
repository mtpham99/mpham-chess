#pragma once

#include <cstdint>
#include <ostream>

namespace mpham_chess {

enum class square;
enum class piece_type;

using move_flags = std::uint8_t;

namespace constants::move {

inline constexpr int from_sq_bit_index{0};
inline constexpr int to_sq_bit_index{6};
inline constexpr int flags_bit_index{12};

namespace flags {

inline constexpr move_flags quiet{0b0000};
inline constexpr move_flags capture{0b0100};
inline constexpr move_flags double_pawn_push{0b0001};
inline constexpr move_flags king_castle{0b0010};
inline constexpr move_flags queen_castle{0b0011};
inline constexpr move_flags enpassant{0b0101};

inline constexpr move_flags promote_knight{0b1000};
inline constexpr move_flags promote_bishop{0b1001};
inline constexpr move_flags promote_rook{0b1010};
inline constexpr move_flags promote_queen{0b1011};

inline constexpr move_flags promote_knight_capture{0b1100};
inline constexpr move_flags promote_bishop_capture{0b1101};
inline constexpr move_flags promote_rook_capture{0b1110};
inline constexpr move_flags promote_queen_capture{0b1111};

inline constexpr move_flags invalid_flag_1{0b0110};
inline constexpr move_flags invalid_flag_2{0b0111};

} // namespace flags

namespace masks {

inline constexpr std::uint16_t from_sq{0b111111 << 0}; // 0000 0000 0011 111}1
inline constexpr std::uint16_t to_sq{0b111111 << 6};   // 0000 1111 1100 000}0
inline constexpr std::uint16_t flags{0b1111 << 12};    // 1111 0000 0000 000}0
inline constexpr std::uint16_t capture{1 << 14};       // 0100 0000 0000 000}0
inline constexpr std::uint16_t promote{1 << 15};       // 1000 0000 0000 000}0

} // namespace masks

} // namespace constants::move

class move {
  // move (16 bits total)
  // bits 0-5:   from square is number between 0..63
  // bits 6-11:  to square is number between 0..63
  // bits 12-15: flags
  // bit  12:    promotion flag
  // bit  13:    capture flag
  // bit  14/15: special (promotion piece, castle, enpassant)
  //
  // special flags (4 bits)
  // 1.  quiet move                (0000) =  0
  // 2.  double pawn push          (0001) =  1
  // 3.  short castle              (0010) =  2
  // 4.  long castle               (0011) =  3
  // 5.  captures                  (0100) =  4
  // 6.  enpassant                 (0101) =  5
  // 7.  promote knight            (1000) =  8
  // 8.  promote bishop            (1001) =  9
  // 9.  promote rook              (1010) = 10
  // 10. promote queen             (1011) = 11
  // 11. promote knight w/ capture (1100) = 12
  // 12. promote bishop w/ capture (1101) = 13
  // 13. promote rook   w/ capture (1110) = 14
  // 14. promote queen  w/ capture (1111) = 15
  //
  // 15. invalid                   (0110) =  6
  // 16. invalid                   (0111) =  7

private:
  std::uint16_t _data{0};

public:
  [[nodiscard]] move() noexcept = default;
  [[nodiscard]] explicit move(std::uint16_t data) noexcept;
  [[nodiscard]] explicit move(square from, square to,
                              move_flags flags) noexcept;

  [[nodiscard]] move_flags get_flags() const noexcept;
  [[nodiscard]] square get_from_square() const noexcept;
  [[nodiscard]] square get_to_square() const noexcept;
  [[nodiscard]] piece_type get_promote_piece_type() const noexcept;

  [[nodiscard]] bool is_quiet() const noexcept;
  [[nodiscard]] bool is_capture() const noexcept;
  [[nodiscard]] bool is_king_castle() const noexcept;
  [[nodiscard]] bool is_queen_castle() const noexcept;
  [[nodiscard]] bool is_castle() const noexcept;
  [[nodiscard]] bool is_promote() const noexcept;
  [[nodiscard]] bool is_enpassant() const noexcept;
  [[nodiscard]] bool is_double_pawn_push() const noexcept;

  friend std::ostream &operator<<(std::ostream &os, const move &mv) noexcept;

private:
  [[nodiscard]] bool is_valid_flags() const noexcept;
};

std::ostream &operator<<(std::ostream &os, const move &mv) noexcept;

} // namespace mpham_chess
