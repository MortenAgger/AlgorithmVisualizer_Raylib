#include "algorithms/DijkstraAlgorithm.h"
#include <algorithm>

DijkstraAlgorithm::DijkstraAlgorithm(Graph* graph, int startId, int targetId)
  : graph_(graph), startId_(startId), targetId_(targetId) {
  reset();
}

void DijkstraAlgorithm::reset() {
  finished_ = false;
  dist_.clear();
  parent_.clear();
  path_.clear();
  while (!pq_.empty()) pq_.pop();
  graph_->clearStates();
  if (!graph_->getNode(startId_) || graph_->isBlocked(startId_)) return;
  dist_[startId_] = 0;
  parent_[startId_] = startId_;
  pq_.push({0, startId_});
  Node* n = graph_->getNode(startId_);
  if (n) n->state = Node::State::InQueue;
}

void DijkstraAlgorithm::step() {
  if (finished_) return;
  if (pq_.empty()) {
    finished_ = true;
    if (targetId_ >= 0 && parent_.count(targetId_)) {
      for (int v = targetId_; ; v = parent_.at(v)) {
        path_.push_back(v);
        Node* n = graph_->getNode(v);
        if (n) n->state = Node::State::Path;
        if (v == startId_) break;
      }
      std::reverse(path_.begin(), path_.end());
    }
    return;
  }
  int d = pq_.top().first;
  int u = pq_.top().second;
  pq_.pop();
  if (dist_.count(u) && d > dist_[u]) return;
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
  for (int v : graph_->getNeighborIds(u)) {
    if (graph_->isBlocked(v)) continue;
    int w = graph_->edgeWeight(u, v);
    int alt = d + w;
    if (!dist_.count(v) || alt < dist_[v]) {
      dist_[v] = alt;
      parent_[v] = u;
      pq_.push({alt, v});
      Node* vn = graph_->getNode(v);
      if (vn) vn->state = Node::State::InQueue;
    }
  }
}

int DijkstraAlgorithm::getDistance(int nodeId) const {
  auto it = dist_.find(nodeId);
  return it == dist_.end() ? -1 : it->second;
}
