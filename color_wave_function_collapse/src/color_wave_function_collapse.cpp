
#include "Walnut/Application.h"
#include "Walnut/Image.h"
#include "config/app_config.hpp"
#include "forms/settings.hpp"
#include "forms/visualizer.hpp"
#include "lib/thread_pool.hpp"
#include "wfc/color_tile_candidate.hpp"
#include "wfc/wave_function_collapse.hpp"

class WaveFunctionCollapseLayer : public Walnut::Layer {
public:
  WaveFunctionCollapseLayer()
      : _wfc(this->_appConfig.WfcSize,
          std::move(
            std::bind(&WaveFunctionCollapseLayer::CreateSuperpositionColor,
              this, std::placeholders::_1)),
          this->_appConfig.ObservationMethod, this->_appConfig.Seed, true),
        _threadPool(1) {
    RebuildColorPossibilities();
  }
  ~WaveFunctionCollapseLayer() {
    this->_wfc.Stop();
    this->_threadPool.Shutdown();
  }
  CellCandidate *CreateSuperpositionColor(Position pos) {
    return new ColorTileCandidate(pos);
  }
  virtual void OnUIRender() override {
    _settingsForm.Draw(this->_appConfig);
    if (this->_appConfig.IsShouldRecreateMap) {
      this->_appConfig.IsShouldRecreateMap = false;
      this->RebuildColorPossibilities();
      this->RecreateWFCWrapper();
      return;
    }

    std::vector<std::vector<ColorTileCandidate *>> grid;

    if (this->_wfcCanBeDrawn == true) {
      auto maxPosition = this->_wfc.GetMaxPosition();
      auto gridMap(this->_wfc.GetGrid());
      grid =
        std::vector<std::vector<ColorTileCandidate *>>(maxPosition.pos[0] + 1,
          std::vector<ColorTileCandidate *>(maxPosition.pos[1] + 1, nullptr));

      auto ctc = gridMap.begin();
      while (ctc != gridMap.end()) {
        auto x = ctc->second->GetPosition().pos[0];
        auto y = ctc->second->GetPosition().pos[1];
        grid.at(x).at(y) = static_cast<ColorTileCandidate *>(ctc->second);
        std::advance(ctc, 1);
      }
    } else {
      grid = std::vector<std::vector<ColorTileCandidate *>>(
        1, std::vector<ColorTileCandidate *>(1, nullptr));
    }

    _colorGrid.Draw(
      VisualizationType::Color, grid, _appConfig.ColorTileDrawSize);
    _entropyGrid.Draw(VisualizationType::Entropy, grid,
      _appConfig.ColorTileDrawSize,
      std::pow(this->_appConfig.ColorFragmentPossibilities + 1, 3));
    _avgColorGrid.Draw(
      VisualizationType::Average, grid, _appConfig.ColorTileDrawSize);

    if (this->_wfcCanBeDrawn == true) {
      if (this->_appConfig.IsManualStep) {
        if (this->_wfcStarted) {
          this->StopWFC();
        }
        if (this->_appConfig.IsShouldStep == true) {
          this->_appConfig.IsShouldStep = false;
          this->StepWFC();
        }
      } else if (this->_wfcStarted == false) {
        this->RunWFC();
        this->_wfcStarted = true;
      }
    }
  }
  void RunWFC() {
    this->_threadPool.QueueJob(
      Job(std::bind(&WaveFunctionCollapse::RunToCompletion, &this->_wfc)));
  }
  void StepWFC() {
    this->_threadPool.QueueJob(
      Job(std::bind(&WaveFunctionCollapse::Step, &this->_wfc)));
  }
  void StopWFC() {
    this->_wfc.Stop();
    this->_threadPool.QueueJob(Job([this]() { this->_wfcStarted = false; }));
  }

private:
  std::atomic<bool> _wfcStarted = false;
  std::atomic<bool> _wfcCanBeDrawn = true;
  AppConfig _appConfig;
  SettingsVisualizer _settingsForm;
  ColorWFCVisualizer _colorGrid;
  ColorWFCVisualizer _entropyGrid;
  ColorWFCVisualizer _avgColorGrid;
  WaveFunctionCollapse _wfc;
  std::vector<ColorTile> _candidates;
  ThreadPool _threadPool;

private:
  void RebuildColorPossibilities() {
    this->_candidates.clear();
    int16_t step = 255 / this->_appConfig.ColorFragmentPossibilities;
    for (int16_t r = 0; r < 255; r += step) {
      for (int16_t g = 0; g < 255; g += step) {
        for (int16_t b = 0; b < 255; b += step) {
          this->_candidates.emplace_back(ColorTile{
            .r = r,
            .g = g,
            .b = b,
          });
        }
      }
    }
    ColorTile::Comparator = step + 1;
    ColorTileCandidate::SetDefaultPossibilities(this->_candidates);
  }
  void RecreateWFCWrapper() {
    this->_wfcCanBeDrawn = false;
    this->_wfc.Stop();
    this->_threadPool.QueueJob(
      Job(std::bind(&WaveFunctionCollapseLayer::RecreateWFC, this)));
    this->_threadPool.QueueJob(Job([this]() {
      this->_wfcStarted = false;
      this->_wfcCanBeDrawn = true;
    }));
  }
  void RecreateWFC() {
    new (&this->_wfc) WaveFunctionCollapse(this->_appConfig.WfcSize,
      std::move(std::bind(&WaveFunctionCollapseLayer::CreateSuperpositionColor,
        this, std::placeholders::_1)),
      this->_appConfig.ObservationMethod, this->_appConfig.Seed, true);
  }
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