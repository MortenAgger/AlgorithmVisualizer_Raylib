#include "algorithms/AStarAlgorithm.h"
#include <cmath>
#include <algorithm>

AStarAlgorithm::AStarAlgorithm(Graph* graph, int startId, int targetId, HeuristicFunc heuristic)
  : graph_(graph), startId_(startId), targetId_(targetId), heuristic_(std::move(heuristic)),
    pq_([](const PQEntry& a, const PQEntry& b) { return a.f > b.f; }) {
  reset();
}

int AStarAlgorithm::defaultHeuristic(int nodeId, int targetId) const {
  const Node* a = graph_->getNode(nodeId);
  const Node* b = graph_->getNode(targetId);
  if (!a || !b) return 0;
  float dx = a->x - b->x, dy = a->y - b->y;
  return static_cast<int>(std::sqrt(dx*dx + dy*dy));
}

void AStarAlgorithm::reset() {
  finished_ = false;
  gScore_.clear();
  fScore_.clear();
  parent_.clear();
  path_.clear();
  while (!pq_.empty()) pq_.pop();
  graph_->clearStates();
  if (!graph_->getNode(startId_) || !graph_->getNode(targetId_) || graph_->isBlocked(startId_)) return;
  int h0 = heuristic_ ? heuristic_(startId_, targetId_) : defaultHeuristic(startId_, targetId_);
  gScore_[startId_] = 0;
  fScore_[startId_] = h0;
  parent_[startId_] = startId_;
  pq_.push({h0, startId_});
  Node* n = graph_->getNode(startId_);
  if (n) n->state = Node::State::InQueue;
}

void AStarAlgorithm::step() {
  if (finished_) return;
  if (pq_.empty()) {
    finished_ = true;
    return;
  }
  PQEntry e = pq_.top();
  pq_.pop();
  int u = e.id;
  if (gScore_.count(u) && e.f > fScore_[u]) return;
  if (graph_->isBlocked(u)) return;
  Node* un = graph_->getNode(u);
  if (un) un->state = Node::State::Visited;
  if (u == targetId_) {
    finished_ = true;
    for (int v = targetId_; ; v = parent_.at(v)) {
      path_.push_back(v);
      Node* n = graph_->getNode(v);
      if (n) n->state = Node::State::Path;
      if (v == startId_) break;
    }
    std::reverse(path_.begin(), path_.end());
    return;
  }
  int gU = gScore_[u];
  for (int v : graph_->getNeighborIds(u)) {
    if (graph_->isBlocked(v)) continue;
    int w = graph_->edgeWeight(u, v);
    int gNew = gU + w;
    if (!gScore_.count(v) || gNew < gScore_[v]) {
      parent_[v] = u;
      gScore_[v] = gNew;
      int h = heuristic_ ? heuristic_(v, targetId_) : defaultHeuristic(v, targetId_);
      fScore_[v] = gNew + h;
      pq_.push({fScore_[v], v});
      Node* vn = graph_->getNode(v);
      if (vn) vn->state = Node::State::InQueue;
    }
  }
}

int AStarAlgorithm::getDistance(int nodeId) const {
  auto it = gScore_.find(nodeId);
  return it == gScore_.end() ? -1 : it->second;
}
