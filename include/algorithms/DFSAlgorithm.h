#pragma once

#include "IAlgorithm.h"
#include "datastructures/Graph.h"
#include <stack>
#include <unordered_set>
#include <vector>

class DFSAlgorithm : public IAlgorithm {
public:
  DFSAlgorithm(Graph* graph, int startId, int targetId = -1);
  void step() override;
  bool isFinished() const override { return finished_; }
  void reset() override;
  std::string name() const override { return "DFS"; }
  std::vector<int> getPath() const { return path_; }

private:
  void buildPath();

  Graph* graph_;
  int startId_;
  int targetId_;
  std::stack<int> stack_;
  std::unordered_set<int> visited_;
  std::unordered_map<int, int> parent_;
  std::vector<int> path_;
  bool finished_{false};
};
