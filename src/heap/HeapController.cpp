#include "heap/HeapController.h"

namespace heap {

HeapController::HeapController(HeapModel& model, HeapStepPlayer& player)
  : model_(model), player_(player),
    policy_(HeapPolicy::minHeap()),
    engine_(model_, policy_) {}

void HeapController::setMaxHeap(bool maxHeap) {
  policy_ = maxHeap ? HeapPolicy::maxHeap() : HeapPolicy::minHeap();
  engine_.setPolicy(policy_);
}

void HeapController::execute(const InsertCommand& cmd) {
  snapshot_ = model_.getState();
  auto steps = engine_.insert(cmd.value);
  player_.setSteps(steps);
  model_.loadState(snapshot_);
}

void HeapController::execute(const DeleteRootCommand& cmd) {
  (void)cmd;
  if (model_.empty()) return;
  snapshot_ = model_.getState();
  auto steps = engine_.deleteRoot();
  player_.setSteps(steps);
  model_.loadState(snapshot_);
}

void HeapController::execute(const UpdateNodeValueCommand& cmd) {
  if (cmd.index >= model_.size()) return;
  snapshot_ = model_.getState();
  auto steps = engine_.updateValue(cmd.index, cmd.newValue);
  player_.setSteps(steps);
  model_.loadState(snapshot_);
}

void HeapController::execute(const HeapifyCommand& cmd) {
  (void)cmd;
  snapshot_ = model_.getState();
  auto steps = engine_.heapify();
  player_.setSteps(steps);
  model_.loadState(snapshot_);
}

void HeapController::applyCurrentStepAndAdvance() {
  const HeapStep* s = player_.getCurrentStep();
  if (s) HeapEngine::applyStep(model_, *s);
  player_.stepForward();
}

void HeapController::restoreAndReplayTo(size_t index) {
  model_.loadState(snapshot_);
  for (size_t i = 0; i < index; ++i) {
    const HeapStep* s = player_.getStep(i);
    if (s) HeapEngine::applyStep(model_, *s);
  }
}

void HeapController::restoreSnapshotAndResetPlayer() {
  model_.loadState(snapshot_);
  player_.resetToStart();
}

void HeapController::execute(const SetHeapTypeCommand& cmd) {
  setMaxHeap(cmd.isMaxHeap);
}

void HeapController::execute(const ClearHeapCommand& cmd) {
  (void)cmd;
  model_.clear();
  player_.clear();
  snapshot_.clear();
}

void HeapController::execute(const SetArrayCommand& cmd) {
  model_.clear();
  for (int v : cmd.values) model_.pushBack(v);
  player_.clear();
  snapshot_.clear();
}

void HeapController::execute(const BuildFromArrayCommand& cmd) {
  snapshot_ = model_.getState();
  auto steps = engine_.buildFromArray(cmd.values, cmd.mode);
  player_.setSteps(steps);
  model_.loadState(snapshot_);
}

} // namespace heap
