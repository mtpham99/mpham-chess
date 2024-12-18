find_package(Catch2 3 REQUIRED)

add_executable(perft_tests chess_programming_wiki.cpp
                           andygrant_ethereal_chess960.cpp roce_testsuite.cpp)
target_link_libraries(perft_tests Catch2::Catch2WithMain mpham_chess_lib)
target_include_directories(perft_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

include(Catch)
catch_discover_tests(perft_tests)
