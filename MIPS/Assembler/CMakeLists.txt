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

include(FetchContent)

FetchContent_Declare(backward
        GIT_REPOSITORY https://github.com/bombela/backward-cpp
        GIT_TAG v1.6)
FetchContent_MakeAvailable(backward)

add_executable(assembler
  src/main.cc
  ${FLEX_scanner_OUTPUTS}
  src/mips_instr.h
  src/mips_instr.cc
  src/assembler_context.h
  src/assembler_context.cc
  src/block.h
  src/block.cc
  src/token.h
  src/token.cc
  ${BACKWARD_ENABLE}
)

add_backward(assembler)

target_link_libraries(assembler PRIVATE fmt::fmt)
target_include_directories(assembler PRIVATE src/ ${CMAKE_CURRENT_BINARY_DIR})


