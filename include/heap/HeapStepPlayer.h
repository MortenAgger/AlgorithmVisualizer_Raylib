#pragma once

#include "heap/HeapStep.h"
#include <vector>
#include <cstddef>

namespace heap {

/**
 * Afspiller step-sekvens: play/pause/step, current index.
 * Kender ikke heap-regler – kun steps og afspilning.
 */
class HeapStepPlayer {
public:
  void clear();
  void appendSteps(const std::vector<HeapStep>& steps);
  void setSteps(const std::vector<HeapStep>& steps);
  size_t getCurrentIndex() const { return currentIndex_; }
  size_t getStepCount() const { return steps_.size(); }
  bool atEnd() const { return currentIndex_ >= steps_.size(); }
  bool canStepForward() const { return currentIndex_ < steps_.size(); }
  bool canStepBackward() const { return currentIndex_ > 0; }
  void stepForward();
  void stepBackward();
  void resetToStart() { currentIndex_ = 0; }
  const HeapStep* getCurrentStep() const;
  const HeapStep* getStep(size_t i) const;
  void setSpeed(float stepsPerSecond) { stepsPerSecond_ = stepsPerSecond; }
  float getSpeed() const { return stepsPerSecond_; }
  void setPlaying(bool p) { playing_ = p; }
  bool isPlaying() const { return playing_; }
  void tick(float deltaSeconds);

private:
  std::vector<HeapStep> steps_;
  size_t currentIndex_{0};
  float stepsPerSecond_{2.f};
  bool playing_{false};
  float accumulator_{0.f};
};

} // namespace heap
