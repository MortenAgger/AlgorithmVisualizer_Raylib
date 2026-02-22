#pragma once

#include "IScene.h"
#include "datastructures/Graph.h"
#include "IRenderer.h"
#include "IAlgorithm.h"
#include "pathfinding/PathEvents.h"
#include "pathfinding/GraphEventRecorder.h"
#include <memory>
#include <vector>
#include <functional>

class Application;

/**
 * Unified Pathfinding: two modes switchable via top bar.
 * - Grid Pathfinding: 2D grid, walls, start/end, BFS/DFS/Dijkstra/A*, presets, event replay.
 * - Graph Editor: place nodes, drag edges, weights, same algorithms (BFS/DFS unweighted).
 * Shared: algorithm menu, Visualize, Clear, Speed, event player, metrics (nodes visited, path length).
 */
class PathfindingScene : public IScene {
public:
  static constexpr int GRID_COLS = 41;
  static constexpr int GRID_ROWS = 25;
  static constexpr int HEADER_H = 72;
  static constexpr int LEGEND_H = 26;
  static constexpr int GRID_OFFSET_Y = 106;
  static constexpr float GRAPH_CANVAS_TOP = 142.f;

  PathfindingScene(IRenderer* renderer, Application* app);
  void setOnBackToMenu(std::function<void()> cb) { onBackToMenu_ = std::move(cb); }

  void handleInput() override;
  void update() override;
  void draw() override;

private:
  enum class Mode { Grid, Graph };
  enum class GraphMode { Weighted, Unweighted };
  enum class EditorState { Idle, DraggingNode, CtrlSelectingEdge, EditingWeight };
  static GraphMode graphModeFromAlgorithm(const std::string& algo);
  void drawTopBar();
  void drawLegend();
  void drawGridContent();
  void drawGraphContent();
  void drawGraphEditorHints();
  void drawWeightEditOverlay();
  void drawSharedMetrics();
  void handleTopBarInput(float mx, float my);
  void handleGridInput(float mx, float my);
  void handleGraphInput(float mx, float my);
  int graphNodeAt(float mx, float my) const;
  bool graphWeightLabelAt(float mx, float my, int& outFromId, int& outToId) const;
  void runAlgorithm();
  void clearPathOnly();
  void clearWallsOnly();
  void clearAll();
  void applyPreset(const char* preset);
  bool cellInBounds(int r, int c) const;
  void cellFromScreen(float mx, float my, int& outR, int& outC) const;
  int nodeId(int r, int c) const { return r * GRID_COLS + c; }
  void buildGraphFromGrid();
  bool buttonHit(float mx, float my, float bx, float by, float bw, float bh) const;
  viz::Color colorFromState(pathfinding::NodeViewState s) const;

  IRenderer* renderer_;
  Application* app_;
  std::function<void()> onBackToMenu_;

  Mode mode_{Mode::Grid};
  pathfinding::EventPlayer eventPlayer_;
  pathfinding::GraphEventRecorder recorder_{eventPlayer_};

  std::vector<std::vector<bool>> walls_;
  int startR_{0}, startC_{0};
  int endR_{GRID_ROWS - 1}, endC_{GRID_COLS - 1};
  float cellW_{0}, cellH_{0};
  static constexpr int GRID_OFFSET_X = 20;

  std::shared_ptr<Graph> graph_;
  int nextNodeId_{0};
  int graphStartId_{0};
  int graphTargetId_{0};
  bool placeStartMode_{false};
  bool placeEndMode_{false};
  bool draggingWall_{false};
  bool drawingWall_{true};
  bool draggingStart_{false};
  bool draggingEnd_{false};

  EditorState editorState_{EditorState::Idle};
  int dragNodeId_{-1};
  int ctrlSelectFirstId_{-1};
  int editWeightFromId_{-1};
  int editWeightToId_{-1};
  std::string editWeightBuffer_;
  std::string graphStatusMessage_;
  float dragStartX_{0.f};
  float dragStartY_{0.f};

  std::string algorithmName_{"BFS"};
  int speedIndex_{1};
  int stepDelayCounter_{0};
  bool running_{false};
  bool animationRunning_{true};
  bool singleStep_{false};
  std::shared_ptr<Graph> algoGraph_;
  std::unique_ptr<IAlgorithm> algorithm_;
  int algoStartId_{0};
  int algoTargetId_{0};
  bool useEventPlayer_{false};
};
