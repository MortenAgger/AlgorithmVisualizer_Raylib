#pragma once

#include "IScene.h"
#include "IRenderer.h"
#include "heap/HeapModel.h"
#include "heap/HeapStepPlayer.h"
#include "heap/HeapController.h"
#include "heap/HeapStep.h"
#include <functional>
#include <string>

class Application;

/**
 * Heap-visualisering: træet er 100% afledt af array-indekser.
 * UI sender commands → Controller → Engine (steps) → Player.
 * Tree view + Array view + panel (Insert, DeleteRoot, Heapify, Min/Max).
 */
class HeapScene : public IScene {
public:
  static constexpr float TREE_CENTER_X = 512.f;
  static constexpr float TREE_TOP_Y = 140.f;
  static constexpr float NODE_R = 22.f;
  static constexpr float LEVEL_DY = 70.f;
  static constexpr float ARRAY_CELL_W = 36.f;
  static constexpr float ARRAY_Y = 680.f;
  static constexpr float PANEL_X = 24.f;
  static constexpr float PANEL_Y = 72.f;

  HeapScene(IRenderer* renderer, Application* app);
  void setMaxHeap(bool maxHeap);
  void setOnBackToMenu(std::function<void()> cb) { onBackToMenu_ = std::move(cb); }

  void handleInput() override;
  void update() override;
  void draw() override;

private:
  void drawTreeView();
  void drawArrayView();
  void drawPanel();
  void drawStepHighlight();
  std::pair<float, float> getNodePosition(size_t index) const;
  int hitTestTree(float mx, float my) const;
  int hitTestArray(float mx, float my) const;
  viz::Color colorForNode(size_t index) const;
  bool buttonHit(float mx, float my, float bx, float by, float bw, float bh) const;

  IRenderer* renderer_;
  Application* app_;
  std::function<void()> onBackToMenu_;

  heap::HeapModel model_;
  heap::HeapStepPlayer player_;
  heap::HeapController controller_;

  bool isMaxHeap_{false};
  int insertValueEdit_{1};
  bool editingNode_{false};
  int editIndex_{-1};
  std::string editBuffer_;
  std::string buildArrayInput_;
  bool buildInputFocus_{false};
  bool editingInsertValue_{false};
  std::string insertValueBuffer_;
  float speed_{2.f};
  static std::vector<int> parseValues(const std::string& s);
  bool insertValueHit(float mx, float my) const;
  bool arrayInputHit(float mx, float my) const;
  float lastClickTime_{0.f};
  float lastClickX_{0.f}, lastClickY_{0.f};
  float playAccum_{0.f};
  std::string statusMessage_;
  static constexpr float DOUBLE_CLICK_TIME = 0.35f;
  static constexpr float DOUBLE_CLICK_MAX_DIST = 8.f;
};
