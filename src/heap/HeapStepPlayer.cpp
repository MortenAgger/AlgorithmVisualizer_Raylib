#include "heap/HeapStepPlayer.h"

namespace heap {

void HeapStepPlayer::clear() {
  steps_.clear();
  currentIndex_ = 0;
  accumulator_ = 0.f;
}

void HeapStepPlayer::appendSteps(const std::vector<HeapStep>& steps) {
  for (const auto& s : steps) steps_.push_back(s);
}

void HeapStepPlayer::setSteps(const std::vector<HeapStep>& steps) {
  steps_ = steps;
  currentIndex_ = 0;
}

void HeapStepPlayer::stepForward() {
  if (currentIndex_ < steps_.size()) currentIndex_++;
}

void HeapStepPlayer::stepBackward() {
  if (currentIndex_ > 0) currentIndex_--;
}

const HeapStep* HeapStepPlayer::getCurrentStep() const {
  if (currentIndex_ >= steps_.size()) return nullptr;
  return &steps_[currentIndex_];
}

const HeapStep* HeapStepPlayer::getStep(size_t i) const {
  if (i >= steps_.size()) return nullptr;
  return &steps_[i];
}

void HeapStepPlayer::tick(float deltaSeconds) {
  if (!playing_ || stepsPerSecond_ <= 0) return;
  accumulator_ += deltaSeconds;
  float stepTime = 1.f / stepsPerSecond_;
  while (accumulator_ >= stepTime && canStepForward()) {
    stepForward();
    accumulator_ -= stepTime;
  }
  if (accumulator_ >= stepTime) accumulator_ = 0.f;
}

} // namespace heap
