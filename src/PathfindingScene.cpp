#include "PathfindingScene.h"
#include "Application.h"
#include "algorithms/BFSAlgorithm.h"
#include "algorithms/DFSAlgorithm.h"
#include "algorithms/DijkstraAlgorithm.h"
#include "algorithms/AStarAlgorithm.h"
#include "serialization/StateSerializer.h"
#include "raylib.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <cstring>

namespace {
constexpr int WIN_W = 1024;
constexpr int WIN_H = 768;
constexpr float BTN_H = 28.f;
constexpr viz::Color C_EMPTY{252, 252, 252, 255};
constexpr viz::Color C_NODE_DEFAULT{228, 228, 232, 255};  // visible circle for graph nodes
constexpr viz::Color C_WALL{45, 55, 72, 255};
constexpr viz::Color C_START{50, 200, 90, 255};
constexpr viz::Color C_END{230, 60, 60, 255};
constexpr viz::Color C_VISITED{100, 200, 200, 255};
constexpr viz::Color C_INQUEUE{180, 220, 255, 255};
constexpr viz::Color C_PATH{255, 230, 80, 255};
constexpr viz::Color C_HEADER{45, 55, 72, 255};
constexpr viz::Color C_BTN{70, 85, 105, 255};
constexpr viz::Color C_BTN_HOVER{90, 110, 135, 255};
constexpr viz::Color C_TEXT{255, 255, 255, 255};
constexpr viz::Color C_TEXT_DARK{30, 30, 30, 255};
constexpr viz::Color C_EDGE{180, 180, 180, 255};
}

PathfindingScene::PathfindingScene(IRenderer* renderer, Application* app)
  : renderer_(renderer), app_(app) {
  walls_.resize(GRID_ROWS, std::vector<bool>(GRID_COLS, false));
  cellW_ = static_cast<float>(WIN_W - 2 * GRID_OFFSET_X) / GRID_COLS;
  cellH_ = static_cast<float>(WIN_H - GRID_OFFSET_Y - 20) / GRID_ROWS;
  graph_ = std::make_shared<Graph>();
}

bool PathfindingScene::buttonHit(float mx, float my, float bx, float by, float bw, float bh) const {
  return mx >= bx && mx <= bx + bw && my >= by && my <= by + bh;
}

viz::Color PathfindingScene::colorFromState(pathfinding::NodeViewState s) const {
  switch (s) {
    case pathfinding::NodeViewState::Unvisited: return C_EMPTY;
    case pathfinding::NodeViewState::InQueue:
    case pathfinding::NodeViewState::Visiting: return C_INQUEUE;
    case pathfinding::NodeViewState::Visited: return C_VISITED;
    case pathfinding::NodeViewState::Path: return C_PATH;
    default: return C_EMPTY;
  }
}

void PathfindingScene::drawTopBar() {
  renderer_->drawRectangle(0, 0, WIN_W, HEADER_H, C_HEADER);
  const float row1Y = 10.f;
  const float row2Y = 42.f;
  const float gap = 14.f;

  float x = 12;
  renderer_->drawText("Pathfinding", viz::Vector2{x, 10}, 18, C_TEXT);
  x = 160;
  if (mode_ == Mode::Grid)
    renderer_->drawRectangle(x, row1Y, 130.f, BTN_H, C_BTN_HOVER);
  else
    renderer_->drawRectangle(x, row1Y, 130.f, BTN_H, C_BTN);
  renderer_->drawText("Grid Pathfinding", viz::Vector2{x + 6, row1Y + 5}, 12, C_TEXT);
  x += 130.f + gap;
  if (mode_ == Mode::Graph)
    renderer_->drawRectangle(x, row1Y, 92.f, BTN_H, C_BTN_HOVER);
  else
    renderer_->drawRectangle(x, row1Y, 92.f, BTN_H, C_BTN);
  renderer_->drawText("Graph Editor", viz::Vector2{x + 6, row1Y + 5}, 12, C_TEXT);
  x += 92.f + gap;
  const char* algos[] = {"BFS", "DFS", "Dijkstra", "A*"};
  for (int i = 0; i < 4; ++i) {
    float w = 58.f;
    viz::Color btnCol = (algorithmName_ == algos[i]) ? C_BTN_HOVER : C_BTN;
    renderer_->drawRectangle(x, row1Y, w, BTN_H, btnCol);
    renderer_->drawText(algos[i], viz::Vector2{x + 4, row1Y + 5}, 11, C_TEXT);
    x += w + 6.f;
  }
  x += gap;
  std::string visLabel = "Visualize " + algorithmName_;
  float vw = 130.f;
  renderer_->drawRectangle(x, row1Y, vw, BTN_H, viz::Color{80, 160, 120, 255});
  renderer_->drawText(visLabel, viz::Vector2{x + 8, row1Y + 5}, 12, C_TEXT);

  x = 12;
  if (mode_ == Mode::Grid) {
    const char* speeds[] = {"Slow", "Med", "Fast"};
    for (int i = 0; i < 3; ++i) {
      float w = 48.f;
      viz::Color btnCol = (speedIndex_ == i) ? C_BTN_HOVER : C_BTN;
      renderer_->drawRectangle(x, row2Y, w, BTN_H, btnCol);
      renderer_->drawText(speeds[i], viz::Vector2{x + 8, row2Y + 5}, 12, C_TEXT);
      x += w + 10.f;
    }
    x += gap;
    float clearPathX = WIN_W - 12.f - 88.f;
    float clearWallsX = clearPathX - 10.f - 95.f;
    renderer_->drawRectangle(clearWallsX, row2Y, 95.f, BTN_H, C_BTN);
    renderer_->drawText("Clear Walls", viz::Vector2{clearWallsX + 6, row2Y + 5}, 12, C_TEXT);
    renderer_->drawRectangle(clearPathX, row2Y, 88.f, BTN_H, C_BTN);
    renderer_->drawText("Clear Path", viz::Vector2{clearPathX + 6, row2Y + 5}, 12, C_TEXT);
  } else {
    float clearPathX = WIN_W - 12.f - 88.f;
    float clearGraphX = clearPathX - 12.f - 100.f;
    float saveX = clearGraphX - 12.f - 58.f;
    float loadX = saveX - 10.f - 58.f;
    renderer_->drawRectangle(loadX, row2Y, 58.f, BTN_H, C_BTN);
    renderer_->drawText("Load", viz::Vector2{loadX + 14, row2Y + 5}, 12, C_TEXT);
    renderer_->drawRectangle(saveX, row2Y, 58.f, BTN_H, C_BTN);
    renderer_->drawText("Save", viz::Vector2{saveX + 14, row2Y + 5}, 12, C_TEXT);
    renderer_->drawRectangle(clearGraphX, row2Y, 100.f, BTN_H, viz::Color{180, 80, 80, 255});
    renderer_->drawText("Clear Graph", viz::Vector2{clearGraphX + 8, row2Y + 5}, 12, C_TEXT);
    renderer_->drawRectangle(clearPathX, row2Y, 88.f, BTN_H, C_BTN);
    renderer_->drawText("Clear Path", viz::Vector2{clearPathX + 6, row2Y + 5}, 12, C_TEXT);
  }
}

void PathfindingScene::drawLegend() {
  float ly = HEADER_H + 4;
  float lx = 20;
  const float sq = 14.f;
  auto drawItem = [this, sq](float px, float py, const char* label, viz::Color col) {
    renderer_->drawRectangle(px, py, sq, sq, col);
    renderer_->drawText(label, viz::Vector2{px + sq + 6, py - 2}, 13, C_TEXT_DARK);
  };
  drawItem(lx, ly, "Start", C_START);
  lx += 75;
  drawItem(lx, ly, "End", C_END);
  lx += 60;
  drawItem(lx, ly, "Visited", C_VISITED);
  lx += 78;
  drawItem(lx, ly, "Unvisited", C_EMPTY);
  lx += 88;
  drawItem(lx, ly, "Path", C_PATH);
  lx += 65;
  drawItem(lx, ly, "Wall", C_WALL);
}

void PathfindingScene::drawSharedMetrics() {
  if (!running_ && eventPlayer_.getEventCount() == 0) return;
  int visited = eventPlayer_.getNodesVisited();
  int pathLen = eventPlayer_.getPathLength();
  std::string m = "Nodes visited: " + std::to_string(visited) + "  |  Path length: " + std::to_string(pathLen);
  renderer_->drawText(m, viz::Vector2{WIN_W - 280, HEADER_H + 4}, 13, C_TEXT_DARK);
}

void PathfindingScene::drawGridContent() {
  for (int r = 0; r < GRID_ROWS; ++r) {
    for (int c = 0; c < GRID_COLS; ++c) {
      float x = GRID_OFFSET_X + c * cellW_;
      float y = GRID_OFFSET_Y + r * cellH_;
      viz::Color col = C_EMPTY;
      if (walls_[r][c]) col = C_WALL;
      else if (useEventPlayer_ && eventPlayer_.getEventCount() > 0) {
        int id = nodeId(r, c);
        pathfinding::NodeViewState s = eventPlayer_.getNodeState(id);
        if (std::find(eventPlayer_.getPath().begin(), eventPlayer_.getPath().end(), id) != eventPlayer_.getPath().end())
          col = C_PATH;
        else
          col = colorFromState(s);
      } else {
        if (r == startR_ && c == startC_) col = C_START;
        else if (r == endR_ && c == endC_) col = C_END;
      }
      if (r == startR_ && c == startC_) col = C_START;
      if (r == endR_ && c == endC_) col = C_END;
      renderer_->drawRectangle(x + 1, y + 1, cellW_ - 1, cellH_ - 1, col);
    }
  }
}

PathfindingScene::GraphMode PathfindingScene::graphModeFromAlgorithm(const std::string& algo) {
  return (algo == "Dijkstra" || algo == "A*") ? GraphMode::Weighted : GraphMode::Unweighted;
}

bool PathfindingScene::graphWeightLabelAt(float mx, float my, int& outFromId, int& outToId) const {
  const float labelW = 28.f;
  const float labelH = 18.f;
  GraphMode gm = graphModeFromAlgorithm(algorithmName_);
  if (gm != GraphMode::Weighted) return false;
  for (const Edge& e : graph_->edges()) {
    const Node* from = graph_->getNode(e.fromId);
    const Node* to = graph_->getNode(e.toId);
    if (!from || !to) continue;
    float lx = (from->x + to->x) * 0.5f - labelW * 0.5f;
    float ly = (from->y + to->y) * 0.5f - labelH * 0.5f;
    if (mx >= lx && mx <= lx + labelW && my >= ly && my <= ly + labelH) {
      outFromId = e.fromId;
      outToId = e.toId;
      return true;
    }
  }
  return false;
}

void PathfindingScene::drawGraphContent() {
  renderer_->drawRectangle(0, GRAPH_CANVAS_TOP, WIN_W, WIN_H - GRAPH_CANVAS_TOP, C_EMPTY);
  GraphMode gm = graphModeFromAlgorithm(algorithmName_);
  const float NODE_R = 18.f;
  const float labelW = 28.f;
  const float labelH = 18.f;
  for (const Edge& e : graph_->edges()) {
    const Node* from = graph_->getNode(e.fromId);
    const Node* to = graph_->getNode(e.toId);
    if (from && to) {
      renderer_->drawEdge(viz::Vector2{from->x, from->y}, viz::Vector2{to->x, to->y}, C_EDGE);
      float mx = (from->x + to->x) * 0.5f, my = (from->y + to->y) * 0.5f;
      std::string wText = (gm == GraphMode::Weighted) ? std::to_string(e.weight) : "1";
      float lx = mx - labelW * 0.5f, ly = my - labelH * 0.5f;
      renderer_->drawRectangle(lx, ly, labelW, labelH, C_EMPTY);
      renderer_->drawText(wText, viz::Vector2{lx + 4, ly + 1}, 14, C_TEXT_DARK);
    }
  }
  for (const Node& n : graph_->nodes()) {
    viz::Color col = C_NODE_DEFAULT;
    if (n.id == graphStartId_) col = C_START;
    else if (n.id == graphTargetId_) col = C_END;
    else if (editorState_ == EditorState::CtrlSelectingEdge && n.id == ctrlSelectFirstId_) col = C_INQUEUE;
    else if (useEventPlayer_ && eventPlayer_.getEventCount() > 0) {
      if (std::find(eventPlayer_.getPath().begin(), eventPlayer_.getPath().end(), n.id) != eventPlayer_.getPath().end())
        col = C_PATH;
      else {
        pathfinding::NodeViewState s = eventPlayer_.getNodeState(n.id);
        col = (s == pathfinding::NodeViewState::Unvisited) ? C_NODE_DEFAULT : colorFromState(s);
      }
    }
    renderer_->drawNode(viz::Vector2{n.x, n.y}, NODE_R, col);
    renderer_->drawText(std::to_string(n.id), viz::Vector2{n.x - 6, n.y - 8}, 16, C_TEXT_DARK);
  }
}

void PathfindingScene::drawGraphEditorHints() {
  if (mode_ != Mode::Graph) return;
  GraphMode gm = graphModeFromAlgorithm(algorithmName_);
  float y = GRAPH_CANVAS_TOP - 22;
  renderer_->drawText("Canvas (place nodes below) | S=Start T=End | Run=Visualize | P=Pause", viz::Vector2{20, y}, 12, C_TEXT_DARK);
  renderer_->drawText("Ctrl+Click two nodes = connect (tree: each node at most one parent) | Right-click node = remove", viz::Vector2{20, y + 16}, 12, C_TEXT_DARK);
  if (gm != GraphMode::Weighted)
    renderer_->drawText("Weights fixed to 1 (BFS/DFS). Select Dijkstra or A* to edit weights.", viz::Vector2{WIN_W - 420, y + 8}, 11, C_TEXT_DARK);
  if (!graphStatusMessage_.empty())
    renderer_->drawText(graphStatusMessage_, viz::Vector2{20, y + 34}, 11, C_TEXT_DARK);
}

void PathfindingScene::drawWeightEditOverlay() {
  if (mode_ != Mode::Graph || editorState_ != EditorState::EditingWeight) return;
  if (editWeightFromId_ < 0 || editWeightToId_ < 0) return;
  float cx = WIN_W * 0.5f - 100;
  float cy = WIN_H * 0.5f - 35;
  renderer_->drawRectangle(cx, cy, 200, 70, C_EMPTY);
  renderer_->drawText("Edge weight (positive integer)", viz::Vector2{cx + 10, cy + 8}, 12, C_TEXT_DARK);
  renderer_->drawText(editWeightBuffer_.empty() ? "_" : editWeightBuffer_, viz::Vector2{cx + 10, cy + 32}, 18, C_TEXT_DARK);
  renderer_->drawText("Enter=OK  Esc=cancel", viz::Vector2{cx + 10, cy + 54}, 11, C_TEXT_DARK);
}

bool PathfindingScene::cellInBounds(int r, int c) const {
  return r >= 0 && r < GRID_ROWS && c >= 0 && c < GRID_COLS;
}

void PathfindingScene::cellFromScreen(float mx, float my, int& outR, int& outC) const {
  int c = static_cast<int>((mx - GRID_OFFSET_X) / cellW_);
  int r = static_cast<int>((my - GRID_OFFSET_Y) / cellH_);
  if (c < 0) c = 0; if (c >= GRID_COLS) c = GRID_COLS - 1;
  if (r < 0) r = 0; if (r >= GRID_ROWS) r = GRID_ROWS - 1;
  outR = r; outC = c;
}

void PathfindingScene::buildGraphFromGrid() {
  algoGraph_ = std::make_shared<Graph>();
  for (int r = 0; r < GRID_ROWS; ++r)
    for (int c = 0; c < GRID_COLS; ++c) {
      if (walls_[r][c]) continue;
      int id = nodeId(r, c);
      float x = GRID_OFFSET_X + c * cellW_ + cellW_ * 0.5f;
      float y = GRID_OFFSET_Y + r * cellH_ + cellH_ * 0.5f;
      algoGraph_->addNode(id, x, y);
    }
  const int dr[] = {-1, 1, 0, 0}, dc[] = {0, 0, -1, 1};
  for (int r = 0; r < GRID_ROWS; ++r)
    for (int c = 0; c < GRID_COLS; ++c) {
      if (walls_[r][c]) continue;
      for (int d = 0; d < 4; ++d) {
        int nr = r + dr[d], nc = c + dc[d];
        if (cellInBounds(nr, nc) && !walls_[nr][nc])
          algoGraph_->addEdge(nodeId(r, c), nodeId(nr, nc), 1);
      }
    }
}

int PathfindingScene::graphNodeAt(float mx, float my) const {
  const float R = 18.f;
  for (const Node& n : graph_->nodes()) {
    float dx = mx - n.x, dy = my - n.y;
    if (dx*dx + dy*dy <= R*R) return n.id;
  }
  return -1;
}

void PathfindingScene::runAlgorithm() {
  if (running_) return;
  clearPathOnly();
  eventPlayer_.clear();
  recorder_.reset();
  useEventPlayer_ = true;
  if (mode_ == Mode::Grid) {
    buildGraphFromGrid();
    algoStartId_ = nodeId(startR_, startC_);
    algoTargetId_ = nodeId(endR_, endC_);
    if (!algoGraph_->getNode(algoStartId_) || !algoGraph_->getNode(algoTargetId_)) return;
    algoGraph_->clearStates();
    if (algorithmName_ == "DFS") algorithm_ = std::make_unique<DFSAlgorithm>(algoGraph_.get(), algoStartId_, algoTargetId_);
    else if (algorithmName_ == "Dijkstra") algorithm_ = std::make_unique<DijkstraAlgorithm>(algoGraph_.get(), algoStartId_, algoTargetId_);
    else if (algorithmName_ == "A*") algorithm_ = std::make_unique<AStarAlgorithm>(algoGraph_.get(), algoStartId_, algoTargetId_);
    else algorithm_ = std::make_unique<BFSAlgorithm>(algoGraph_.get(), algoStartId_, algoTargetId_);
  } else {
    if (graph_->nodeCount() < 2) return;
    algoGraph_ = graph_;
    algoStartId_ = graphStartId_;
    algoTargetId_ = graphTargetId_;
    if (!algoGraph_->getNode(algoStartId_) || !algoGraph_->getNode(algoTargetId_)) return;
    algoGraph_->clearStates();
    if (algorithmName_ == "DFS") algorithm_ = std::make_unique<DFSAlgorithm>(algoGraph_.get(), algoStartId_, algoTargetId_);
    else if (algorithmName_ == "Dijkstra") algorithm_ = std::make_unique<DijkstraAlgorithm>(algoGraph_.get(), algoStartId_, algoTargetId_);
    else if (algorithmName_ == "A*") algorithm_ = std::make_unique<AStarAlgorithm>(algoGraph_.get(), algoStartId_, algoTargetId_);
    else algorithm_ = std::make_unique<BFSAlgorithm>(algoGraph_.get(), algoStartId_, algoTargetId_);
  }
  running_ = true;
}

void PathfindingScene::clearPathOnly() {
  running_ = false;
  algorithm_.reset();
  animationRunning_ = true;
  useEventPlayer_ = false;
  eventPlayer_.clear();
  if (algoGraph_) algoGraph_->clearStates();
}

void PathfindingScene::clearWallsOnly() {
  clearPathOnly();
  for (int r = 0; r < GRID_ROWS; ++r)
    for (int c = 0; c < GRID_COLS; ++c)
      walls_[r][c] = false;
  startR_ = 0; startC_ = 0;
  endR_ = GRID_ROWS - 1; endC_ = GRID_COLS - 1;
}

void PathfindingScene::clearAll() {
  clearWallsOnly();
  if (mode_ == Mode::Graph) {
    graph_ = std::make_shared<Graph>();
    nextNodeId_ = 0;
    graphStartId_ = 0;
    graphTargetId_ = 0;
    editorState_ = EditorState::Idle;
    dragNodeId_ = -1;
    ctrlSelectFirstId_ = -1;
    editWeightFromId_ = editWeightToId_ = -1;
    editWeightBuffer_.clear();
    graphStatusMessage_.clear();
  }
}

void PathfindingScene::applyPreset(const char* preset) {
  clearPathOnly();
  for (int r = 0; r < GRID_ROWS; ++r)
    for (int c = 0; c < GRID_COLS; ++c)
      walls_[r][c] = false;
  if (strcmp(preset, "empty") == 0) {
    startR_ = 0; startC_ = 0;
    endR_ = GRID_ROWS - 1; endC_ = GRID_COLS - 1;
    return;
  }
  if (strcmp(preset, "random") == 0) {
    std::mt19937 rng(static_cast<unsigned>(time(nullptr)));
    for (int r = 0; r < GRID_ROWS; ++r)
      for (int c = 0; c < GRID_COLS; ++c)
        if ((r != startR_ || c != startC_) && (r != endR_ || c != endC_) && (rng() % 4) == 0)
          walls_[r][c] = true;
    return;
  }
  if (strcmp(preset, "maze") == 0) {
    for (int r = 1; r < GRID_ROWS - 1; ++r)
      for (int c = 1; c < GRID_COLS - 1; ++c)
        if ((r + c) % 2 == 0 && (r * 7 + c) % 3 != 0)
          walls_[r][c] = true;
    startR_ = 0; startC_ = 0;
    endR_ = GRID_ROWS - 1; endC_ = GRID_COLS - 1;
  }
}

void PathfindingScene::handleTopBarInput(float mx, float my) {
  if (my < 0 || my >= HEADER_H) return;
  const float row1Y = 10.f;
  const float row2Y = 42.f;
  if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
  if (my >= row1Y && my < row1Y + BTN_H) {
    if (buttonHit(mx, my, 160.f, row1Y, 130.f, BTN_H)) { mode_ = Mode::Grid; clearPathOnly(); return; }
    if (buttonHit(mx, my, 304.f, row1Y, 92.f, BTN_H)) { mode_ = Mode::Graph; clearPathOnly(); return; }
    if (buttonHit(mx, my, 410.f, row1Y, 58.f, BTN_H)) { algorithmName_ = "BFS"; return; }
    if (buttonHit(mx, my, 474.f, row1Y, 58.f, BTN_H)) { algorithmName_ = "DFS"; return; }
    if (buttonHit(mx, my, 538.f, row1Y, 58.f, BTN_H)) { algorithmName_ = "Dijkstra"; return; }
    if (buttonHit(mx, my, 602.f, row1Y, 58.f, BTN_H)) { algorithmName_ = "A*"; return; }
    if (buttonHit(mx, my, 680.f, row1Y, 130.f, BTN_H)) { runAlgorithm(); return; }
    return;
  }
  if (my >= row2Y && my < row2Y + BTN_H) {
    if (mode_ == Mode::Grid) {
      if (buttonHit(mx, my, 12.f, row2Y, 48.f, BTN_H)) { speedIndex_ = 0; return; }
      if (buttonHit(mx, my, 70.f, row2Y, 48.f, BTN_H)) { speedIndex_ = 1; return; }
      if (buttonHit(mx, my, 128.f, row2Y, 48.f, BTN_H)) { speedIndex_ = 2; return; }
      float clearPathX = WIN_W - 12.f - 88.f;
      float clearWallsX = clearPathX - 10.f - 95.f;
      if (buttonHit(mx, my, clearWallsX, row2Y, 95.f, BTN_H)) { clearWallsOnly(); return; }
      if (buttonHit(mx, my, clearPathX, row2Y, 88.f, BTN_H)) { clearPathOnly(); return; }
    } else {
      float clearPathX = WIN_W - 12.f - 88.f;
      float clearGraphX = clearPathX - 12.f - 100.f;
      float saveX = clearGraphX - 12.f - 58.f;
      float loadX = saveX - 10.f - 58.f;
      if (buttonHit(mx, my, loadX, row2Y, 58.f, BTN_H)) {
        StateSerializer ser;
        auto loaded = ser.load("graph_save.json");
        if (loaded) {
          graph_ = std::shared_ptr<Graph>(loaded.release());
          int maxId = 0;
          for (const Node& n : graph_->nodes()) if (n.id > maxId) maxId = n.id;
          nextNodeId_ = maxId + 1;
          editorState_ = EditorState::Idle;
          dragNodeId_ = ctrlSelectFirstId_ = -1;
          editWeightFromId_ = editWeightToId_ = -1;
          editWeightBuffer_.clear();
        }
        return;
      }
      if (buttonHit(mx, my, saveX, row2Y, 58.f, BTN_H)) {
        StateSerializer ser;
        ser.save(*graph_, "graph_save.json");
        return;
      }
      if (buttonHit(mx, my, clearGraphX, row2Y, 100.f, BTN_H)) { clearAll(); return; }
      if (buttonHit(mx, my, clearPathX, row2Y, 88.f, BTN_H)) { clearPathOnly(); return; }
    }
  }
}

void PathfindingScene::handleGridInput(float mx, float my) {
  int r, c;
  cellFromScreen(mx, my, r, c);
  if (IsKeyPressed(KEY_S)) placeStartMode_ = true;
  if (IsKeyPressed(KEY_T)) placeEndMode_ = true;
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (placeStartMode_ && cellInBounds(r, c) && !walls_[r][c]) { startR_ = r; startC_ = c; placeStartMode_ = false; return; }
    if (placeEndMode_ && cellInBounds(r, c) && !walls_[r][c]) { endR_ = r; endC_ = c; placeEndMode_ = false; return; }
    if (!running_) {
      if (r == startR_ && c == startC_) return;
      if (r == endR_ && c == endC_) return;
      walls_[r][c] = true;
      draggingWall_ = true;
      drawingWall_ = true;
    }
  }
  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !running_) {
    walls_[r][c] = false;
    draggingWall_ = true;
    drawingWall_ = false;
  }
  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && draggingWall_ && drawingWall_ && !running_) walls_[r][c] = true;
  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && draggingWall_ && !running_) walls_[r][c] = false;
  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) draggingWall_ = false;
  if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) draggingWall_ = false;
}

void PathfindingScene::handleGraphInput(float mx, float my) {
  if (my < GRAPH_CANVAS_TOP) return;
  const float NODE_R = 18.f;
  bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
  int nodeIdHit = graphNodeAt(mx, my);
  int wFrom = -1, wTo = -1;
  bool weightLabelHit = graphWeightLabelAt(mx, my, wFrom, wTo);
  GraphMode gm = graphModeFromAlgorithm(algorithmName_);

  if (IsKeyPressed(KEY_S)) { placeStartMode_ = true; return; }
  if (IsKeyPressed(KEY_T)) { placeEndMode_ = true; return; }
  if (placeStartMode_ && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && nodeIdHit >= 0) {
    graphStartId_ = nodeIdHit;
    placeStartMode_ = false;
    return;
  }
  if (placeEndMode_ && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && nodeIdHit >= 0) {
    graphTargetId_ = nodeIdHit;
    placeEndMode_ = false;
    return;
  }
  if (placeStartMode_ || placeEndMode_) return;

  if (editorState_ == EditorState::EditingWeight) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
      if (editWeightFromId_ >= 0 && editWeightToId_ >= 0 && !editWeightBuffer_.empty()) {
        int w = std::stoi(editWeightBuffer_);
        if (w > 0) graph_->setEdgeWeight(editWeightFromId_, editWeightToId_, w);
      }
      editorState_ = EditorState::Idle;
      editWeightFromId_ = editWeightToId_ = -1;
      editWeightBuffer_.clear();
      return;
    }
    int k = GetCharPressed();
    if (k >= '0' && k <= '9') editWeightBuffer_ += static_cast<char>(k);
    if (IsKeyPressed(KEY_BACKSPACE) && !editWeightBuffer_.empty()) editWeightBuffer_.pop_back();
    return;
  }

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    if (editorState_ == EditorState::DraggingNode) {
      editorState_ = EditorState::Idle;
      dragNodeId_ = -1;
    }
    return;
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && nodeIdHit >= 0) {
    int id = nodeIdHit;
    graph_->removeNode(id);
    if (graphStartId_ == id || graphTargetId_ == id) {
      if (graph_->nodeCount() == 0)
        graphStartId_ = graphTargetId_ = -1;
      else {
        int firstId = graph_->nodes().front().id;
        if (graphStartId_ == id) graphStartId_ = firstId;
        if (graphTargetId_ == id) graphTargetId_ = firstId;
      }
    }
    if (dragNodeId_ == id) { editorState_ = EditorState::Idle; dragNodeId_ = -1; }
    if (ctrlSelectFirstId_ == id) { editorState_ = EditorState::Idle; ctrlSelectFirstId_ = -1; }
    if (editWeightFromId_ == id || editWeightToId_ == id) {
      editorState_ = EditorState::Idle;
      editWeightFromId_ = editWeightToId_ = -1;
      editWeightBuffer_.clear();
    }
    return;
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (ctrl) {
      if (weightLabelHit && gm == GraphMode::Weighted) {
        editorState_ = EditorState::EditingWeight;
        editWeightFromId_ = wFrom;
        editWeightToId_ = wTo;
        const Edge* e = graph_->getEdge(wFrom, wTo);
        editWeightBuffer_ = e ? std::to_string(e->weight) : "1";
        return;
      }
      if (nodeIdHit >= 0) {
        if (editorState_ == EditorState::CtrlSelectingEdge && ctrlSelectFirstId_ >= 0) {
          if (nodeIdHit != ctrlSelectFirstId_) {
            int childId = nodeIdHit;
            int newParentId = ctrlSelectFirstId_;
            int existingParentId = graph_->getParentId(childId);
            if (existingParentId >= 0 && existingParentId != newParentId) {
              graph_->removeEdge(existingParentId, childId);
              graphStatusMessage_ = "Node " + std::to_string(childId) + " already had parent " + std::to_string(existingParentId) + ". Re-parented to " + std::to_string(newParentId) + ".";
            } else {
              graphStatusMessage_.clear();
            }
            if (!graph_->hasEdge(newParentId, childId))
              graph_->addEdge(newParentId, childId, 1);
            editorState_ = EditorState::Idle;
            ctrlSelectFirstId_ = -1;
          }
          return;
        }
        editorState_ = EditorState::CtrlSelectingEdge;
        ctrlSelectFirstId_ = nodeIdHit;
        return;
      }
      if (editorState_ == EditorState::CtrlSelectingEdge) {
        editorState_ = EditorState::Idle;
        ctrlSelectFirstId_ = -1;
      }
      return;
    }

    if (nodeIdHit >= 0) {
      editorState_ = EditorState::DraggingNode;
      dragNodeId_ = nodeIdHit;
      Node* n = graph_->getNode(nodeIdHit);
      if (n) { dragStartX_ = n->x; dragStartY_ = n->y; }
      return;
    }

    if (editorState_ == EditorState::CtrlSelectingEdge) {
      editorState_ = EditorState::Idle;
      ctrlSelectFirstId_ = -1;
    }

    if (graph_->nodeCount() == 0) {
      graph_->addNode(nextNodeId_++, mx, my);
      graphStartId_ = nextNodeId_ - 1;
      graphTargetId_ = nextNodeId_ - 1;
    } else {
      graph_->addNode(nextNodeId_++, mx, my);
      if (graph_->nodeCount() == 2) graphTargetId_ = nextNodeId_ - 1;
    }
    return;
  }

  if (editorState_ == EditorState::DraggingNode && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && dragNodeId_ >= 0) {
    Node* n = graph_->getNode(dragNodeId_);
    if (n) {
      n->x = mx;
      n->y = my;
    }
  }
}

void PathfindingScene::handleInput() {
  if (IsKeyPressed(KEY_ESCAPE)) {
    if (mode_ == Mode::Graph && editorState_ != EditorState::Idle) {
      if (editorState_ == EditorState::EditingWeight) {
        editorState_ = EditorState::Idle;
        editWeightFromId_ = editWeightToId_ = -1;
        editWeightBuffer_.clear();
      } else if (editorState_ == EditorState::CtrlSelectingEdge) {
        editorState_ = EditorState::Idle;
        ctrlSelectFirstId_ = -1;
      } else if (editorState_ == EditorState::DraggingNode) {
        Node* n = graph_->getNode(dragNodeId_);
        if (n) { n->x = dragStartX_; n->y = dragStartY_; }
        editorState_ = EditorState::Idle;
        dragNodeId_ = -1;
      }
      return;
    }
    if (onBackToMenu_) onBackToMenu_();
    return;
  }
  if (running_ && IsKeyPressed(KEY_SPACE)) singleStep_ = true;
  if (running_ && IsKeyPressed(KEY_P)) animationRunning_ = !animationRunning_;

  float mx = static_cast<float>(GetMouseX());
  float my = static_cast<float>(GetMouseY());

  if (mode_ == Mode::Grid) {
    if (IsKeyPressed(KEY_ONE)) applyPreset("empty");
    if (IsKeyPressed(KEY_TWO)) applyPreset("random");
    if (IsKeyPressed(KEY_THREE)) applyPreset("maze");
  }
  handleTopBarInput(mx, my);
  if (mode_ == Mode::Grid && my >= GRID_OFFSET_Y) handleGridInput(mx, my);
  else if (mode_ == Mode::Graph) handleGraphInput(mx, my);
}

void PathfindingScene::update() {
  if (!running_ || !algorithm_) return;
  if (algorithm_->isFinished()) {
    if (algorithmName_ == "BFS") {
      auto* bfs = dynamic_cast<BFSAlgorithm*>(algorithm_.get());
      if (bfs) recorder_.pushFinalPath(bfs->getPath());
    } else if (algorithmName_ == "DFS") {
      auto* dfs = dynamic_cast<DFSAlgorithm*>(algorithm_.get());
      if (dfs) recorder_.pushFinalPath(dfs->getPath());
    } else if (algorithmName_ == "Dijkstra") {
      auto* d = dynamic_cast<DijkstraAlgorithm*>(algorithm_.get());
      if (d) recorder_.pushFinalPath(d->getPath());
    } else if (algorithmName_ == "A*") {
      auto* a = dynamic_cast<AStarAlgorithm*>(algorithm_.get());
      if (a) recorder_.pushFinalPath(a->getPath());
    }
    eventPlayer_.setCurrentIndex(eventPlayer_.getEventCount());
    running_ = false;
    return;
  }
  int delay = (speedIndex_ == 0) ? 8 : (speedIndex_ == 1) ? 2 : 0;
  stepDelayCounter_++;
  if (singleStep_ || (delay == 0) || (stepDelayCounter_ >= delay)) {
    stepDelayCounter_ = 0;
    algorithm_->step();
    recorder_.recordFromGraph(algoGraph_.get(), algoStartId_, algoTargetId_);
    eventPlayer_.setCurrentIndex(eventPlayer_.getEventCount());
    singleStep_ = false;
  }
}

void PathfindingScene::draw() {
  renderer_->beginFrame();
  drawTopBar();
  drawLegend();
  drawSharedMetrics();
  if (mode_ == Mode::Grid) drawGridContent();
  else {
    drawGraphContent();
    drawGraphEditorHints();
    drawWeightEditOverlay();
  }
  if (placeStartMode_) renderer_->drawText("Klik for start", viz::Vector2{20, GRID_OFFSET_Y - 18}, 14, C_START);
  if (placeEndMode_) renderer_->drawText("Klik for maal", viz::Vector2{20, GRID_OFFSET_Y - 18}, 14, C_END);
  if (mode_ == Mode::Grid) {
    renderer_->drawText("Presets: 1=Empty 2=Random 3=Maze | S/T+klik=start/maal | Træk=vaegge", viz::Vector2{WIN_W - 480, GRID_OFFSET_Y - 18}, 11, C_TEXT_DARK);
  }
  renderer_->endFrame();
}
