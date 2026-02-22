#pragma once

#include "IAlgorithm.h"
#include "datastructures/Graph.h"
#include <vector>
#include <unordered_map>
#include <queue>
#include <utility>

class DijkstraAlgorithm : public IAlgorithm {
public:
  DijkstraAlgorithm(Graph* graph, int startId, int targetId = -1);
  void step() override;
  bool isFinished() const override { return finished_; }
  void reset() override;
  std::string name() const override { return "Dijkstra"; }
  int getDistance(int nodeId) const;
  std::vector<int> getPath() const { return path_; }

private:
  Graph* graph_;
  int startId_;
  int targetId_;
  std::unordered_map<int, int> dist_;
  std::unordered_map<int, int> parent_;
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq_;
  std::vector<int> path_;
  bool finished_{false};
};
