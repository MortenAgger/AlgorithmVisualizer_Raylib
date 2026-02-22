#include "MstScene.h"
#include "Application.h"
#include "mst/HitTest.h"
#include "mst/PrimMst.h"
#include "mst/KruskalMst.h"
#include "raylib.h"
#include <cmath>
#include <algorithm>
#include <sstream>

namespace {
constexpr viz::Color C_BG{252, 252, 252, 255};
constexpr viz::Color C_HEADER{50, 60, 80, 255};
constexpr viz::Color C_PANEL{240, 242, 245, 255};
constexpr viz::Color C_BTN{70, 85, 105, 255};
constexpr viz::Color C_BTN_HOVER{90, 110, 135, 255};
constexpr viz::Color C_TEXT{255, 255, 255, 255};
constexpr viz::Color C_TEXT_DARK{30, 30, 30, 255};
constexpr viz::Color C_NODE{70, 130, 180, 255};
constexpr viz::Color C_NODE_HOVER{100, 160, 210, 255};
constexpr viz::Color C_NODE_SELECT{255, 200, 80, 255};
constexpr viz::Color C_EDGE{120, 120, 130, 255};
constexpr viz::Color C_EDGE_DIMMED{200, 200, 205, 255};
constexpr viz::Color C_EDGE_ACCEPTED{50, 180, 90, 255};
constexpr viz::Color C_MST_EDGE{20, 200, 80, 255};
constexpr float MST_EDGE_THICK = 4.f;
constexpr viz::Color C_EDGE_REJECTED{220, 80, 80, 255};
constexpr viz::Color C_EDGE_CANDIDATE{80, 140, 220, 255};
constexpr viz::Color C_EDGE_CURRENT{255, 180, 50, 255};
constexpr viz::Color C_RUN{80, 160, 120, 255};
constexpr float BTN_H = 26.f;
}

MstScene::MstScene(IRenderer* renderer, Application* app)
  : renderer_(renderer), app_(app) {}

bool MstScene::inCanvas(float mx, float my) const {
  return mx >= CANVAS_LEFT && mx < CANVAS_LEFT + CANVAS_W && my >= CANVAS_TOP && my < CANVAS_TOP + CANVAS_H;
}

int MstScene::weightLabelEdgeAt(float mx, float my) const {
  const float labelW = 24.f;
  const float labelH = 18.f;
  for (const mst::Edge& e : graph_.edges()) {
    const mst::Node* na = graph_.getNode(e.a);
    const mst::Node* nb = graph_.getNode(e.b);
    if (!na || !nb) continue;
    float lx = (na->pos.x + nb->pos.x) * 0.5f - 10.f;
    float ly = (na->pos.y + nb->pos.y) * 0.5f - 12.f;
    if (mx >= lx && mx <= lx + labelW && my >= ly && my <= ly + labelH)
      return e.id;
  }
  return -1;
}

viz::Color MstScene::edgeColor(int edgeId) const {
  const mst::ViewState& vs = engine_.getViewState();
  for (int id : vs.acceptedEdges) if (id == edgeId) return C_EDGE_ACCEPTED;
  for (int id : vs.rejectedEdges) if (id == edgeId) return C_EDGE_REJECTED;
  for (int id : vs.candidateEdges) if (id == edgeId) return C_EDGE_CANDIDATE;
  if (vs.currentEdgeId == edgeId) return C_EDGE_CURRENT;
  if (selectedEdge_ == edgeId) return C_EDGE_CURRENT;
  return C_EDGE;
}

viz::Color MstScene::nodeColor(int nodeId) const {
  if (editorState_ == MstEditorState::CtrlSelectingEdge && nodeId == ctrlSelectFirstId_) return C_NODE_SELECT;
  const mst::ViewState& vs = engine_.getViewState();
  if (vs.currentNodeId == nodeId && engine_.getStepCount() > 0) return C_NODE_SELECT;
  if (selectedNode_ == nodeId) return C_NODE_SELECT;
  if (hoverNode_ == nodeId) return C_NODE_HOVER;
  return C_NODE;
}

void MstScene::drawTopBar() {
  renderer_->drawRectangle(0, 0, WIN_W, TOPBAR_H, C_HEADER);
  float y = (TOPBAR_H - BTN_H) * 0.5f;
  float x = 12;
  renderer_->drawText("MST Visualizer", viz::Vector2{x, y + 2}, 16, C_TEXT);
  x = 160;
  const char* algos[] = {"Prim", "Kruskal"};
  for (int i = 0; i < 2; ++i) {
    float w = 70.f;
    viz::Color col = (algoIndex_ == i) ? C_BTN_HOVER : C_BTN;
    renderer_->drawRectangle(x, y, w, BTN_H, col);
    renderer_->drawText(algos[i], viz::Vector2{x + 8, y + 5}, 12, C_TEXT);
    x += w + 8;
  }
  x += 8;
  renderer_->drawRectangle(x, y, 42.f, BTN_H, C_BTN);
  renderer_->drawText("Step", viz::Vector2{x + 8, y + 5}, 11, C_TEXT);
  x += 50;
  renderer_->drawRectangle(x, y, 42.f, BTN_H, C_BTN);
  renderer_->drawText("Back", viz::Vector2{x + 6, y + 5}, 11, C_TEXT);
  x += 50;
  renderer_->drawRectangle(x, y, 50.f, BTN_H, C_BTN);
  renderer_->drawText(playing_ ? "Pause" : "Play", viz::Vector2{x + 8, y + 5}, 11, C_TEXT);
  x += 58;
  renderer_->drawText("Speed", viz::Vector2{x, y + 5}, 11, C_TEXT);
  x += 38;
  renderer_->drawRectangle(x, y, 60.f, BTN_H, C_BTN);
  renderer_->drawText(std::to_string(speed_) + " (klik)", viz::Vector2{x + 8, y + 5}, 10, C_TEXT);
  x += 88;
  renderer_->drawRectangle(x, y, 52.f, BTN_H, viz::Color{180, 80, 80, 255});
  renderer_->drawText("Clear", viz::Vector2{x + 12, y + 5}, 11, C_TEXT);
}

void MstScene::drawLeftPanel() {
  renderer_->drawRectangle(0, TOPBAR_H, LEFT_PANEL_W, WIN_H - TOPBAR_H, C_PANEL);
  float y = TOPBAR_H + 16;
  renderer_->drawText("Redigering", viz::Vector2{12, y}, 14, C_TEXT_DARK);
  y += 24;
  renderer_->drawText("Klik = placere node", viz::Vector2{12, y}, 11, C_TEXT_DARK);
  y += 18;
  renderer_->drawText("Træk node = flyt", viz::Vector2{12, y}, 11, C_TEXT_DARK);
  y += 18;
  renderer_->drawText("Ctrl+klik to noder = kant", viz::Vector2{12, y}, 11, C_TEXT_DARK);
  y += 18;
  renderer_->drawText("Ctrl+klik vægt = rediger", viz::Vector2{12, y}, 11, C_TEXT_DARK);
  y += 18;
  renderer_->drawText("Højreklik node/kant = fjern", viz::Vector2{12, y}, 11, C_TEXT_DARK);
  y += 28;
  renderer_->drawText("Prim: Start node", viz::Vector2{12, y}, 12, C_TEXT_DARK);
  y += 20;
  renderer_->drawRectangle(12, y, 156, BTN_H, C_BTN);
  renderer_->drawText("S eller Pick: " + (graph_.hasNode(startNodeId_) ? std::to_string(startNodeId_) : "ingen"), viz::Vector2{20, y + 5}, 10, C_TEXT);
}

void MstScene::drawRightPanel() {
  float rx = WIN_W - RIGHT_PANEL_W;
  renderer_->drawRectangle(rx, TOPBAR_H, RIGHT_PANEL_W, WIN_H - TOPBAR_H, C_PANEL);
  float y = TOPBAR_H + 16;
  renderer_->drawText("Explanation", viz::Vector2{rx + 12, y}, 14, C_TEXT_DARK);
  y += 22;
  std::string expl = engine_.getViewState().explanation;
  if (expl.empty()) expl = "Run Prim or Kruskal to start.";
  renderer_->drawText(expl, viz::Vector2{rx + 12, y}, 12, C_TEXT_DARK);
  y += 40;
  const mst::ViewState& vs = engine_.getViewState();
  renderer_->drawText("Total weight: " + std::to_string(vs.totalWeight), viz::Vector2{rx + 12, y}, 12, C_TEXT_DARK);
  y += 28;
  renderer_->drawText("MST edges (accepted)", viz::Vector2{rx + 12, y}, 12, C_TEXT_DARK);
  y += 18;
  for (size_t i = 0; i < vs.acceptedEdges.size() && i < 14; ++i) {
    const mst::Edge* e = graph_.getEdge(vs.acceptedEdges[i]);
    if (e) {
      std::string line = std::to_string(e->a) + "-" + std::to_string(e->b) + " w=" + std::to_string(e->weight);
      renderer_->drawText(line, viz::Vector2{rx + 12, y}, 11, C_TEXT_DARK);
    }
    y += 16;
  }
  if (algoIndex_ == 1) {
    y += 16;
    renderer_->drawText("All edges (by weight)", viz::Vector2{rx + 12, y}, 12, C_TEXT_DARK);
    y += 18;
    std::vector<std::pair<int, int>> sorted;
    for (const mst::Edge& e : graph_.edges()) sorted.push_back({e.weight, e.id});
    std::sort(sorted.begin(), sorted.end());
    for (size_t i = 0; i < sorted.size() && i < 10; ++i) {
      const mst::Edge* e = graph_.getEdge(sorted[i].second);
      if (e) {
        std::string line = std::to_string(e->a) + "-" + std::to_string(e->b) + " w=" + std::to_string(e->weight);
        renderer_->drawText(line, viz::Vector2{rx + 12, y}, 11, C_TEXT_DARK);
      }
      y += 16;
    }
  }
}

void MstScene::drawCanvas() {
  renderer_->drawRectangle(CANVAS_LEFT, CANVAS_TOP, CANVAS_W, CANVAS_H, C_BG);
  const mst::ViewState& vs = engine_.getViewState();
  auto isAccepted = [&vs](int eid) {
    for (int id : vs.acceptedEdges) if (id == eid) return true;
    return false;
  };
  for (const mst::Edge& e : graph_.edges()) {
    const mst::Node* na = graph_.getNode(e.a);
    const mst::Node* nb = graph_.getNode(e.b);
    if (!na || !nb) continue;
    viz::Color col = edgeColor(e.id);
    if (!isAccepted(e.id) && vs.acceptedEdges.size() > 0)
      col = C_EDGE_DIMMED;
    renderer_->drawEdge(na->pos, nb->pos, col);
    float mx = (na->pos.x + nb->pos.x) * 0.5f - 8;
    float my = (na->pos.y + nb->pos.y) * 0.5f - 10;
    renderer_->drawRectangle(mx - 2, my - 2, 24, 18, C_BG);
    renderer_->drawText(std::to_string(e.weight), viz::Vector2{mx, my}, 12, C_TEXT_DARK);
  }
  for (int eid : vs.acceptedEdges) {
    const mst::Edge* e = graph_.getEdge(eid);
    if (!e) continue;
    const mst::Node* na = graph_.getNode(e->a);
    const mst::Node* nb = graph_.getNode(e->b);
    if (!na || !nb) continue;
    renderer_->drawEdgeThick(na->pos, nb->pos, C_MST_EDGE, MST_EDGE_THICK);
  }
  for (const mst::Node& n : graph_.nodes()) {
    viz::Color col = nodeColor(n.id);
    renderer_->drawNode(n.pos, NODE_R, col);
    renderer_->drawText(std::to_string(n.id), viz::Vector2{n.pos.x - 6, n.pos.y - 8}, 14, C_TEXT);
  }
}

void MstScene::drawWeightEditOverlay() {
  if (!weightEdit_ || weightEditEdgeId_ < 0) return;
  float cx = WIN_W * 0.5f - 120;
  float cy = WIN_H * 0.5f - 40;
  renderer_->drawRectangle(cx, cy, 240, 80, C_PANEL);
  renderer_->drawText("Edge weight (integer > 0)", viz::Vector2{cx + 12, cy + 8}, 14, C_TEXT_DARK);
  renderer_->drawText(weightEditBuffer_.empty() ? "_" : weightEditBuffer_, viz::Vector2{cx + 12, cy + 36}, 18, C_TEXT_DARK);
  renderer_->drawText("Enter=OK  Esc=cancel", viz::Vector2{cx + 12, cy + 62}, 11, C_TEXT_DARK);
}

void MstScene::runAlgorithm() {
  if (graph_.nodeCount() == 0) {
    engine_.setSteps({});
    return;
  }
  if (algoIndex_ == 0 && !graph_.getNode(startNodeId_))
    startNodeId_ = graph_.nodes()[0].id;
  if (algoIndex_ == 0)
    algorithm_ = std::make_unique<mst::PrimMst>();
  else
    algorithm_ = std::make_unique<mst::KruskalMst>();
  auto steps = algorithm_->generateSteps(graph_, startNodeId_);
  engine_.setSteps(steps);
}

void MstScene::clearGraph(bool) {
  graph_.clear();
  engine_.setSteps({});
  playing_ = false;
  selectedNode_ = selectedEdge_ = -1;
  ctrlSelectFirstId_ = -1;
  dragNodeId_ = -1;
  editorState_ = MstEditorState::Idle;
  weightEdit_ = false;
  weightEditEdgeId_ = -1;
  weightEditBuffer_.clear();
  startNodeId_ = 0;
}

void MstScene::handleInput() {
  if (IsKeyPressed(KEY_ESCAPE)) {
    if (weightEdit_) {
      weightEdit_ = false;
      weightEditEdgeId_ = -1;
      weightEditBuffer_.clear();
      editorState_ = MstEditorState::Idle;
    } else if (editorState_ != MstEditorState::Idle) {
      editorState_ = MstEditorState::Idle;
      ctrlSelectFirstId_ = -1;
      dragNodeId_ = -1;
    } else if (onBackToMenu_) onBackToMenu_();
    return;
  }
  shiftHeld_ = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
  float mx = static_cast<float>(GetMouseX());
  float my = static_cast<float>(GetMouseY());

  if (my >= 0.f && my < TOPBAR_H && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    int ix = static_cast<int>(mx);
    int iy = static_cast<int>(my);
    auto hit = [iy, ix](float left, float width) {
      return iy >= 0 && iy < static_cast<int>(TOPBAR_H) && ix >= static_cast<int>(left) && ix < static_cast<int>(left + width);
    };
    float x = 160.f;
    const float algoW = 70.f, algoGap = 8.f;
    if (hit(x, algoW)) {
      if (algoIndex_ != 0) { algoIndex_ = 0; playing_ = false; runAlgorithm(); }
      return;
    }
    x += algoW + algoGap;
    if (hit(x, algoW)) {
      if (algoIndex_ != 1) { algoIndex_ = 1; playing_ = false; runAlgorithm(); }
      return;
    }
    x += algoW + algoGap;
    x += 8.f;
    if (hit(x, 42.f)) { if (engine_.canStepForward()) engine_.stepForward(); return; }
    x += 50.f;
    if (hit(x, 42.f)) { if (engine_.canStepBackward()) engine_.stepBackward(); return; }
    x += 50.f;
    if (hit(x, 50.f)) {
      if (graph_.nodeCount() > 0 && engine_.getStepCount() == 0) runAlgorithm();
      playing_ = !playing_;
      return;
    }
    x += 58.f;
    x += 38.f;
    if (hit(x, 60.f)) { speedIndex_ = (speedIndex_ + 1) % static_cast<int>(speedOptions_.size()); speed_ = speedOptions_[speedIndex_]; return; }
    x += 88.f;
    if (hit(x, 52.f)) { clearGraph(true); return; }
    return;
  }
  if (my < TOPBAR_H) return;

  if (IsKeyPressed(KEY_S)) { pickStartMode_ = true; return; }
  if (weightEdit_) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
      if (weightEditEdgeId_ >= 0 && !weightEditBuffer_.empty()) {
        int w = std::stoi(weightEditBuffer_);
        if (w > 0) graph_.setEdgeWeight(weightEditEdgeId_, w);
      }
      weightEdit_ = false;
      weightEditEdgeId_ = -1;
      weightEditBuffer_.clear();
      editorState_ = MstEditorState::Idle;
    }
    int k = GetCharPressed();
    if (k >= '0' && k <= '9') weightEditBuffer_ += static_cast<char>(k);
    if (IsKeyPressed(KEY_BACKSPACE) && !weightEditBuffer_.empty()) weightEditBuffer_.pop_back();
    return;
  }

  if (mx < LEFT_PANEL_W && my >= TOPBAR_H) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      float y = TOPBAR_H + 168;
      if (my >= y && my < y + BTN_H) pickStartMode_ = true;
    }
    return;
  }

  if (!inCanvas(mx, my)) return;

  hoverNode_ = mst::hitTestNode(graph_, mx, my, NODE_R);
  hoverEdge_ = mst::hitTestEdge(graph_, mx, my, EDGE_HIT_THRESH);
  int weightLabelEdgeId = weightLabelEdgeAt(mx, my);
  bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

  if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
    if (editorState_ == MstEditorState::DraggingNode) {
      editorState_ = MstEditorState::Idle;
      dragNodeId_ = -1;
    }
    return;
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    if (ctrl) {
      if (weightLabelEdgeId >= 0) {
        weightEdit_ = true;
        weightEditEdgeId_ = weightLabelEdgeId;
        const mst::Edge* e = graph_.getEdge(weightLabelEdgeId);
        weightEditBuffer_ = e ? std::to_string(e->weight) : "1";
        editorState_ = MstEditorState::EditingWeight;
        return;
      }
      if (hoverNode_ >= 0) {
        if (editorState_ == MstEditorState::CtrlSelectingEdge && ctrlSelectFirstId_ >= 0) {
          if (hoverNode_ != ctrlSelectFirstId_ && !graph_.hasEdge(ctrlSelectFirstId_, hoverNode_)) {
            int eid = graph_.nextEdgeId();
            graph_.addEdge(eid, ctrlSelectFirstId_, hoverNode_, 1);
          }
          editorState_ = MstEditorState::Idle;
          ctrlSelectFirstId_ = -1;
          return;
        }
        editorState_ = MstEditorState::CtrlSelectingEdge;
        ctrlSelectFirstId_ = hoverNode_;
        return;
      }
      if (editorState_ == MstEditorState::CtrlSelectingEdge) {
        editorState_ = MstEditorState::Idle;
        ctrlSelectFirstId_ = -1;
      }
      return;
    }

    if (hoverNode_ >= 0) {
      if (pickStartMode_) {
        startNodeId_ = hoverNode_;
        pickStartMode_ = false;
        return;
      }
      editorState_ = MstEditorState::DraggingNode;
      dragNodeId_ = hoverNode_;
      selectedNode_ = hoverNode_;
      selectedEdge_ = -1;
      if (algoIndex_ == 0) startNodeId_ = hoverNode_;
      dragStartX_ = mx;
      dragStartY_ = my;
      return;
    }
    if (pickStartMode_) pickStartMode_ = false;

    if (editorState_ == MstEditorState::CtrlSelectingEdge) {
      editorState_ = MstEditorState::Idle;
      ctrlSelectFirstId_ = -1;
    }

    if (hoverEdge_ < 0 && hoverNode_ < 0) {
      int id = graph_.nextNodeId();
      graph_.addNode(id, mx, my);
      if (graph_.nodeCount() == 1) startNodeId_ = id;
    }
    return;
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
    if (hoverNode_ >= 0) {
      graph_.removeNode(hoverNode_);
      selectedNode_ = selectedEdge_ = -1;
      if (ctrlSelectFirstId_ == hoverNode_) { ctrlSelectFirstId_ = -1; editorState_ = MstEditorState::Idle; }
      if (dragNodeId_ == hoverNode_) dragNodeId_ = -1;
      if (startNodeId_ == hoverNode_ && graph_.nodeCount() > 0) startNodeId_ = graph_.nodes()[0].id;
      else if (graph_.nodeCount() == 0) startNodeId_ = 0;
      return;
    }
    if (hoverEdge_ >= 0) {
      graph_.removeEdge(hoverEdge_);
      selectedEdge_ = -1;
      return;
    }
  }

  if (editorState_ == MstEditorState::DraggingNode && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && dragNodeId_ >= 0) {
    mst::Node* n = graph_.getNode(dragNodeId_);
    if (n) {
      n->pos.x += mx - dragStartX_;
      n->pos.y += my - dragStartY_;
      dragStartX_ = mx;
      dragStartY_ = my;
    }
  }
}

void MstScene::update() {
  if (playing_ && engine_.canStepForward()) {
    playAccum_ += GetFrameTime() * speed_;
    while (playAccum_ >= 0.5f && engine_.canStepForward()) {
      engine_.stepForward();
      playAccum_ -= 0.5f;
    }
  }
}

void MstScene::draw() {
  renderer_->beginFrame();
  drawTopBar();
  drawLeftPanel();
  drawRightPanel();
  drawCanvas();
  drawWeightEditOverlay();
  renderer_->drawText("Ctrl+klik noder = forbind | Ctrl+klik vægt = rediger | Højreklik = fjern | S = vælg Prim-start | Esc = tilbage", viz::Vector2{CANVAS_LEFT, WIN_H - 22}, 11, C_TEXT_DARK);
  renderer_->endFrame();
}
