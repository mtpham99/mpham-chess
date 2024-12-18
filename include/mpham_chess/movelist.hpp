#pragma once

#include "mpham_chess/constants.hpp"

#include "detail/fixed_vector.hpp"

namespace mpham_chess {

class move;

using move_list = detail::fixed_vector<move, constants::max_ply>;

} // namespace mpham_chess
