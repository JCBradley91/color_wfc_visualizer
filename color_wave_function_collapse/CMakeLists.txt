cmake_minimum_required(VERSION 3.16)

project(color_wave_function_collapse)
set(CMAKE_CXX_STANDARD 20)

add_executable(colorwavefunctioncollapse
  src/color_wave_function_collapse.cpp
  src/config/app_config.hpp
  src/forms/settings.cpp
  src/forms/settings.hpp
  src/forms/visualizer.cpp
  src/forms/visualizer.hpp
  src/lib/thread_pool.cpp
  src/lib/thread_pool.hpp
  src/wfc/cell_candidate_type.hpp
  src/wfc/cell_candidate.hpp
  src/wfc/color_tile_candidate.hpp
  src/wfc/color_tile.hpp
  src/wfc/position.hpp
  src/wfc/wave_function_collapse.cpp
  src/wfc/wave_function_collapse.hpp
)

target_link_libraries(colorwavefunctioncollapse walnut)
