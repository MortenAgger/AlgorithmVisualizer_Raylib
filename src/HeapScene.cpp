#include "HeapScene.h"
#include "Application.h"
#include "raylib.h"
#include <cmath>
#include <string>
#include <sstream>
#include <cctype>

namespace {
constexpr viz::Color C_NODE{70, 130, 180, 255};
constexpr viz::Color C_HIGHLIGHT{255, 200, 80, 255};
constexpr viz::Color C_COMPARE{180, 220, 255, 255};
constexpr viz::Color C_TEXT{255, 255, 255, 255};
constexpr viz::Color C_LABEL{40, 40, 40, 255};
constexpr viz::Color C_LINE{150, 150, 150, 255};
constexpr viz::Color C_PANEL_BG{240, 240, 245, 255};
constexpr viz::Color C_BTN{70, 85, 105, 255};
constexpr float START_DX = 200.f;
}

HeapScene::HeapScene(IRenderer* renderer, Application* app)
  : renderer_(renderer), app_(app), controller_(model_, player_) {
  player_.setSpeed(2.f);
  speed_ = 2.f;
}

std::vector<int> HeapScene::parseValues(const std::string& s) {
  std::vector<int> out;
  std::stringstream ss(s);
  std::string part;
  while (std::getline(ss, part, ',')) {
    while (!part.empty() && std::isspace(static_cast<unsigned char>(part.front()))) part.erase(0, 1);
    while (!part.empty() && std::isspace(static_cast<unsigned char>(part.back()))) part.pop_back();
    if (part.empty()) continue;
    try {
      out.push_back(std::stoi(part));
    } catch (...) {}
  }
  return out;
}

std::pair<float, float> HeapScene::getNodePosition(size_t index) const {
  if (index == 0) return {TREE_CENTER_X, TREE_TOP_Y};
  int depth = 0;
  size_t t = index;
  while (t > 0) { t = (t - 1) / 2; depth++; }
  float dx = START_DX;
  for (int d = 0; d < depth; d++) dx *= 0.5f;
  float x = TREE_CENTER_X, y = TREE_TOP_Y;
  int path = static_cast<int>(index);
  for (int level = 0; level < depth; level++) {
    int goRight = (path % 2 == 0) ? 1 : -1;
    x += static_cast<float>(goRight) * dx;
    y += LEVEL_DY;
    path = (path - 1) / 2;
  }
  return {x, y};
}

void HeapScene::drawTreeView() {
  if (model_.empty()) return;
  // Tegn kun én kant ind til hver node (fra forælder til barn), så ingen node får to forældre
  for (size_t j = 1; j < model_.size(); ++j) {
    size_t parent = (j - 1) / 2;
    auto [ax, ay] = getNodePosition(parent);
    auto [bx, by] = getNodePosition(j);
    renderer_->drawEdge(viz::Vector2{ax, ay}, viz::Vector2{bx, by}, C_LINE);
  }
  for (size_t i = 0; i < model_.size(); ++i) {
    auto [nx, ny] = getNodePosition(i);
    viz::Color col = colorForNode(i);
    renderer_->drawNode(viz::Vector2{nx, ny}, NODE_R, col);
    renderer_->drawText(std::to_string(model_.get(i)), viz::Vector2{nx - 8, ny - 10}, 18, C_TEXT);
  }
}

void HeapScene::drawArrayView() {
  size_t n = model_.size();
  float totalW = n * (ARRAY_CELL_W + 4);
  float startX = 512.f - totalW * 0.5f;
  for (size_t i = 0; i < n; ++i) {
    float x = startX + i * (ARRAY_CELL_W + 4);
    viz::Color col = colorForNode(i);
    renderer_->drawRectangle(x, ARRAY_Y, ARRAY_CELL_W, 32, col);
    renderer_->drawText(std::to_string(model_.get(i)), viz::Vector2{x + 8, ARRAY_Y + 6}, 16, C_TEXT);
    renderer_->drawText(std::to_string(i), viz::Vector2{x + 4, ARRAY_Y - 16}, 12, C_LABEL);
  }
}

constexpr float INSERT_VAL_X = HeapScene::PANEL_X + 58.f;
constexpr float INSERT_VAL_Y = HeapScene::PANEL_Y + 34.f;
constexpr float INSERT_VAL_W = 52.f;
constexpr float INSERT_VAL_H = 22.f;

bool HeapScene::insertValueHit(float mx, float my) const {
  // Lidt større ramme så værdiboksen er nem at ramme (ctrl+klik eller almindeligt klik)
  const float margin = 2.f;
  return buttonHit(mx, my, INSERT_VAL_X - margin, INSERT_VAL_Y - margin, INSERT_VAL_W + 2.f * margin, INSERT_VAL_H + 2.f * margin);
}

// Array input rect: samme Y som i drawPanel (efter "Array"-label +18). Lidt margin så det er nemt at ramme.
bool HeapScene::arrayInputHit(float mx, float my) const {
  const float ay = PANEL_Y + 252;
  const float margin = 3.f;
  return buttonHit(mx, my, PANEL_X + 10 - margin, ay - margin, 120 + 2.f * margin, 24 + 2.f * margin);
}

void HeapScene::drawPanel() {
  // Height: array row + Speed row (by ender ved 324+28+18+24 = 394), så 394 - 72 + lidt = 324
  renderer_->drawRectangle(PANEL_X, PANEL_Y, 200, 324, C_PANEL_BG);
  renderer_->drawText(isMaxHeap_ ? "Max Heap" : "Min Heap", viz::Vector2{PANEL_X + 10, PANEL_Y + 8}, 18, C_LABEL);
  renderer_->drawText("Insert:", viz::Vector2{PANEL_X + 10, PANEL_Y + 38}, 14, C_LABEL);
  renderer_->drawRectangle(INSERT_VAL_X, INSERT_VAL_Y, INSERT_VAL_W, INSERT_VAL_H, C_BTN);
  renderer_->drawText(editingInsertValue_ ? (insertValueBuffer_.empty() ? "_" : insertValueBuffer_) : std::to_string(insertValueEdit_), viz::Vector2{INSERT_VAL_X + 6, INSERT_VAL_Y + 2}, 14, C_TEXT);
  renderer_->drawText("Ctrl+klik=vaerdi", viz::Vector2{PANEL_X + 116, PANEL_Y + 38}, 10, C_LABEL);
  float by = PANEL_Y + 64;
  renderer_->drawRectangle(PANEL_X + 10, by, 80, 28, C_BTN);
  renderer_->drawText("Insert", viz::Vector2{PANEL_X + 24, by + 6}, 14, C_TEXT);
  by += 34;
  renderer_->drawRectangle(PANEL_X + 10, by, 100, 28, C_BTN);
  renderer_->drawText("DeleteRoot", viz::Vector2{PANEL_X + 14, by + 6}, 12, C_TEXT);
  by += 34;
  renderer_->drawRectangle(PANEL_X + 10, by, 80, 28, C_BTN);
  renderer_->drawText("Heapify", viz::Vector2{PANEL_X + 22, by + 6}, 14, C_TEXT);
  by += 34;
  renderer_->drawRectangle(PANEL_X + 10, by, 72, 28, C_BTN);
  renderer_->drawText("Min", viz::Vector2{PANEL_X + 28, by + 6}, 14, C_TEXT);
  renderer_->drawRectangle(PANEL_X + 88, by, 72, 28, C_BTN);
  renderer_->drawText("Max", viz::Vector2{PANEL_X + 108, by + 6}, 14, C_TEXT);
  by += 34;
  renderer_->drawRectangle(PANEL_X + 10, by, 90, 28, viz::Color{180, 80, 80, 255});
  renderer_->drawText("Clear Heap", viz::Vector2{PANEL_X + 14, by + 6}, 12, C_TEXT);
  by += 34;
  renderer_->drawText("Array (tal med komma):", viz::Vector2{PANEL_X + 10, by}, 10, C_LABEL);
  by += 18;
  renderer_->drawRectangle(PANEL_X + 10, by, 120, 24, C_BTN);
  std::string arrDisplay = buildArrayInput_.empty() ? "5, 2, 9, 1, 7" : buildArrayInput_;
  if (arrDisplay.length() > 18u) arrDisplay = arrDisplay.substr(0, 15) + "...";
  renderer_->drawText(arrDisplay, viz::Vector2{PANEL_X + 14, by + 2}, 11, C_TEXT);
  renderer_->drawRectangle(PANEL_X + 134, by, 58, 24, viz::Color{60, 100, 80, 255});
  renderer_->drawText("Indsaet", viz::Vector2{PANEL_X + 138, by + 2}, 10, C_TEXT);
  by += 28;
  renderer_->drawText("Speed:", viz::Vector2{PANEL_X + 10, by}, 11, C_LABEL);
  renderer_->drawText(std::to_string(static_cast<int>(speed_ * 10) / 10.f), viz::Vector2{PANEL_X + 52, by}, 11, C_LABEL);
  by += 18;
  renderer_->drawRectangle(PANEL_X + 10, by, 36, 24, C_BTN);
  renderer_->drawText("+", viz::Vector2{PANEL_X + 20, by + 2}, 14, C_TEXT);
  renderer_->drawRectangle(PANEL_X + 52, by, 36, 24, C_BTN);
  renderer_->drawText("-", viz::Vector2{PANEL_X + 62, by + 2}, 14, C_TEXT);
}

void HeapScene::drawStepHighlight() {
  const heap::HeapStep* step = player_.getCurrentStep();
  if (!step) return;
  if (step->type == heap::StepType::Compare || step->type == heap::StepType::Swap) {
    auto [x0, y0] = getNodePosition(step->i);
    auto [x1, y1] = getNodePosition(step->j);
    renderer_->drawNode(viz::Vector2{x0, y0}, NODE_R + 4, C_COMPARE);
    renderer_->drawNode(viz::Vector2{x1, y1}, NODE_R + 4, C_COMPARE);
  }
}

viz::Color HeapScene::colorForNode(size_t index) const {
  const heap::HeapStep* step = player_.getCurrentStep();
  if (step && (step->i == index || step->j == index)) {
    if (step->type == heap::StepType::Compare) return C_COMPARE;
    if (step->type == heap::StepType::Swap) return C_HIGHLIGHT;
  }
  return C_NODE;
}

bool HeapScene::buttonHit(float mx, float my, float bx, float by, float bw, float bh) const {
  // Pixel-based hit: same as drawn rect (right/bottom exclusive)
  int px = static_cast<int>(mx), py = static_cast<int>(my);
  int x0 = static_cast<int>(bx), y0 = static_cast<int>(by);
  int x1 = x0 + static_cast<int>(bw), y1 = y0 + static_cast<int>(bh);
  return px >= x0 && px < x1 && py >= y0 && py < y1;
}

int HeapScene::hitTestTree(float mx, float my) const {
  for (size_t i = 0; i < model_.size(); ++i) {
    auto [x, y] = getNodePosition(i);
    float dx = mx - x, dy = my - y;
    if (dx*dx + dy*dy <= NODE_R * NODE_R) return static_cast<int>(i);
  }
  return -1;
}

int HeapScene::hitTestArray(float mx, float my) const {
  size_t n = model_.size();
  float totalW = n * (ARRAY_CELL_W + 4);
  float startX = 512.f - totalW * 0.5f;
  if (my < ARRAY_Y || my > ARRAY_Y + 32) return -1;
  for (size_t i = 0; i < n; ++i) {
    float x = startX + i * (ARRAY_CELL_W + 4);
    if (mx >= x && mx <= x + ARRAY_CELL_W) return static_cast<int>(i);
  }
  return -1;
}

void HeapScene::setMaxHeap(bool maxHeap) {
  isMaxHeap_ = maxHeap;
  controller_.setMaxHeap(maxHeap);
}

void HeapScene::handleInput() {
  if (IsKeyPressed(KEY_ESCAPE)) {
    if (editingInsertValue_) { editingInsertValue_ = false; return; }
    if (buildInputFocus_) { buildInputFocus_ = false; return; }
    if (editingNode_) { editingNode_ = false; editIndex_ = -1; }
    else if (onBackToMenu_) onBackToMenu_();
    return;
  }
  float mx = static_cast<float>(GetMouseX());
  float my = static_cast<float>(GetMouseY());

  // Klik eller ctrl+klik på værdiboksen: rediger indsættelsesværdi
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && insertValueHit(mx, my)) {
    editingInsertValue_ = true;
    insertValueBuffer_ = std::to_string(insertValueEdit_);
    return;
  }
  // Klik eller ctrl+klik på array-feltet: rediger array (skriv egne tal)
  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && arrayInputHit(mx, my)) {
    buildInputFocus_ = true;
    return;
  }

  if (editingInsertValue_) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
      if (!insertValueBuffer_.empty()) {
        try { insertValueEdit_ = std::stoi(insertValueBuffer_); } catch (...) {}
      }
      editingInsertValue_ = false;
      insertValueBuffer_.clear();
      return;
    }
    if (IsKeyPressed(KEY_BACKSPACE) && !insertValueBuffer_.empty()) insertValueBuffer_.pop_back();
    int key;
    while ((key = GetCharPressed()) != 0) {
      if (key >= '0' && key <= '9') insertValueBuffer_ += static_cast<char>(key);
      else if (key == '-' && insertValueBuffer_.empty()) insertValueBuffer_ = '-';
    }
    return;
  }

  if (buildInputFocus_) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
      std::string input = buildArrayInput_.empty() ? "10,3,7,1" : buildArrayInput_;
      std::vector<int> vals = parseValues(input);
      if (!vals.empty()) {
        controller_.execute(heap::SetArrayCommand{vals});
        statusMessage_.clear();
      }
      buildInputFocus_ = false;
      return;
    }
    if (IsKeyPressed(KEY_ESCAPE)) { buildInputFocus_ = false; return; }
    if (IsKeyPressed(KEY_BACKSPACE) && !buildArrayInput_.empty()) buildArrayInput_.pop_back();
    int key;
    while ((key = GetCharPressed()) != 0) {
      if ((key >= '0' && key <= '9') || key == ',' || key == ' ') buildArrayInput_ += static_cast<char>(key);
      else if (key == '-' && (buildArrayInput_.empty() || buildArrayInput_.back() == ',' || buildArrayInput_.back() == ' ')) buildArrayInput_ += '-';
    }
    return;
  }

  if (editingNode_) {
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
      if (editIndex_ >= 0 && !editBuffer_.empty()) {
        int v = std::stoi(editBuffer_);
        controller_.execute(heap::UpdateNodeValueCommand{static_cast<size_t>(editIndex_), v});
        while (player_.canStepForward()) controller_.applyCurrentStepAndAdvance();
      }
      editingNode_ = false;
      editIndex_ = -1;
      editBuffer_.clear();
    }
    int key = GetCharPressed();
    if (key >= '0' && key <= '9') editBuffer_ += static_cast<char>(key);
    if (key == '-' && editBuffer_.empty()) editBuffer_ += '-';
    if (IsKeyPressed(KEY_BACKSPACE) && !editBuffer_.empty()) editBuffer_.pop_back();
    return;
  }

  if (IsKeyPressed(KEY_SPACE) && player_.canStepForward()) controller_.applyCurrentStepAndAdvance();
  if (IsKeyPressed(KEY_P)) player_.setPlaying(!player_.isPlaying());
  if (IsKeyPressed(KEY_LEFT_BRACKET) && player_.canStepBackward()) {
    player_.stepBackward();
    controller_.restoreAndReplayTo(player_.getCurrentIndex());
  }
  if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
    while (player_.canStepForward()) controller_.applyCurrentStepAndAdvance();
  }
  if (IsKeyPressed(KEY_BACKSLASH)) {
    controller_.restoreSnapshotAndResetPlayer();
  }

  auto applyAllSteps = [this]() {
    while (player_.canStepForward()) controller_.applyCurrentStepAndAdvance();
  };

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    // Insert: apply all steps immediately so the new node is visible
    if (buttonHit(mx, my, PANEL_X + 8, PANEL_Y + 62, 84, 32)) {
      controller_.execute(heap::InsertCommand{insertValueEdit_});
      applyAllSteps();
      return;
    }
    if (buttonHit(mx, my, PANEL_X + 10, PANEL_Y + 98, 100, 28)) {
      if (model_.empty()) statusMessage_ = "Heap er tom.";
      else {
        controller_.execute(heap::DeleteRootCommand{});
        applyAllSteps();
        statusMessage_.clear();
      }
      return;
    }
    if (buttonHit(mx, my, PANEL_X + 10, PANEL_Y + 132, 80, 28)) {
      controller_.execute(heap::HeapifyCommand{});
      applyAllSteps();
      return;
    }
    if (buttonHit(mx, my, PANEL_X + 10, PANEL_Y + 166, 72, 28)) {
      setMaxHeap(false);
      return;
    }
    if (buttonHit(mx, my, PANEL_X + 88, PANEL_Y + 166, 72, 28)) {
      setMaxHeap(true);
      return;
    }
    // Clear Heap: slightly larger hit area for reliability
    if (buttonHit(mx, my, PANEL_X + 8, PANEL_Y + 198, 94, 32)) {
      controller_.execute(heap::ClearHeapCommand{});
      statusMessage_.clear();
      return;
    }
    // Indsæt array-knap tjekkes FØRST (samme række som array-felt, så klik her = indsæt)
    const float arrayRowY = PANEL_Y + 252.f;
    if (buttonHit(mx, my, PANEL_X + 132, arrayRowY - 2, 62, 28)) {
      std::string input = buildArrayInput_.empty() ? "5, 2, 9, 1, 7" : buildArrayInput_;
      std::vector<int> vals = parseValues(input);
      if (!vals.empty()) {
        controller_.execute(heap::SetArrayCommand{vals});
        statusMessage_ = "Indsat " + std::to_string(vals.size()) + " tal. Tryk Heapify for at bygge heap.";
      } else {
        statusMessage_ = "Skriv tal med komma (fx 5, 2, 9) og klik array-feltet først.";
      }
      return;
    }
    // Array input rect: klik = rediger array (tal med komma)
    if (buttonHit(mx, my, PANEL_X + 10, arrayRowY, 120, 24)) {
      buildInputFocus_ = true;
      return;
    }
    // Speed +/-: lige under array-rækken (by = PANEL_Y+252+28+18 = 298+72 = 370 for knapperne)
    const float speedRowY = PANEL_Y + 298.f;
    if (buttonHit(mx, my, PANEL_X + 10, speedRowY, 36, 24)) {
      speed_ = std::min(10.f, speed_ + 0.5f);
      player_.setSpeed(speed_);
      return;
    }
    if (buttonHit(mx, my, PANEL_X + 52, speedRowY, 36, 24)) {
      speed_ = std::max(0.5f, speed_ - 0.5f);
      player_.setSpeed(speed_);
      return;
    }
    int treeIdx = hitTestTree(mx, my);
    int arrayIdx = hitTestArray(mx, my);
    int idx = treeIdx >= 0 ? treeIdx : arrayIdx;
    if (idx >= 0) {
      float t = static_cast<float>(GetTime());
      bool doubleClick = (t - lastClickTime_ <= DOUBLE_CLICK_TIME)
          && (std::abs(mx - lastClickX_) <= DOUBLE_CLICK_MAX_DIST)
          && (std::abs(my - lastClickY_) <= DOUBLE_CLICK_MAX_DIST);
      if (doubleClick) {
        editIndex_ = idx;
        editBuffer_ = std::to_string(model_.get(static_cast<size_t>(idx)));
        editingNode_ = true;
        lastClickTime_ = 0.f;
      } else {
        lastClickTime_ = t;
        lastClickX_ = mx;
        lastClickY_ = my;
      }
    }
  }
}

void HeapScene::update() {
  float dt = GetFrameTime();
  if (player_.isPlaying() && player_.canStepForward()) {
    float stepTime = 1.f / std::max(0.1f, player_.getSpeed());
    playAccum_ += dt;
    while (playAccum_ >= stepTime && player_.canStepForward()) {
      controller_.applyCurrentStepAndAdvance();
      playAccum_ -= stepTime;
    }
    if (!player_.canStepForward()) playAccum_ = 0.f;
  } else {
    playAccum_ = 0.f;
  }
}

void HeapScene::draw() {
  renderer_->beginFrame();
  renderer_->drawText(isMaxHeap_ ? "Max Heap" : "Min Heap", viz::Vector2{20, 20}, 24, C_LABEL);
  renderer_->drawText("Esc=tilbage", viz::Vector2{20, 48}, 14, C_LABEL);

  drawTreeView();
  drawArrayView();
  drawPanel();
  drawStepHighlight();

  std::string status = statusMessage_;
  if (status.empty()) {
    const heap::HeapStep* step = player_.getCurrentStep();
    if (step && !step->message.empty()) status = step->message;
  }
  if (!status.empty())
    renderer_->drawText("Status: " + status, viz::Vector2{520, 72}, 12, C_LABEL);

  if (editingInsertValue_) {
    renderer_->drawRectangle(PANEL_X, PANEL_Y + 56, 200, 22, C_PANEL_BG);
    renderer_->drawText("Vaerdi: " + (insertValueBuffer_.empty() ? "_" : insertValueBuffer_) + "  Enter=OK  Esc=annuller", viz::Vector2{PANEL_X + 10, PANEL_Y + 60}, 12, C_LABEL);
  }
  if (buildInputFocus_) {
    renderer_->drawRectangle(PANEL_X, PANEL_Y + 56, 200, 22, C_PANEL_BG);
    std::string arr = buildArrayInput_.empty() ? "_" : buildArrayInput_;
    if (arr.length() > 28u) arr = arr.substr(0, 25) + "...";
    renderer_->drawText("Tal med komma: " + arr + "  Enter=indsæt  Esc=annuller", viz::Vector2{PANEL_X + 10, PANEL_Y + 60}, 12, C_LABEL);
  }

  if (editingNode_ && editIndex_ >= 0) {
    renderer_->drawRectangle(300, 300, 220, 60, C_PANEL_BG);
    renderer_->drawText("Ny vaerdi for index " + std::to_string(editIndex_) + ":", viz::Vector2{310, 308}, 14, C_LABEL);
    renderer_->drawText(editBuffer_.empty() ? "_" : editBuffer_, viz::Vector2{310, 328}, 18, C_LABEL);
    renderer_->drawText("Enter=OK  Esc=annuller", viz::Vector2{310, 358}, 12, C_LABEL);
  }

  renderer_->endFrame();
}
