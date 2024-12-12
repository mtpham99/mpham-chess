#!/usr/bin/env sh

set -eu

PROJECT_DIR="$(dirname "$(realpath "$0")")"
BUILD_DIR="./build"
CMAKE_GENERATOR="Ninja"
CMAKE_BUILD_TYPE="Debug"
CMAKE_CXX_COMPILER="$(which clang++)"
CMAKE_CXX_FLAGS=""
CMAKE_COMPILE_COMMANDS="ON"
BUILD_TESTING="ON"

if ! command -v cmake >/dev/null 2>&1; then
	echo "Error: cmake was not found in PATH!"
	exit 1
fi

cmake \
	-S "$PROJECT_DIR"\
	-G "$CMAKE_GENERATOR" \
	-B "$BUILD_DIR" \
	-DBUILD_TESTING="$BUILD_TESTING" \
	-DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
	-DCMAKE_CXX_COMPILER="$CMAKE_CXX_COMPILER" \
	-DCMAKE_CXX_FLAGS="$CMAKE_CXX_FLAGS" \
	-DCMAKE_EXPORT_COMPILE_COMMANDS="$CMAKE_COMPILE_COMMANDS"
