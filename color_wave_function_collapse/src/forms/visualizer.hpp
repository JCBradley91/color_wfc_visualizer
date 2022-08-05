
#include "./../wfc/color_tile_candidate.hpp"
#include <atomic>
#include <cstdint>
#include <vector>

enum struct VisualizationType {
  None,
  Color,
  Entropy,
  Average,
};

class ColorWFCVisualizer {
public:
  ColorWFCVisualizer();
  ~ColorWFCVisualizer();
  void Draw(VisualizationType displayType,
    std::vector<std::vector<ColorTileCandidate *>> &grid, uint8_t tileDrawSize,
    ColorTileCandidate *&hoveredCandidate,
    uint32_t totalColorPossibilities = 1);

private:
  inline static std::atomic<uint32_t> s_counter = 0;
  uint32_t _counterIndex = 0;
};
