#include "pathfinding/PathEvents.h"

namespace pathfinding {

void EventPlayer::clear() {
  events_.clear();
  currentIndex_ = 0;
  nodeState_.clear();
  path_.clear();
  nodesVisited_ = 0;
}

void EventPlayer::push(const PathEvent& e) {
  events_.push_back(e);
}

void EventPlayer::setCurrentIndex(size_t i) {
  if (i <= events_.size()) {
    currentIndex_ = i;
    rebuildStateFromEvents(currentIndex_);
  }
}

void EventPlayer::rebuildStateFromEvents(size_t upToIndex) {
  nodeState_.clear();
  path_.clear();
  nodesVisited_ = 0;
  for (size_t i = 0; i < upToIndex && i < events_.size(); ++i) {
    const PathEvent& e = events_[i];
    switch (e.type) {
      case EventType::VisitNode:
        nodeState_[e.idA] = NodeViewState::Visited;
        nodesVisited_++;
        break;
      case EventType::Enqueue:
        if (nodeState_.count(e.idA) == 0 || nodeState_[e.idA] != NodeViewState::Visited)
          nodeState_[e.idA] = NodeViewState::InQueue;
        break;
      case EventType::SetParent:
        break;
      case EventType::FinalPath:
        path_ = e.path;
        break;
    }
  }
}

void EventPlayer::stepForward() {
  if (currentIndex_ >= events_.size()) return;
  const PathEvent& e = events_[currentIndex_];
  switch (e.type) {
    case EventType::VisitNode:
      nodeState_[e.idA] = NodeViewState::Visited;
      nodesVisited_++;
      break;
    case EventType::Enqueue:
      if (nodeState_.count(e.idA) == 0 || nodeState_[e.idA] != NodeViewState::Visited)
        nodeState_[e.idA] = NodeViewState::InQueue;
      break;
    case EventType::SetParent:
      break;
    case EventType::FinalPath:
      path_ = e.path;
      break;
  }
  currentIndex_++;
}

void EventPlayer::stepBackward() {
  if (currentIndex_ == 0) return;
  currentIndex_--;
  rebuildStateFromEvents(currentIndex_);
}

NodeViewState EventPlayer::getNodeState(int nodeId) const {
  auto it = nodeState_.find(nodeId);
  return it != nodeState_.end() ? it->second : NodeViewState::Unvisited;
}

} // namespace pathfinding
