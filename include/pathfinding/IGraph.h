#pragma once

#include <vector>
#include <utility>

/**
 * Read-only graph abstraction for algorithms. UI provides input; algorithms are independent of rendering.
 */
class IGraph {
public:
  virtual ~IGraph() = default;
  virtual std::vector<int> getNodeIds() const = 0;
  virtual std::vector<int> getNeighbors(int nodeId) const = 0;
  virtual int getEdgeWeight(int fromId, int toId) const = 0;
  virtual bool isBlocked(int nodeId) const = 0;
  virtual void getNodePosition(int nodeId, float& x, float& y) const = 0;
  virtual int getHeuristic(int fromId, int toId) const { (void)fromId; (void)toId; return 0; }
};
