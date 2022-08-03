#ifndef POSITION_HPP
#define POSITION_HPP

#include <cstddef>
#include <cstdint>

// clang-format off
#define DIMENSION_COUNT 4ULL
#define EMPTY_DIMENSION { 0U, 0U, 0U, 0U, }
#define EMPTY_ADJACENCIES { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, }
#define DIMENSION_DISTANCE_BIT_SHIFT 12ULL
// clang-format on

struct Position {
  uint16_t pos[DIMENSION_COUNT] = EMPTY_DIMENSION;
  inline static uint64_t ToHash(Position position) {
    uint64_t returnValue = 0ULL;
    for (size_t i = 0; i < DIMENSION_COUNT; i++) {
      uint64_t p = static_cast<uint64_t>(position.pos[i]);
      uint64_t shiftCount =
        (DIMENSION_COUNT - 1ULL - i) * DIMENSION_DISTANCE_BIT_SHIFT;
      uint64_t shiftedValue = p << shiftCount;
      returnValue += shiftedValue;
    }
    return returnValue;
  }
};

#endif /* POSITION_HPP */