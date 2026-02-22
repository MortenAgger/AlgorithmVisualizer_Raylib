#pragma once

#include "IScene.h"
#include "ui/Button.h"
#include <memory>
#include <vector>
#include <string>
#include <functional>

class Application;
class IRenderer;

/**
 * Main menu: algorithm selection and start. SRP: menu screen only.
 */
class MainMenuScene : public IScene {
public:
  using AlgorithmChoiceCallback = std::function<void(const std::string&)>;

  MainMenuScene(IRenderer* renderer, Application* app);
  void setAlgorithmChoiceCallback(AlgorithmChoiceCallback cb) { onAlgorithmChosen_ = std::move(cb); }
  void handleInput() override;
  void update() override;
  void draw() override;

  void setAlgorithms(const std::vector<std::string>& names);

private:
  IRenderer* renderer_;
  Application* app_;
  AlgorithmChoiceCallback onAlgorithmChosen_;
  std::vector<std::string> algorithmNames_;
  int selectedIndex_{0};
  std::vector<Button> buttons_;
  void layoutButtons();
};
