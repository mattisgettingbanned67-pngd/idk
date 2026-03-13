cmake_minimum_required(VERSION 3.20)
project(il2cpp_offset_dumper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(il2cpp_offset_dumper
    src/main.cpp
    src/il2cpp_dumper.cpp
)

target_include_directories(il2cpp_offset_dumper PRIVATE include)

if (MSVC)
    target_compile_options(il2cpp_offset_dumper PRIVATE /W4 /permissive-)
else()
    target_compile_options(il2cpp_offset_dumper PRIVATE -Wall -Wextra -Wpedantic)
endif()
