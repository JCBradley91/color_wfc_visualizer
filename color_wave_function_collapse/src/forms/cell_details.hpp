#ifndef CELL_DETAILS_HPP
#define CELL_DETAILS_HPP

#include "../wfc/color_tile_candidate.hpp"
#include "imgui.h"

class CellDetailsVisualizer {
public:
  CellDetailsVisualizer() = default;
  ~CellDetailsVisualizer() = default;
  void Draw(ColorTileCandidate *colorTileCandidate);
};

#endif /* CELL_DETAILS_HPP */