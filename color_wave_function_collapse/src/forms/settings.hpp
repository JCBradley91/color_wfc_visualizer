#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "../wfc/wave_function_collapse.hpp"
#include "imgui.h"
#include <chrono>
#include <cstdint>

class SettingsVisualizer {
public:
  SettingsVisualizer();
  ~SettingsVisualizer();
  void Draw();

private:
  uint64_t _seed = 0ULL;
  bool _isManualStep = true;
  bool _isShouldStep = false;
  bool _isShouldRecreateMap = false;
  uint8_t _colorTileDrawSize = 10U;
  NextObservationMethod _nextObservationMethod =
    NextObservationMethod::LowestEntropyRandom;
  Position _wfcSize;
};

#endif /* SETTINGS_HPP */