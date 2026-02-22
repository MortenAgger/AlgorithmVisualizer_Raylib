#pragma once

#include "pathfinding/IGraph.h"
#include "datastructures/Graph.h"
#include <memory>

namespace pathfinding {

/**
 * Adapts our Graph (nodes/edges) to IGraph. BFS/DFS treat all edges as weight 1 for "unweighted" intuition.
 */
class GraphAdapter : public IGraph {
public:
  explicit GraphAdapter(const Graph* graph);
  std::vector<int> getNodeIds() const override;
  std::vector<int> getNeighbors(int nodeId) const override;
  int getEdgeWeight(int fromId, int toId) const override;
  bool isBlocked(int nodeId) const override;
  void getNodePosition(int nodeId, float& x, float& y) const override;
  int getHeuristic(int fromId, int toId) const override;

private:
  const Graph* graph_;
};

} // namespace pathfinding
