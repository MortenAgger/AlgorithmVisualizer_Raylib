#include "Application.h"
#include "MainMenuScene.h"
#include "HeapScene.h"
#include "PathfindingScene.h"
#include "MstScene.h"
#include "render/RaylibRenderer.h"
#include <memory>

int main() {
  Application app;
  RaylibRenderer renderer;
  auto mainMenu = std::make_unique<MainMenuScene>(&renderer, &app);
  auto heapScene = std::make_unique<HeapScene>(&renderer, &app);
  auto pathfindingScene = std::make_unique<PathfindingScene>(&renderer, &app);
  auto mstScene = std::make_unique<MstScene>(&renderer, &app);

  mainMenu->setAlgorithmChoiceCallback([&](const std::string& name) {
    if (name == "Shortest path") {
      pathfindingScene->setOnBackToMenu([&]() { app.setScene(mainMenu.get()); });
      app.setScene(pathfindingScene.get());
      return;
    }
    if (name == "MST (Prim/Kruskal)") {
      mstScene->setOnBackToMenu([&]() { app.setScene(mainMenu.get()); });
      app.setScene(mstScene.get());
      return;
    }
    if (name == "Heap (Min/Max)") {
      heapScene->setOnBackToMenu([&]() { app.setScene(mainMenu.get()); });
      app.setScene(heapScene.get());
      return;
    }
  });

  app.setScene(mainMenu.get());
  app.run();
  return 0;
}
