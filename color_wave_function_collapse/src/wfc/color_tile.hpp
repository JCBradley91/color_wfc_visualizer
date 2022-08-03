#ifndef COLOR_TILE_HPP
#define COLOR_TILE_HPP

#include <cmath>
#include <cstdint>

struct ColorTile {
  int16_t r = 0;
  int16_t g = 0;
  int16_t b = 0;
  int16_t a = 255;
  friend inline bool operator==(const ColorTile &lhs, const ColorTile &rhs) {
    return lhs.r == rhs.r && lhs.b == rhs.b && lhs.g == rhs.g && lhs.a == rhs.a;
  }
  friend inline bool operator!=(const ColorTile &lhs, const ColorTile &rhs) {
    return !(lhs == rhs);
  }
  friend inline int8_t operator-(const ColorTile &lhs, const ColorTile &rhs) {
    if (lhs == rhs) {
      return 0;
    }
    static uint8_t comparator = 35;
    if (std::abs(lhs.r - rhs.r) > comparator) {
      return 2;
    }
    if (std::abs(lhs.g - rhs.g) > comparator) {
      return 2;
    }
    if (std::abs(lhs.b - rhs.b) > comparator) {
      return 2;
    }
    if (std::abs(lhs.a - rhs.a) > comparator) {
      return 2;
    }
    return 1;
  }
};

#endif /* COLOR_TILE_HPP */