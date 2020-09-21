
cmake_minimum_required(VERSION 3.15.3)

#Project Name
project(directory_test)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

# Executable Name
set(EXECUTABLE_NAME main_example)
set(TEST_EXECUTABLE_NAME unit_test)

set(INCLUDE ./include)
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/example)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#Bring the headers, such as Student.h into the project
include_directories(${INCLUDE})


#However, the file(GLOB...) allows for wildcard additions:
set(MAIN "Example/main.cpp")
file(GLOB SOURCES "./src/Lexer/*.cpp" "./src/Parser/*.cpp" "./src/*.cpp")
file(GLOB TEST_SOURCES "./tests/*.cpp")
file(GLOB HEADERS "./include/*.h")

add_library(GenericLexer STATIC ${SOURCES} ${HEADERS})


add_executable(${TEST_EXECUTABLE_NAME} ${TEST_SOURCES} ${SOURCES} ${HEADERS})
target_link_libraries(${TEST_EXECUTABLE_NAME} gtest gtest_main)

add_executable(define_test "tests/TestDefine.cpp" ${SOURCES} ${HEADERS})
target_link_libraries(define_test gtest gtest_main)