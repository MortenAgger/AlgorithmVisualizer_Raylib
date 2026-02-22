#pragma once

#include "IAlgorithm.h"
#include "datastructures/Graph.h"
#include <vector>
#include <unordered_map>
#include <queue>
#include <functional>

class AStarAlgorithm : public IAlgorithm {
public:
  using HeuristicFunc = std::function<int(int nodeId, int targetId)>;
  AStarAlgorithm(Graph* graph, int startId, int targetId, HeuristicFunc heuristic = nullptr);
  void step() override;
  bool isFinished() const override { return finished_; }
  void reset() override;
  std::string name() const override { return "A*"; }
  int getDistance(int nodeId) const;
  std::vector<int> getPath() const { return path_; }

private:
  int defaultHeuristic(int nodeId, int targetId) const;

  Graph* graph_;
  int startId_;
  int targetId_;
  HeuristicFunc heuristic_;
  std::unordered_map<int, int> gScore_;
  std::unordered_map<int, int> fScore_;
  std::unordered_map<int, int> parent_;
  struct PQEntry { int f, id; };
  std::priority_queue<PQEntry, std::vector<PQEntry>, std::function<bool(const PQEntry&, const PQEntry&)>> pq_;
  std::vector<int> path_;
  bool finished_{false};
};
