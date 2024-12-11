# set(mpham_chess_src )

# add_library(mpham_chess_lib ${mpham_chess_src})
# target_include_directories(mpham_chess PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(main main.cpp)
# target_link_libraries(main mpham_chess_lib)
target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/include)
