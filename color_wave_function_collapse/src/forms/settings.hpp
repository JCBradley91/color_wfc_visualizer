#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "../config/app_config.hpp"
#include "../wfc/wave_function_collapse.hpp"
#include "imgui.h"
#include <chrono>

class SettingsVisualizer {
public:
  SettingsVisualizer();
  ~SettingsVisualizer();
  void Draw(AppConfig &appConfig);

public:
};

#endif /* SETTINGS_HPP */