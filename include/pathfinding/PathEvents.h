#pragma once

#include <vector>
#include <unordered_map>

namespace pathfinding {

enum class NodeViewState { Unvisited, InQueue, Visiting, Visited, Path };

enum class EventType { VisitNode, Enqueue, SetParent, FinalPath };

struct PathEvent {
  EventType type;
  int idA{0};
  int idB{0};
  std::vector<int> path;
};

/**
 * Replayable event log: apply events[0..currentIndex] to get current view state.
 * Supports step, pause, and undo (step back).
 */
class EventPlayer {
public:
  void clear();
  void push(const PathEvent& e);
  void setCurrentIndex(size_t i);
  size_t getCurrentIndex() const { return currentIndex_; }
  size_t getEventCount() const { return events_.size(); }
  bool canStepForward() const { return currentIndex_ < events_.size(); }
  bool canStepBackward() const { return currentIndex_ > 0; }
  void stepForward();
  void stepBackward();
  NodeViewState getNodeState(int nodeId) const;
  const std::vector<int>& getPath() const { return path_; }
  int getNodesVisited() const { return nodesVisited_; }
  int getPathLength() const { return static_cast<int>(path_.size()); }

private:
  void rebuildStateFromEvents(size_t upToIndex);

  std::vector<PathEvent> events_;
  size_t currentIndex_{0};
  std::unordered_map<int, NodeViewState> nodeState_;
  std::vector<int> path_;
  int nodesVisited_{0};
};

} // namespace pathfinding
