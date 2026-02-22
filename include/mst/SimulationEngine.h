#pragma once

#include "mst/Step.h"
#include <vector>
#include <cstddef>

namespace mst {

class SimulationEngine {
public:
  void setSteps(const std::vector<Step>& steps);
  void resetToStart();
  bool stepForward();
  bool stepBackward();
  size_t getCurrentIndex() const { return currentIndex_; }
  size_t getStepCount() const { return steps_.size(); }
  bool atEnd() const { return currentIndex_ >= steps_.size(); }
  bool canStepForward() const { return currentIndex_ < steps_.size(); }
  bool canStepBackward() const { return currentIndex_ > 0; }
  const ViewState& getViewState() const { return viewState_; }
  const Step* getCurrentStep() const;

private:
  std::vector<Step> steps_;
  std::vector<ViewState> snapshots_;
  size_t currentIndex_{0};
  ViewState viewState_;
};

} // namespace mst
