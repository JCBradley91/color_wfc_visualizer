#ifndef COLOR_TILE_CANDIDATE_HPP
#define COLOR_TILE_CANDIDATE_HPP

#include "cell_candidate_type.hpp"
#include "color_tile.hpp"
#include "position.hpp"

template <typename T> class TileCandidate : public CellCandidateType<T> {
public:
  inline TileCandidate<T>(
    Position position, std::vector<T> defaultPossibilities)
      : CellCandidateType<T>(position, defaultPossibilities) {}
  inline ~TileCandidate<T>() {}
  inline bool CanBeNeighborsWithValue(
    uint8_t adjacencyIndex, T *value) override {
    if (this->_isObserved) {
      auto diff = std::abs(*this->_value - *value);
      return diff <= 1;
    }
    auto pIT = this->_possibleValues.begin();
    while (pIT != this->_possibleValues.end()) {
      if (std::abs(**pIT - *value) <= 1) {
        return true;
      }
      pIT++;
    }
    return false;
  }
};

class ColorTileCandidate : public TileCandidate<ColorTile> {
public:
  inline ColorTileCandidate(
    Position position, std::vector<ColorTile> defaultPossibilities)
      : TileCandidate<ColorTile>(position, defaultPossibilities) {}
};

#endif /* COLOR_TILE_CANDIDATE_HPP */