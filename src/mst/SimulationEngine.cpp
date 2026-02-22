#include "mst/SimulationEngine.h"

namespace mst {

void SimulationEngine::setSteps(const std::vector<Step>& steps) {
  steps_ = steps;
  snapshots_.clear();
  snapshots_.resize(steps_.size() + 1);
  viewState_.clear();
  for (size_t i = 0; i <= steps_.size(); ++i) {
    ViewState vs;
    for (size_t j = 0; j < i; ++j) {
      const Step& s = steps_[j];
      switch (s.kind) {
        case StepKind::ConsiderEdge:
          vs.currentEdgeId = s.edgeId;
          vs.explanation = s.reason;
          break;
        case StepKind::AcceptEdge:
          vs.acceptedEdges.push_back(s.edgeId);
          vs.currentEdgeId = 0;
          vs.explanation = s.reason;
          break;
        case StepKind::RejectEdge:
          vs.rejectedEdges.push_back(s.edgeId);
          vs.currentEdgeId = 0;
          vs.explanation = s.reason;
          break;
        case StepKind::SetCandidates:
          vs.candidateEdges = s.edgeIds;
          break;
        case StepKind::SetCurrentNode:
          vs.currentNodeId = s.nodeId;
          break;
        case StepKind::Done:
          vs.totalWeight = s.totalWeight;
          vs.explanation = "MST complete. Total weight: " + std::to_string(s.totalWeight);
          break;
        case StepKind::Error:
          vs.explanation = "Error: " + s.reason;
          break;
      }
    }
    snapshots_[i] = vs;
  }
  currentIndex_ = 0;
  viewState_ = snapshots_.empty() ? ViewState{} : snapshots_[0];
}

void SimulationEngine::resetToStart() {
  currentIndex_ = 0;
  viewState_ = snapshots_.empty() ? ViewState{} : snapshots_[0];
}

bool SimulationEngine::stepForward() {
  if (currentIndex_ >= steps_.size()) return false;
  currentIndex_++;
  viewState_ = snapshots_[currentIndex_];
  return true;
}

bool SimulationEngine::stepBackward() {
  if (currentIndex_ == 0) return false;
  currentIndex_--;
  viewState_ = snapshots_[currentIndex_];
  return true;
}

const Step* SimulationEngine::getCurrentStep() const {
  if (currentIndex_ >= steps_.size()) return nullptr;
  return &steps_[currentIndex_];
}

} // namespace mst
