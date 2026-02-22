#pragma once

#include "pathfinding/IGraph.h"
#include <vector>
#include <unordered_set>

namespace pathfinding {

/**
 * IGraph for 2D grid: implicit 4-neighborhood, walls, optional terrain cost.
 */
class GridGraph : public IGraph {
public:
  GridGraph(int cols, int rows);
  std::vector<int> getNodeIds() const override;
  std::vector<int> getNeighbors(int nodeId) const override;
  int getEdgeWeight(int fromId, int toId) const override;
  bool isBlocked(int nodeId) const override;
  void getNodePosition(int nodeId, float& x, float& y) const override;

  void setWall(int r, int c, bool wall);
  bool isWall(int r, int c) const;
  void setTerrainCost(int r, int c, int cost);
  int getTerrainCost(int r, int c) const;
  int nodeToRow(int id) const;
  int nodeToCol(int id) const;
  int cellToId(int r, int c) const { return r * cols_ + c; }
  int getCols() const { return cols_; }
  int getRows() const { return rows_; }
  void setCellSize(float w, float h) { cellW_ = w; cellH_ = h; }
  void setOffset(float x, float y) { offsetX_ = x; offsetY_ = y; }

private:
  int cols_, rows_;
  float cellW_{1.f}, cellH_{1.f};
  float offsetX_{0.f}, offsetY_{0.f};
  std::vector<bool> walls_;
  std::vector<int> terrainCost_;
};

} // namespace pathfinding
