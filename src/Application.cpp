#include "Application.h"
#include "IScene.h"
#include "raylib.h"

Application::Application() = default;

Application::~Application() = default;

void Application::setScene(IScene* scene) {
  currentScene_ = scene;
}

void Application::run() {
  const int screenWidth = 1024;
  const int screenHeight = 768;
  InitWindow(screenWidth, screenHeight, "Algorithm Visualizer");
  SetTargetFPS(60);
  SetExitKey(KEY_NULL);  // ESC goes to scene (e.g. back to menu), not close app
  initialized_ = true;

  while (!shouldQuit_ && !WindowShouldClose()) {
    if (currentScene_) {
      currentScene_->handleInput();
      currentScene_->update();
      currentScene_->draw();
    }
  }

  CloseWindow();
  initialized_ = false;
}
