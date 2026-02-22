#include "MainMenuScene.h"
#include "Application.h"
#include "raylib.h"
#include <algorithm>

MainMenuScene::MainMenuScene(IRenderer* renderer, Application* app)
  : renderer_(renderer), app_(app) {
  setAlgorithms({"Shortest path", "MST (Prim/Kruskal)", "Heap (Min/Max)"});
  layoutButtons();
}

void MainMenuScene::setAlgorithms(const std::vector<std::string>& names) {
  algorithmNames_ = names;
  selectedIndex_ = 0;
  layoutButtons();
}

void MainMenuScene::layoutButtons() {
  buttons_.clear();
  float x = 400;
  float y = 280;
  float w = 224;
  float h = 40;
  float gap = 12;

  buttons_.emplace_back(x, y, w, h, "Start");
  buttons_.back().setCallback([this]() {
    if (onAlgorithmChosen_ && selectedIndex_ < static_cast<int>(algorithmNames_.size()))
      onAlgorithmChosen_(algorithmNames_[selectedIndex_]);
  });
  y += h + gap;
  buttons_.emplace_back(x, y, w, h, "Prev algorithm");
  buttons_.back().setCallback([this]() {
    selectedIndex_ = (selectedIndex_ + static_cast<int>(algorithmNames_.size()) - 1) % static_cast<int>(algorithmNames_.size());
  });
  y += h + gap;
  buttons_.emplace_back(x, y, w, h, "Next algorithm");
  buttons_.back().setCallback([this]() {
    selectedIndex_ = (selectedIndex_ + 1) % static_cast<int>(algorithmNames_.size());
  });
  y += h + gap;
  buttons_.emplace_back(x, y, w, h, "Luk program");
  buttons_.back().setCallback([this]() {
    if (app_) app_->requestQuit();
  });
}

void MainMenuScene::handleInput() {
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    float mx = static_cast<float>(GetMouseX());
    float my = static_cast<float>(GetMouseY());
    for (auto& b : buttons_)
      b.onMouseClick(mx, my);
  }
  if (IsKeyPressed(KEY_LEFT)) {
    selectedIndex_ = (selectedIndex_ + algorithmNames_.size() - 1) % algorithmNames_.size();
  }
  if (IsKeyPressed(KEY_RIGHT)) {
    selectedIndex_ = (selectedIndex_ + 1) % algorithmNames_.size();
  }
}

void MainMenuScene::update() {}

void MainMenuScene::draw() {
  renderer_->beginFrame();

  viz::Color titleCol{40, 40, 40, 255};
  renderer_->drawText("Algorithm Visualizer", viz::Vector2{320, 80}, 42, titleCol);

  std::string algoName = selectedIndex_ < static_cast<int>(algorithmNames_.size())
    ? algorithmNames_[selectedIndex_] : "";
  renderer_->drawText("Algorithm: " + algoName, viz::Vector2{400, 220}, 24, titleCol);

  for (auto& b : buttons_)
    b.draw(renderer_);

  renderer_->endFrame();
}
