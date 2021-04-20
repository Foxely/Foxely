cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 14)

project(Foxely VERSION 1.0.0 LANGUAGES CXX)

include_directories(
    "include"
)

file(GLOB_RECURSE SRCS "src/*.cpp")

set(LEXER_LIB "lib/Lexer/liblixer.a")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(lib)

add_library(foxely STATIC ${SRCS})
target_link_libraries(foxely ${CMAKE_DL_LIBS} lexer)

add_subdirectory(examples)