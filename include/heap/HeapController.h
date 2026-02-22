#pragma once

#include "heap/HeapModel.h"
#include "heap/HeapPolicy.h"
#include "heap/HeapEngine.h"
#include "heap/HeapStepPlayer.h"
#include "heap/HeapCommands.h"
#include <memory>
#include <vector>

namespace heap {

/**
 * Applikationslag: modtager commands, kalder engine, sender step-sekvens til player.
 */
class HeapController {
public:
  HeapController(HeapModel& model, HeapStepPlayer& player);

  void setMaxHeap(bool maxHeap);
  void execute(const InsertCommand& cmd);
  void execute(const DeleteRootCommand& cmd);
  void execute(const UpdateNodeValueCommand& cmd);
  void execute(const HeapifyCommand& cmd);
  void execute(const SetHeapTypeCommand& cmd);
  void execute(const ClearHeapCommand& cmd);
  void execute(const SetArrayCommand& cmd);
  void execute(const BuildFromArrayCommand& cmd);

  HeapModel& getModel() { return model_; }
  const HeapModel& getModel() const { return model_; }
  HeapStepPlayer& getPlayer() { return player_; }
  const HeapStepPlayer& getPlayer() const { return player_; }

  /** Apply the step at current index to model, then step forward. Call before player.stepForward(). */
  void applyCurrentStepAndAdvance();
  /** Restore model to state after applying steps [0, index). Call after player.stepBackward(). */
  void restoreAndReplayTo(size_t index);
  /** Snapshot at start of current command (for Skip Back). */
  void restoreSnapshotAndResetPlayer();

private:
  HeapModel& model_;
  HeapStepPlayer& player_;
  HeapPolicy policy_;
  HeapEngine engine_;
  std::vector<int> snapshot_;
};

} // namespace heap
