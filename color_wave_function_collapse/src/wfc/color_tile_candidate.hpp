#ifndef COLOR_TILE_CANDIDATE_HPP
#define COLOR_TILE_CANDIDATE_HPP

#include "cell_candidate_type.hpp"
#include "color_tile.hpp"
#include "position.hpp"

template <typename T> class TileCandidate : public CellCandidateType<T> {
public:
  explicit inline TileCandidate<T>(Position position)
      : CellCandidateType<T>(position) {}
  inline ~TileCandidate<T>() {}
  inline bool CanBeNeighborsWithValue(
    uint8_t adjacencyIndex, T *value) override {
    if (this->_isObserved) {
      auto diff = std::abs(*this->_value - *value);
      return diff <= 1;
    }
    if (!this->_hasBeenTouched) {
      return true;
    } else {
      auto pIT = this->_possibleValues.begin();
      while (pIT != this->_possibleValues.end()) {
        if (std::abs(**pIT - *value) <= 1) {
          return true;
        }
        pIT++;
      }
      return false;
    }
  }
};

class ColorTileCandidate : public TileCandidate<ColorTile> {
public:
  explicit inline ColorTileCandidate(Position position)
      : TileCandidate<ColorTile>(position) {}
  inline const ColorTile &GetAverageColor() {
    if (this->_isObserved) {
      return *this->_value;
    }
    return this->avgColor;
  }
  inline void OnHandledCollapsedNeighbor() override {
    // rebuild average color
    if (this->_possibleValues.size() != this->_defaultPossibilities.size()) {
      uint64_t counter = 0;
      uint64_t rSum = 0ULL, gSum = 0ULL, bSum = 0ULL;
      auto colorPossibility = this->_possibleValues.begin();
      while (colorPossibility != this->_possibleValues.end()) {
        auto possibility = *colorPossibility;
        rSum += static_cast<uint64_t>(possibility->r * possibility->r);
        gSum += static_cast<uint64_t>(possibility->g * possibility->g);
        bSum += static_cast<uint64_t>(possibility->b * possibility->b);
        counter++;
        std::advance(colorPossibility, 1);
      }
      auto alphaRatio = counter / (float)this->_defaultPossibilities.size();
      this->avgColor = ColorTile{
        .r = static_cast<int16_t>(std::sqrt(rSum / (float)counter)),
        .g = static_cast<int16_t>(std::sqrt(gSum / (float)counter)),
        .b = static_cast<int16_t>(std::sqrt(bSum / (float)counter)),
        .a = static_cast<int16_t>(255 - (alphaRatio * 255)),
      };
    }
  }

private:
  ColorTile avgColor = ColorTile{
    .r = 255,
    .g = 255,
    .b = 255,
    .a = 0,
  };
};

#endif /* COLOR_TILE_CANDIDATE_HPP */