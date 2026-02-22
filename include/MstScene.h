#pragma once

#include "IScene.h"
#include "IRenderer.h"
#include "mst/Graph.h"
#include "mst/Step.h"
#include "mst/SimulationEngine.h"
#include "mst/IMstAlgorithm.h"
#include "mst/PrimMst.h"
#include "mst/KruskalMst.h"
#include <memory>
#include <functional>
#include <string>
#include <vector>

class Application;

enum class MstEditorState { Idle, DraggingNode, CtrlSelectingEdge, EditingWeight };

class MstScene : public IScene {
public:
  static constexpr int WIN_W = 1024;
  static constexpr int WIN_H = 768;
  static constexpr float NODE_R = 18.f;
  static constexpr float EDGE_HIT_THRESH = 6.f;
  static constexpr float TOPBAR_H = 40.f;
  static constexpr float LEFT_PANEL_W = 180.f;
  static constexpr float RIGHT_PANEL_W = 260.f;
  static constexpr float CANVAS_LEFT = 180.f;
  static constexpr float CANVAS_TOP = 44.f;
  static constexpr float CANVAS_W = WIN_W - LEFT_PANEL_W - RIGHT_PANEL_W;
  static constexpr float CANVAS_H = WIN_H - CANVAS_TOP;

  MstScene(IRenderer* renderer, Application* app);
  void setOnBackToMenu(std::function<void()> cb) { onBackToMenu_ = std::move(cb); }

  void handleInput() override;
  void update() override;
  void draw() override;

private:
  void drawTopBar();
  void drawLeftPanel();
  void drawRightPanel();
  void drawCanvas();
  void drawWeightEditOverlay();
  viz::Color edgeColor(int edgeId) const;
  viz::Color nodeColor(int nodeId) const;
  bool inCanvas(float mx, float my) const;
  /** Returns edge id if (mx, my) hits an edge weight label, else -1. */
  int weightLabelEdgeAt(float mx, float my) const;
  void runAlgorithm();
  void clearGraph(bool confirm);

  IRenderer* renderer_;
  Application* app_;
  std::function<void()> onBackToMenu_;

  mst::Graph graph_;
  mst::SimulationEngine engine_;
  std::unique_ptr<mst::IMstAlgorithm> algorithm_;
  int algoIndex_{0};
  int startNodeId_{0};

  MstEditorState editorState_{MstEditorState::Idle};
  int selectedNode_{-1};
  int selectedEdge_{-1};
  int hoverNode_{-1};
  int hoverEdge_{-1};
  int ctrlSelectFirstId_{-1};
  int dragNodeId_{-1};
  float dragStartX_{0}, dragStartY_{0};
  bool pickStartMode_{false};

  bool weightEdit_{false};
  int weightEditEdgeId_{-1};
  std::string weightEditBuffer_;
  float playAccum_{0.f};
  bool playing_{false};
  float speed_{1.5f};
  int speedIndex_{2};
  std::vector<float> speedOptions_{0.5f, 1.f, 1.5f, 2.f, 2.5f, 3.f};
  bool shiftHeld_{false};
};
