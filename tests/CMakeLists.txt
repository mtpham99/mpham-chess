find_package(Catch2 3 REQUIRED)

file(COPY "./lichess_db_puzzle_fens_01dec24.txt" DESTINATION ".")
add_executable(tests parse_fen.cpp)
target_link_libraries(tests Catch2::Catch2WithMain mpham_chess_lib)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

include(Catch)
catch_discover_tests(tests)
