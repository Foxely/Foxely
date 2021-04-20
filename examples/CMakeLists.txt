cmake_minimum_required(VERSION 3.16)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(lib/MeCli)
add_executable(fox main.cpp)
target_link_libraries(fox foxely mecli)