{ pkgs ? import <nixpkgs> {} }:
let
	version="18";
in
pkgs.mkShell.override { stdenv = pkgs."clang${version}Stdenv"; } {
	packages = [
		# clang run time
		pkgs."clang-tools_${version}"

		# build systems
		pkgs.cmake
		pkgs.ninja

		# unit testing
		pkgs.catch2_3

		# analyzers/formatters/linters
		pkgs.pre-commit
		pkgs.cppcheck
		pkgs.include-what-you-use
		pkgs.cmake-format
		pkgs.cmake-lint

		# python3 (python script used for generating test cases)
		pkgs.python3
	];

	shellHook = ''
		# pre-commit install
		${pkgs.pre-commit}/bin/pre-commit install
	'';
}
