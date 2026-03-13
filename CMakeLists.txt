cmake_minimum_required(VERSION 3.20)
project(il2cpp_mod_base LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(il2cpp_mod_base SHARED
    src/dllmain.cpp
    src/il2cpp_base.cpp
)

target_include_directories(il2cpp_mod_base PRIVATE include)

target_compile_definitions(il2cpp_mod_base PRIVATE
    UNICODE
    _UNICODE
)

if (MSVC)
    target_compile_options(il2cpp_mod_base PRIVATE /W4 /permissive-)
else()
    target_compile_options(il2cpp_mod_base PRIVATE -Wall -Wextra -Wpedantic)
endif()
