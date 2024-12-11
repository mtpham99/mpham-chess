{ pkgs ? import <nixpkgs> {} }:
let
	clang-version="18";
in
pkgs.mkShell.override { stdenv = pkgs."clang${clang-version}Stdenv"; } {
	packages = [
		# clang run time
		pkgs."clang-tools_${clang-version}"

		# build systems
		pkgs.cmake
		pkgs.ninja

		# unit testing
		pkgs.catch2_3

		# analyzers/formatters/linters
		pkgs.pre-commit
		pkgs.cppcheck
		pkgs.cpplint
		pkgs.include-what-you-use
		pkgs.cmake-format
		pkgs.cmake-lint
	];

	shellHook = ''
		# pre-commit install
		${pkgs.pre-commit}/bin/pre-commit install
	'';
}
