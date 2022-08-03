
#include "Walnut/Application.h"
#include "Walnut/Image.h"
#include "forms/settings.hpp"

class WaveFunctionCollapseLayer : public Walnut::Layer {
public:
  virtual void OnUIRender() override { _settingsForm.Draw(); }

private:
  SettingsVisualizer _settingsForm;
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