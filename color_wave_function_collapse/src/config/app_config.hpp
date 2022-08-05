#ifndef APP_CONFIG_HPP
#define APP_CONFIG_HPP

#include "../wfc/wave_function_collapse.hpp"
#include <cstdint>

struct AppConfig {
  uint64_t Seed = 0ULL;
  bool IsManualStep = true;
  bool IsShouldStep = false;
  bool IsShouldRecreateMap = false;
  uint8_t ColorTileDrawSize = 6U;
  uint8_t ColorFragmentPossibilities = 9;
  NextObservationMethod ObservationMethod =
    NextObservationMethod::LowestEntropyRandom;
  Position WfcSize = {100, 100};
};

#endif /* APP_CONFIG_HPP */
