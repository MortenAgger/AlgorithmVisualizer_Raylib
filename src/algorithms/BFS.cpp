#include "algorithms/BFSAlgorithm.h"
#include <algorithm>

BFSAlgorithm::BFSAlgorithm(Graph* graph, int startId, int targetId)
  : graph_(graph), startId_(startId), targetId_(targetId) {
  reset();
}

void BFSAlgorithm::reset() {
  finished_ = false;
  while (!queue_.empty()) queue_.pop();
  visited_.clear();
  path_.clear();
  parent_.clear();
  graph_->clearStates();
  if (graph_->getNode(startId_) && !graph_->isBlocked(startId_)) {
    queue_.push(startId_);
    visited_.insert(startId_);
    parent_[startId_] = startId_;
    Node* n = graph_->getNode(startId_);
    if (n) n->state = Node::State::InQueue;
  }
}

void BFSAlgorithm::step() {
  if (finished_ || queue_.empty()) {
    finished_ = true;
    if (targetId_ >= 0 && parent_.count(targetId_)) {
      for (int v = targetId_; ; v = parent_.at(v)) {
        path_.push_back(v);
        if (v == startId_) break;
      }
      std::reverse(path_.begin(), path_.end());
      for (int id : path_) {
        Node* n = graph_->getNode(id);
        if (n) n->state = Node::State::Path;
      }
    }
    return;
  }
  int u = queue_.front();
  queue_.pop();
  Node* un = graph_->getNode(u);
  if (un) un->state = Node::State::Visited;
  if (u == targetId_) {
    finished_ = true;
    for (int v = targetId_; ; v = parent_.at(v)) {
      path_.push_back(v);
      if (v == startId_) break;
    }
    std::reverse(path_.begin(), path_.end());
    for (int id : path_) {
      Node* n = graph_->getNode(id);
      if (n) n->state = Node::State::Path;
    }
    return;
  }
  for (int v : graph_->getNeighborIds(u)) {
    if (graph_->isBlocked(v) || visited_.count(v)) continue;
    visited_.insert(v);
    parent_[v] = u;
    queue_.push(v);
    Node* vn = graph_->getNode(v);
    if (vn) vn->state = Node::State::InQueue;
  }
}
