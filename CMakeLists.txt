cmake_minimum_required(VERSION 3.16)
project(color_wave_function_collapse_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_compile_definitions("NDEBUG")

add_subdirectory(include/walnut/Walnut)
add_subdirectory(color_wave_function_collapse)
