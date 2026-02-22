#include "pathfinding/GraphAdapter.h"
#include <cmath>

namespace pathfinding {

GraphAdapter::GraphAdapter(const Graph* graph) : graph_(graph) {}

std::vector<int> GraphAdapter::getNodeIds() const {
  std::vector<int> ids;
  for (const Node& n : graph_->nodes())
    if (!n.blocked) ids.push_back(n.id);
  return ids;
}

std::vector<int> GraphAdapter::getNeighbors(int nodeId) const {
  return graph_->getNeighborIds(nodeId);
}

int GraphAdapter::getEdgeWeight(int fromId, int toId) const {
  return graph_->edgeWeight(fromId, toId);
}

bool GraphAdapter::isBlocked(int nodeId) const {
  return graph_->isBlocked(nodeId);
}

void GraphAdapter::getNodePosition(int nodeId, float& x, float& y) const {
  const Node* n = graph_->getNode(nodeId);
  if (n) { x = n->x; y = n->y; }
  else { x = 0; y = 0; }
}

int GraphAdapter::getHeuristic(int fromId, int toId) const {
  const Node* a = graph_->getNode(fromId);
  const Node* b = graph_->getNode(toId);
  if (!a || !b) return 0;
  float dx = a->x - b->x, dy = a->y - b->y;
  return static_cast<int>(std::sqrt(dx*dx + dy*dy));
}

} // namespace pathfinding
