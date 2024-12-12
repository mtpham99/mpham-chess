# mpham-chess (under construction)

Re-writing [my chess engine](https://github.com/mtpham99/MPChess).

Under construction: still working on board and movegen...

# Build Instructions

Tested on Linux (NixOS) environment using:

- CMake v3.30.4
- Ninja v1.12.1
- Clang v18.1.8
- Catch2 v3.7.1

## Build Requirements

- C++23 supported compiler
- Catch2 v3.x (optional)

## Instructions (CMake)

```
# clone repo
git clone https://github.com/mtpham99/mpham-chess.git

# create build dir and configure
cd mpham-chess
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF ..

# build
make -j$(nproc)

# run
./src/main
```

# Implemented / In-Progress Features

## General

- [ ] [Chess960 (Fischer Random Chess)](https://en.wikipedia.org/wiki/Chess960) Compatibility
- [x] [X-FEN](https://en.wikipedia.org/wiki/X-FEN) & [Shredder-FEN](https://www.chessprogramming.org/Forsyth-Edwards_Notation#Shredder-FEN) Compatibility

## Engine Backend

- [ ] [Bitboards](https://www.chessprogramming.org/Bitboards)
- [ ] [Magic Bitboards](https://www.chessprogramming.org/Magic_Bitboards)
- [ ] [Incremental Updates](https://www.chessprogramming.org/Incremental_Updates)

## Search

- [ ] [Minimax Search](https://en.wikipedia.org/wiki/Minimax)
- [ ] [Alpha-Beta Pruning](https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning)
- [ ] [Transposition Table](https://en.wikipedia.org/wiki/Transposition_table)

# Resources

1. [The Chess Programming Wiki](https://www.chessprogramming.org/Main_Page)
2. [Good explanation on "Magic Bitboards"](https://analog-hors.github.io/site/magic-bitboards/)
