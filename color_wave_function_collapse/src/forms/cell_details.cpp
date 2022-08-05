
#include "./cell_details.hpp"

void CellDetailsVisualizer::Draw(ColorTileCandidate *colorTileCandidate) {

  ImGui::Begin("Color Tile Candidate Details");

  if (colorTileCandidate != nullptr) {
    Position pos = colorTileCandidate->GetPosition();
    uint64_t entropy = colorTileCandidate->GetEntropy();
    ColorTile *colorTile = colorTileCandidate->GetValue();
    ColorTile averageColor = colorTileCandidate->GetAverageColor();

    std::string x("X: " + std::to_string(pos.pos[0]));
    std::string y("Y: " + std::to_string(pos.pos[1]));
    ImGui::Text("%s", x.c_str());
    ImGui::Text("%s", y.c_str());

    std::string remainingPossibilities(
      "Remaining Possibilities: " + std::to_string(entropy));
    ImGui::Text("%s", remainingPossibilities.c_str());

    ImGui::Separator();
    std::string colorTitle, r, g, b, a;
    if (colorTile != nullptr) {
      colorTitle = std::string("Collapsed:");
      r = std::string("R: " + std::to_string(colorTile->r));
      g = std::string("G: " + std::to_string(colorTile->g));
      b = std::string("B: " + std::to_string(colorTile->b));
      a = std::string("A: " + std::to_string(colorTile->a));
    } else {
      colorTitle = std::string("Average:");
      r = std::string("R: " + std::to_string(averageColor.r));
      g = std::string("G: " + std::to_string(averageColor.g));
      b = std::string("B: " + std::to_string(averageColor.b));
      a = std::string("A: " + std::to_string(averageColor.a));
    }

    ImGui::Text("%s", colorTitle.c_str());
    ImGui::Text("%s", r.c_str());
    ImGui::Text("%s", g.c_str());
    ImGui::Text("%s", b.c_str());
    ImGui::Text("%s", a.c_str());
  }

  ImGui::End();
}