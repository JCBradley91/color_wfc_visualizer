
#include "settings.hpp"

SettingsVisualizer::SettingsVisualizer() {}

SettingsVisualizer::~SettingsVisualizer() {}

void SettingsVisualizer::Draw(AppConfig &appConfig) {

  ImGui::Begin("WFC Settings");

  ImGui::Checkbox("Manual Step", &appConfig.IsManualStep);
  if (appConfig.IsManualStep == false) {
    appConfig.IsShouldStep = true;
  } else {
    appConfig.IsShouldStep = false;
  }

  ImGui::Separator();

  static bool sizeX_isLocked = true;
  static bool sizeY_isLocked = true;
  const uint8_t colorSplitMin = 2U;
  const uint8_t colorSplitMax = 255U;
  const uint8_t realisticColorSplitMax = 20U;
  const uint16_t minSliderValue = 1;
  const uint16_t maxSliderValue = (1 << DIMENSION_DISTANCE_BIT_SHIFT) - 1;
  const uint16_t realisticMaxSliderValue = 100;

  ImGui::InputScalarN("Seed", ImGuiDataType_U64, &appConfig.Seed, 1);

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
  appConfig.ObservationMethod = obsMethods[obsMethodIndex];

  static bool enableSillySettings = false;
  ImGui::Checkbox("Allow Silly Settings", &enableSillySettings);
  if (enableSillySettings == false) {
    if (appConfig.WfcSize.pos[0] > realisticMaxSliderValue) {
      appConfig.WfcSize.pos[0] = realisticMaxSliderValue;
    }
    if (appConfig.WfcSize.pos[1] > realisticMaxSliderValue) {
      appConfig.WfcSize.pos[1] = realisticMaxSliderValue;
    }
    if (appConfig.ColorFragmentPossibilities > realisticColorSplitMax) {
      appConfig.ColorFragmentPossibilities = realisticColorSplitMax;
    }
  }

  ImGui::Checkbox("Lock X", &sizeX_isLocked);
  ImGui::SameLine();
  ImGui::BeginDisabled(sizeX_isLocked);
  ImGui::SliderScalar("Size X", ImGuiDataType_U16, &appConfig.WfcSize.pos[0],
    &minSliderValue,
    enableSillySettings ? &maxSliderValue : &realisticMaxSliderValue);
  ImGui::EndDisabled();

  ImGui::Checkbox("Lock Y", &sizeY_isLocked);
  ImGui::SameLine();
  ImGui::BeginDisabled(sizeY_isLocked);
  ImGui::SliderScalar("Size Y", ImGuiDataType_U16, &appConfig.WfcSize.pos[1],
    &minSliderValue,
    enableSillySettings ? &maxSliderValue : &realisticMaxSliderValue);
  ImGui::EndDisabled();

  ImGui::Separator();

  ImGui::SliderScalar("# of R,G,B Slices", ImGuiDataType_U8,
    &appConfig.ColorFragmentPossibilities, &colorSplitMin,
    enableSillySettings ? &colorSplitMax : &realisticColorSplitMax);
  uint64_t totalPossibleColors =
    std::pow(appConfig.ColorFragmentPossibilities + 1, 3);
  std::string totalCount(
    " - Total Possible Combinations ((slices + 1) ^ 3) : " +
    std::to_string(totalPossibleColors));
  ImGui::Text("%s", totalCount.c_str());

  ImGui::Separator();

  const uint8_t dotSizeMin = 1U;
  const uint8_t dotSizeMax = 255U;
  ImGui::SliderScalar("Dot Size", ImGuiDataType_U8,
    &appConfig.ColorTileDrawSize, &dotSizeMin, &dotSizeMax);

  ImGui::Separator();

  if (ImGui::Button("Randomize")) {
    std::mt19937_64 rand;
    std::uniform_int_distribution<uint16_t> uint16_dist(minSliderValue,
      enableSillySettings ? maxSliderValue : realisticMaxSliderValue);
    appConfig.Seed =
      std::chrono::steady_clock::now().time_since_epoch().count();
    rand.seed(appConfig.Seed);
    if (sizeX_isLocked == false) {
      appConfig.WfcSize.pos[0] = uint16_dist(rand);
    }
    if (sizeY_isLocked == false) {
      appConfig.WfcSize.pos[1] = uint16_dist(rand);
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("Apply")) {
    appConfig.IsShouldRecreateMap = true;
  }
  ImGui::SameLine();
  uint64_t memSizePerCell = totalPossibleColors * 64;
  uint64_t cellCount = appConfig.WfcSize.pos[0] * appConfig.WfcSize.pos[1];
  float memoryEstimate =
    (cellCount * memSizePerCell) / 1024.f / 1024.f / 1024.f;
  std::string estimatedSizeText(
    "Estimated Memory Size: " + std::to_string(memoryEstimate) + "Gb");
  ImGui::Text("%s", estimatedSizeText.c_str());

  if (appConfig.IsManualStep) {
    if (ImGui::Button("Step")) {
      appConfig.IsShouldStep = true;
    }
  }

  ImGui::End();
}