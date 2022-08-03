cmake_minimum_required(VERSION 3.16)

project(color_wave_function_collapse)
set(CMAKE_CXX_STANDARD 20)

add_executable(colorwavefunctioncollapse
  src/color_wave_function_collapse.cpp
)

target_link_libraries(colorwavefunctioncollapse walnut)
