#pragma once

#include "IRenderer.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace mst {

struct Node {
  int id{0};
  viz::Vector2 pos{0.f, 0.f};
  std::string label;
};

struct Edge {
  int id{0};
  int a{0};
  int b{0};
  int weight{1};
};

class Graph {
public:
  Graph() = default;

  int nextNodeId() { return nextNodeId_++; }
  int nextEdgeId() { return nextEdgeId_++; }

  Node* addNode(int id, float x, float y);
  Node* getNode(int id);
  const Node* getNode(int id) const;
  void removeNode(int id);
  bool hasNode(int id) const;

  bool addEdge(int id, int a, int b, int weight = 1);
  void removeEdge(int id);
  Edge* getEdge(int id);
  const Edge* getEdge(int id) const;
  const Edge* findEdge(int a, int b) const;
  void setEdgeWeight(int id, int w);
  bool hasEdge(int a, int b) const;

  std::vector<int> getNeighborIds(int nodeId) const;
  const std::vector<Node>& nodes() const { return nodes_; }
  const std::vector<Edge>& edges() const { return edges_; }
  size_t nodeCount() const { return nodes_.size(); }
  size_t edgeCount() const { return edges_.size(); }

  void clear();
  bool validateNoSelfLoops() const;
  bool validateNoDuplicateEdges() const;

private:
  std::vector<Node> nodes_;
  std::vector<Edge> edges_;
  std::unordered_map<int, size_t> nodeIdToIndex_;
  std::unordered_map<int, size_t> edgeIdToIndex_;
  std::unordered_map<int, std::vector<size_t>> adjacency_;
  int nextNodeId_{1};
  int nextEdgeId_{1};

  void rebuildAdjacency();
};

} // namespace mst
