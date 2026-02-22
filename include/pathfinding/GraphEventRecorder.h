#pragma once

#include "pathfinding/PathEvents.h"
#include "datastructures/Graph.h"
#include <unordered_set>
#include <vector>

namespace pathfinding {

/**
 * After each algorithm step(), call recordFromGraph() to push delta events to the player.
 * Keeps previous state to only emit new events.
 */
class GraphEventRecorder {
public:
  explicit GraphEventRecorder(EventPlayer& player);
  void recordFromGraph(const Graph* graph, int startId, int targetId);
  void pushFinalPath(const std::vector<int>& path);
  void reset();

private:
  EventPlayer* player_;
  std::unordered_set<int> prevVisited_;
  std::unordered_set<int> prevInQueue_;
  std::vector<int> prevPath_;
};

} // namespace pathfinding
