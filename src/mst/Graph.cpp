#include "mst/Graph.h"
#include <algorithm>

namespace mst {

Node* Graph::addNode(int id, float x, float y) {
  if (nodeIdToIndex_.count(id)) {
    Node* n = &nodes_[nodeIdToIndex_[id]];
    n->pos.x = x;
    n->pos.y = y;
    return n;
  }
  nodeIdToIndex_[id] = nodes_.size();
  nodes_.push_back(Node{id, viz::Vector2{x, y}, std::to_string(id)});
  rebuildAdjacency();
  return &nodes_.back();
}

Node* Graph::getNode(int id) {
  auto it = nodeIdToIndex_.find(id);
  if (it == nodeIdToIndex_.end()) return nullptr;
  return &nodes_[it->second];
}

const Node* Graph::getNode(int id) const {
  auto it = nodeIdToIndex_.find(id);
  if (it == nodeIdToIndex_.end()) return nullptr;
  return &nodes_[it->second];
}

void Graph::removeNode(int id) {
  auto it = nodeIdToIndex_.find(id);
  if (it == nodeIdToIndex_.end()) return;
  size_t idx = it->second;
  nodes_.erase(nodes_.begin() + static_cast<std::ptrdiff_t>(idx));
  edges_.erase(
    std::remove_if(edges_.begin(), edges_.end(),
      [id](const Edge& e) { return e.a == id || e.b == id; }),
    edges_.end());
  nodeIdToIndex_.clear();
  for (size_t i = 0; i < nodes_.size(); ++i)
    nodeIdToIndex_[nodes_[i].id] = i;
  edgeIdToIndex_.clear();
  for (size_t i = 0; i < edges_.size(); ++i)
    edgeIdToIndex_[edges_[i].id] = i;
  rebuildAdjacency();
}

bool Graph::hasNode(int id) const {
  return nodeIdToIndex_.count(id) != 0;
}

bool Graph::addEdge(int id, int a, int b, int weight) {
  if (a == b) return false;
  if (!hasNode(a) || !hasNode(b)) return false;
  if (hasEdge(a, b)) return false;
  if (edgeIdToIndex_.count(id)) return false;
  edgeIdToIndex_[id] = edges_.size();
  edges_.push_back(Edge{id, a, b, weight});
  rebuildAdjacency();
  return true;
}

void Graph::removeEdge(int id) {
  auto it = edgeIdToIndex_.find(id);
  if (it == edgeIdToIndex_.end()) return;
  size_t idx = it->second;
  edges_.erase(edges_.begin() + static_cast<std::ptrdiff_t>(idx));
  edgeIdToIndex_.clear();
  for (size_t i = 0; i < edges_.size(); ++i)
    edgeIdToIndex_[edges_[i].id] = i;
  rebuildAdjacency();
}

Edge* Graph::getEdge(int id) {
  auto it = edgeIdToIndex_.find(id);
  if (it == edgeIdToIndex_.end()) return nullptr;
  return &edges_[it->second];
}

const Edge* Graph::getEdge(int id) const {
  auto it = edgeIdToIndex_.find(id);
  if (it == edgeIdToIndex_.end()) return nullptr;
  return &edges_[it->second];
}

const Edge* Graph::findEdge(int a, int b) const {
  for (const Edge& e : edges_) {
    if ((e.a == a && e.b == b) || (e.a == b && e.b == a))
      return &e;
  }
  return nullptr;
}

void Graph::setEdgeWeight(int id, int w) {
  Edge* e = getEdge(id);
  if (e && w > 0) e->weight = w;
}

bool Graph::hasEdge(int a, int b) const {
  return findEdge(a, b) != nullptr;
}

std::vector<int> Graph::getNeighborIds(int nodeId) const {
  std::vector<int> out;
  auto it = adjacency_.find(nodeId);
  if (it == adjacency_.end()) return out;
  for (size_t ei : it->second) {
    const Edge& e = edges_[ei];
    int other = (e.a == nodeId) ? e.b : e.a;
    out.push_back(other);
  }
  return out;
}

void Graph::clear() {
  nodes_.clear();
  edges_.clear();
  nodeIdToIndex_.clear();
  edgeIdToIndex_.clear();
  adjacency_.clear();
  nextNodeId_ = 1;
  nextEdgeId_ = 1;
}

bool Graph::validateNoSelfLoops() const {
  for (const Edge& e : edges_)
    if (e.a == e.b) return false;
  return true;
}

bool Graph::validateNoDuplicateEdges() const {
  for (size_t i = 0; i < edges_.size(); ++i)
    for (size_t j = i + 1; j < edges_.size(); ++j) {
      const Edge& u = edges_[i], v = edges_[j];
      if ((u.a == v.a && u.b == v.b) || (u.a == v.b && u.b == v.a))
        return false;
    }
  return true;
}

void Graph::rebuildAdjacency() {
  adjacency_.clear();
  for (size_t i = 0; i < edges_.size(); ++i) {
    const Edge& e = edges_[i];
    adjacency_[e.a].push_back(i);
    adjacency_[e.b].push_back(i);
  }
}

} // namespace mst
