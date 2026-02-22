#include "pathfinding/GridGraph.h"
#include <cmath>

namespace pathfinding {

GridGraph::GridGraph(int cols, int rows) : cols_(cols), rows_(rows) {
  walls_.resize(static_cast<size_t>(cols * rows), false);
  terrainCost_.resize(static_cast<size_t>(cols * rows), 1);
}

std::vector<int> GridGraph::getNodeIds() const {
  std::vector<int> ids;
  for (int r = 0; r < rows_; ++r)
    for (int c = 0; c < cols_; ++c)
      if (!walls_[static_cast<size_t>(r * cols_ + c)])
        ids.push_back(r * cols_ + c);
  return ids;
}

std::vector<int> GridGraph::getNeighbors(int nodeId) const {
  std::vector<int> out;
  int r = nodeToRow(nodeId), c = nodeToCol(nodeId);
  const int dr[] = {-1, 1, 0, 0};
  const int dc[] = {0, 0, -1, 1};
  for (int d = 0; d < 4; ++d) {
    int nr = r + dr[d], nc = c + dc[d];
    if (nr >= 0 && nr < rows_ && nc >= 0 && nc < cols_) {
      int nid = cellToId(nr, nc);
      if (!isWall(nr, nc)) out.push_back(nid);
    }
  }
  return out;
}

int GridGraph::getEdgeWeight(int fromId, int toId) const {
  (void)fromId;
  size_t idx = static_cast<size_t>(toId);
  return idx < terrainCost_.size() ? terrainCost_[idx] : 1;
}

bool GridGraph::isBlocked(int nodeId) const {
  return isWall(nodeToRow(nodeId), nodeToCol(nodeId));
}

void GridGraph::getNodePosition(int nodeId, float& x, float& y) const {
  int r = nodeToRow(nodeId), c = nodeToCol(nodeId);
  x = offsetX_ + (c + 0.5f) * cellW_;
  y = offsetY_ + (r + 0.5f) * cellH_;
}

void GridGraph::setWall(int r, int c, bool wall) {
  if (r >= 0 && r < rows_ && c >= 0 && c < cols_)
    walls_[static_cast<size_t>(r * cols_ + c)] = wall;
}

bool GridGraph::isWall(int r, int c) const {
  if (r < 0 || r >= rows_ || c < 0 || c >= cols_) return true;
  return walls_[static_cast<size_t>(r * cols_ + c)];
}

void GridGraph::setTerrainCost(int r, int c, int cost) {
  if (r >= 0 && r < rows_ && c >= 0 && c < cols_)
    terrainCost_[static_cast<size_t>(r * cols_ + c)] = cost;
}

int GridGraph::getTerrainCost(int r, int c) const {
  if (r < 0 || r >= rows_ || c < 0 || c >= cols_) return 1;
  return terrainCost_[static_cast<size_t>(r * cols_ + c)];
}

int GridGraph::nodeToRow(int id) const { return id / cols_; }
int GridGraph::nodeToCol(int id) const { return id % cols_; }

} // namespace pathfinding
