cmake_minimum_required(VERSION 4.1)



project(Compiler C CXX)
include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_LINKER_TYPE MOLD)
set(BUILD_SHARED_LIBS ON)

set(CMAKE_CXX_STANDARD 23)
add_executable(compiler src/main.cpp)

include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG        v0.9.5 # Use the latest version
)
FetchContent_MakeAvailable(magic_enum)
set("MAGIC_ENUM_LIBRARIES" magic_enum::magic_enum)
target_include_directories(compiler
   PRIVATE ${CMAKE_SOURCE_DIR}/header)
target_link_libraries(compiler
   PRIVATE
    clang-cpp
    LLVM  clang-cpp
    ${llvm_libs}   
    ${MAGIC_ENUM_LIBRARIES}

)