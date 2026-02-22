#include "datastructures/Graph.h"
#include <algorithm>

Node* Graph::addNode(int id, float x, float y) {
  if (idToIndex_.count(id))
    return &nodes_[idToIndex_[id]];
  idToIndex_[id] = nodes_.size();
  nodes_.push_back(Node{id, x, y, Node::State::Default});
  return &nodes_.back();
}

Node* Graph::getNode(int id) {
  auto it = idToIndex_.find(id);
  if (it == idToIndex_.end()) return nullptr;
  return &nodes_[it->second];
}

const Node* Graph::getNode(int id) const {
  auto it = idToIndex_.find(id);
  if (it == idToIndex_.end()) return nullptr;
  return &nodes_[it->second];
}

void Graph::addEdge(int fromId, int toId, int weight) {
  if (fromId == toId) return;
  if (hasEdge(fromId, toId)) return;
  addNode(fromId);
  addNode(toId);
  edges_.push_back(Edge{fromId, toId, weight});
  size_t ei = edges_.size() - 1;
  adjacency_[fromId].push_back(ei);
  adjacency_[toId].push_back(ei);
}

int Graph::getParentId(int nodeId) const {
  for (const Edge& e : edges_) {
    if (e.toId == nodeId) return e.fromId;
  }
  return -1;
}

bool Graph::hasEdge(int fromId, int toId) const {
  auto it = adjacency_.find(fromId);
  if (it == adjacency_.end()) return false;
  for (size_t ei : it->second) {
    const Edge& e = edges_[ei];
    if ((e.fromId == fromId && e.toId == toId) || (e.fromId == toId && e.toId == fromId))
      return true;
  }
  return false;
}

std::vector<int> Graph::getNeighborIds(int nodeId) const {
  std::vector<int> out;
  auto it = adjacency_.find(nodeId);
  if (it == adjacency_.end()) return out;
  for (size_t ei : it->second) {
    const Edge& e = edges_[ei];
    int other = (e.fromId == nodeId) ? e.toId : e.fromId;
    out.push_back(other);
  }
  return out;
}

const Edge* Graph::getEdge(int fromId, int toId) const {
  auto it = adjacency_.find(fromId);
  if (it == adjacency_.end()) return nullptr;
  for (size_t ei : it->second) {
    const Edge& e = edges_[ei];
    if ((e.fromId == fromId && e.toId == toId) || (e.fromId == toId && e.toId == fromId))
      return &edges_[ei];
  }
  return nullptr;
}

Edge* Graph::getEdge(int fromId, int toId) {
  auto it = adjacency_.find(fromId);
  if (it == adjacency_.end()) return nullptr;
  for (size_t ei : it->second) {
    Edge& e = edges_[ei];
    if ((e.fromId == fromId && e.toId == toId) || (e.fromId == toId && e.toId == fromId))
      return &e;
  }
  return nullptr;
}

void Graph::setEdgeWeight(int fromId, int toId, int weight) {
  Edge* e = getEdge(fromId, toId);
  if (e && weight > 0) e->weight = weight;
}

int Graph::edgeWeight(int fromId, int toId) const {
  const Edge* e = getEdge(fromId, toId);
  return e ? e->weight : 0;
}

void Graph::clearStates() {
  for (Node& n : nodes_)
    n.state = Node::State::Default;
  // blocked tilføjes ikke – hindringer er del af udfordringen
}

void Graph::setBlocked(int nodeId, bool blocked) {
  Node* n = getNode(nodeId);
  if (n) n->blocked = blocked;
}

bool Graph::isBlocked(int nodeId) const {
  const Node* n = getNode(nodeId);
  return n && n->blocked;
}

void Graph::removeNode(int id) {
  auto it = idToIndex_.find(id);
  if (it == idToIndex_.end()) return;
  size_t idx = it->second;
  idToIndex_.erase(it);
  adjacency_.erase(id);
  for (auto& p : adjacency_)
    p.second.clear();
  edges_.erase(
    std::remove_if(edges_.begin(), edges_.end(),
      [id](const Edge& e) { return e.fromId == id || e.toId == id; }),
    edges_.end());
  nodes_.erase(nodes_.begin() + static_cast<std::ptrdiff_t>(idx));
  idToIndex_.clear();
  for (size_t i = 0; i < nodes_.size(); ++i)
    idToIndex_[nodes_[i].id] = i;
  for (size_t i = 0; i < edges_.size(); ++i)
    adjacency_[edges_[i].fromId].push_back(i);
}

void Graph::removeEdge(int fromId, int toId) {
  for (size_t i = 0; i < edges_.size(); ++i) {
    const Edge& e = edges_[i];
    if ((e.fromId == fromId && e.toId == toId) || (e.fromId == toId && e.toId == fromId)) {
      edges_.erase(edges_.begin() + static_cast<std::ptrdiff_t>(i));
      adjacency_.clear();
      for (size_t j = 0; j < edges_.size(); ++j) {
        adjacency_[edges_[j].fromId].push_back(j);
        adjacency_[edges_[j].toId].push_back(j);
      }
      return;
    }
  }
}
