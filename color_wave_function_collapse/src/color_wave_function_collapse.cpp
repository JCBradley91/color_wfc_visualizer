
#include "Walnut/Application.h"
#include "Walnut/Image.h"
#include "config/app_config.hpp"
#include "forms/settings.hpp"
#include "forms/visualizer.hpp"
#include "wfc/color_tile_candidate.hpp"

class WaveFunctionCollapseLayer : public Walnut::Layer {
public:
  virtual void OnUIRender() override {
    _settingsForm.Draw(_appConfig);
    std::vector<std::vector<ColorTileCandidate *>> grid(
      _appConfig.WfcSize.pos[0],
      std::vector<ColorTileCandidate *>(_appConfig.WfcSize.pos[1], nullptr));
    _colorGrid.Draw(
      VisualizationType::Color, grid, _appConfig.ColorTileDrawSize);
    _entropyGrid.Draw(
      VisualizationType::Entropy, grid, _appConfig.ColorTileDrawSize);
  }

private:
  AppConfig _appConfig;
  SettingsVisualizer _settingsForm;
  ColorWFCVisualizer _colorGrid;
  ColorWFCVisualizer _entropyGrid;
};

Walnut::Application *Walnut::CreateApplication(int argc, char **argv) {
  Walnut::ApplicationSpecification spec{
    .Name = "WFC - Color",
    .Width = 1600,
    .Height = 900,
  };

  Walnut::Application *app = new Walnut::Application(spec);
  app->PushLayer<WaveFunctionCollapseLayer>();
  app->SetMenubarCallback([app]() {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Exit")) {
        app->Close();
      }
      ImGui::EndMenu();
    }
  });
  return app;
}