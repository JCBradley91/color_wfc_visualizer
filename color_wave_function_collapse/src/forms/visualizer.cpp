
#include "./visualizer.hpp"
#include "imgui.h"

ColorWFCVisualizer::ColorWFCVisualizer() : _counterIndex(s_counter++) {}

ColorWFCVisualizer::~ColorWFCVisualizer() {}

void ColorWFCVisualizer::Draw(VisualizationType displayType,
  std::vector<std::vector<ColorTileCandidate *>> &grid, uint8_t tileDrawSize,
  uint32_t totalColorPossibilities) {

  std::string title(
    "WFC Block Visualizer #" + std::to_string(this->_counterIndex));
  ImGui::Begin(title.c_str());

  auto drawList = ImGui::GetWindowDrawList();

  for (size_t k = 0; k < grid.at(0).size(); k++) {
    for (size_t i = 0; i < grid.size(); i++) {
      auto ctc = grid.at(i).at(k);
      if (ctc == nullptr) {
        continue;
      }
      ColorTile *ct = ctc->GetValue();
      auto entropyRatio =
        grid.at(i).at(k)->GetEntropy() / (float)totalColorPossibilities;
      ImVec2 p = ImGui::GetCursorScreenPos();
      ImVec2 markerMin =
        ImVec2(p.x + (i * tileDrawSize), p.y + (k * tileDrawSize));
      ImVec2 markerMax =
        ImVec2(markerMin.x + tileDrawSize + 1.0f, markerMin.y + tileDrawSize);
      ImVec4 color;
      switch (displayType) {
      case VisualizationType::Color:
        if (ct != nullptr) {
          color =
            ImVec4(ct->r / 255.f, ct->g / 255.f, ct->b / 255.f, ct->a / 255.f);
        }
        break;
      case VisualizationType::Entropy:
        color = ImVec4(entropyRatio, entropyRatio, entropyRatio, 1.f);
        break;
      case VisualizationType::None:
        break;
      }
      drawList->AddRectFilled(markerMin, markerMax, ImColor(color));
    }
  }

  ImGui::End();
}
