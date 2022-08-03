
#include "settings.hpp"

SettingsVisualizer::SettingsVisualizer() {}

SettingsVisualizer::~SettingsVisualizer() {}

void SettingsVisualizer::Draw() {

  ImGui::Begin("WFC Settings");

  ImGui::Checkbox("Manual Step", &this->_isManualStep);
  if (this->_isManualStep == false) {
    this->_isShouldStep = true;
  } else {
    this->_isShouldStep = false;
  }

  ImGui::Separator();

  static bool sizeX_isLocked = true;
  static bool sizeY_isLocked = true;
  uint16_t minSliderValue = 0;
  uint16_t maxSliderValue = (1 << DIMENSION_DISTANCE_BIT_SHIFT) - 1;

  ImGui::InputScalarN("Seed", ImGuiDataType_U64, &this->_seed, 1);

  const NextObservationMethod obsMethods[] = {
    NextObservationMethod::LowestEntropyRandom,
    NextObservationMethod::LowestEntropySequential,
    NextObservationMethod::LowestEntropyReverseSequential,
    NextObservationMethod::HighestEntropyRandom,
    NextObservationMethod::HighestEntropySequential,
    NextObservationMethod::HighestEntropyReverseSequential,
    NextObservationMethod::Random,
  };
  const char *obsMethodStrings[] = {
    "LowestEntropyRandom",
    "LowestEntropySequential",
    "LowestEntropyReverseSequential",
    "HighestEntropyRandom",
    "HighestEntropySequential",
    "HighestEntropyReverseSequential",
    "Random",
  };
  static int obsMethodIndex = 0;
  ImGui::Combo("Next Observe Method", &obsMethodIndex, obsMethodStrings,
    IM_ARRAYSIZE(obsMethods));
  this->_nextObservationMethod = obsMethods[obsMethodIndex];

  ImGui::Checkbox("Lock X", &sizeX_isLocked);
  ImGui::SameLine();
  ImGui::BeginDisabled(sizeX_isLocked);
  ImGui::SliderScalar("sizeX", ImGuiDataType_U16, &this->_wfcSize.pos[0],
    &minSliderValue, &maxSliderValue);
  ImGui::EndDisabled();

  ImGui::Checkbox("Lock Y", &sizeY_isLocked);
  ImGui::SameLine();
  ImGui::BeginDisabled(sizeY_isLocked);
  ImGui::SliderScalar("sizeY", ImGuiDataType_U16, &this->_wfcSize.pos[1],
    &minSliderValue, &maxSliderValue);
  ImGui::EndDisabled();

  ImGui::Separator();

  const uint8_t min = 0U;
  const uint8_t max = 255U;
  ImGui::SliderScalar(
    "dotSize", ImGuiDataType_U8, &this->_colorTileDrawSize, &min, &max);

  ImGui::Separator();

  if (ImGui::Button("Randomize")) {
    std::mt19937_64 rand;
    std::uniform_int_distribution<uint16_t> uint16_dist(
      minSliderValue, maxSliderValue);
    this->_seed = std::chrono::steady_clock::now().time_since_epoch().count();
    rand.seed(this->_seed);
    if (sizeX_isLocked == false) {
      this->_wfcSize.pos[0] = uint16_dist(rand);
    }
    if (sizeY_isLocked == false) {
      this->_wfcSize.pos[1] = uint16_dist(rand);
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Apply")) {
    this->_isShouldRecreateMap = true;
  }

  if (this->_isManualStep) {
    if (ImGui::Button("Step")) {
      _isShouldStep = true;
    }
  }

  ImGui::End();
}