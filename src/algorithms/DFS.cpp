#include "algorithms/DFSAlgorithm.h"
#include <algorithm>

DFSAlgorithm::DFSAlgorithm(Graph* graph, int startId, int targetId)
  : graph_(graph), startId_(startId), targetId_(targetId) {
  reset();
}

void DFSAlgorithm::reset() {
  finished_ = false;
  while (!stack_.empty()) stack_.pop();
  visited_.clear();
  path_.clear();
  parent_.clear();
  graph_->clearStates();
  if (graph_->getNode(startId_) && !graph_->isBlocked(startId_)) {
    stack_.push(startId_);
    Node* n = graph_->getNode(startId_);
    if (n) n->state = Node::State::Current;
  }
}

void DFSAlgorithm::buildPath() {
  path_.clear();
  if (targetId_ < 0 || !parent_.count(targetId_)) return;
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

void DFSAlgorithm::step() {
  if (finished_) return;
  if (stack_.empty()) {
    finished_ = true;
    buildPath();
    return;
  }
  int u = stack_.top();
  stack_.pop();
  Node* un = graph_->getNode(u);
  if (un) un->state = Node::State::Visited;
  if (u == targetId_) {
    finished_ = true;
    buildPath();
    return;
  }
  for (int v : graph_->getNeighborIds(u)) {
    if (graph_->isBlocked(v) || visited_.count(v)) continue;
    visited_.insert(v);
    parent_[v] = u;
    stack_.push(v);
    Node* vn = graph_->getNode(v);
    if (vn) vn->state = Node::State::InQueue;
  }
  visited_.insert(u);
}
