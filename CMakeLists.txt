cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

set(NAME a.out)

find_package(OpenGL REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS})
include_directories("src")
include_directories("src/vendor")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Static library directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "-g -Wall")

add_library(glad src/vendor/glad/glad.c)
target_link_libraries(glad dl)

add_executable(${NAME} src/main.cpp)

target_link_libraries(${NAME} ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} glad)