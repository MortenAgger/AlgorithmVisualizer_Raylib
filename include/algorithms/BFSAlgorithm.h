#pragma once

#include "IAlgorithm.h"
#include "datastructures/Graph.h"
#include <queue>
#include <unordered_set>
#include <vector>

class BFSAlgorithm : public IAlgorithm {
public:
  BFSAlgorithm(Graph* graph, int startId, int targetId = -1);
  void step() override;
  bool isFinished() const override { return finished_; }
  void reset() override;
  std::string name() const override { return "BFS"; }
  std::vector<int> getPath() const { return path_; }

private:
  Graph* graph_;
  int startId_;
  int targetId_;
  std::queue<int> queue_;
  std::unordered_set<int> visited_;
  std::vector<int> path_;
  std::unordered_map<int, int> parent_;
  bool finished_{false};
};
