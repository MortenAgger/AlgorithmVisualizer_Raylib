#pragma once

#include "heap/HeapModel.h"
#include "heap/HeapPolicy.h"
#include "heap/HeapStep.h"
#include "heap/HeapCommands.h"
#include <vector>

namespace heap {

/**
 * Domæne: Heap-algoritmer. Ændrer modellen og producerer step-sekvens.
 * Ingen raylib, ingen UI – kun heap-logik og steps ud.
 */
class HeapEngine {
public:
  HeapEngine(HeapModel& model, HeapPolicy policy);

  std::vector<HeapStep> insert(int x);
  std::vector<HeapStep> deleteRoot();
  std::vector<HeapStep> updateValue(size_t i, int newValue);
  std::vector<HeapStep> heapify();
  std::vector<HeapStep> buildFromArray(const std::vector<int>& values, BuildMode mode);

  void setPolicy(HeapPolicy policy) { policy_ = std::move(policy); }

  /** Apply a single step to the model (for replay). */
  static void applyStep(HeapModel& model, const HeapStep& step);

private:
  void siftUp(size_t i, std::vector<HeapStep>& out);
  void siftDown(size_t i, std::vector<HeapStep>& out);
  size_t parent(size_t i) const { return (i - 1) / 2; }
  size_t left(size_t i) const { return 2 * i + 1; }
  size_t right(size_t i) const { return 2 * i + 2; }

  HeapModel& model_;
  HeapPolicy policy_;
};

} // namespace heap
