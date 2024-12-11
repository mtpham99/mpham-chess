cmake_minimum_required(VERSION 3.20...3.30)
project(
  mpham-chess
  VERSION 0.1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(${PROJECT_SOURCE_DIR}/src)

# if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) include(CTest) endif()
# if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
# add_subdirectory(tests) endif()
