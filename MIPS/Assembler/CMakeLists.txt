cmake_minimum_required(VERSION 3.20)

project(mips-assembler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "-fsanitize=address")
endif()

find_package(fmt CONFIG REQUIRED)
find_package(FLEX)


FLEX_TARGET(scanner src/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.cc
            DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.h)


add_executable(compiler
  src/main.cc
  ${FLEX_scanner_OUTPUTS}
)

target_link_libraries(compiler PRIVATE fmt::fmt)
target_include_directories(compiler PRIVATE src/ ${CMAKE_CURRENT_BINARY_DIR})
