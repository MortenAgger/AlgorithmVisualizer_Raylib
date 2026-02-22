#pragma once

#include <vector>
#include <unordered_map>
#include <cstddef>

/**
 * Graph node: id and optional screen position for visualization.
 */
struct Node {
  int id{0};
  float x{0.f}, y{0.f};
  enum class State { Default, Visited, InQueue, Current, Path };
  State state{State::Default};
  bool blocked{false};  // hindring – algoritmen må ikke gå her
};

/**
 * Directed edge with optional weight (for Dijkstra/A*).
 */
struct Edge {
  int fromId{0};
  int toId{0};
  int weight{1};
};

/**
 * Graph: adjacency list, nodes and edges. SRP: structure only, no rendering.
 */
class Graph {
public:
  Graph() = default;
  Node* addNode(int id, float x = 0.f, float y = 0.f);
  Node* getNode(int id);
  const Node* getNode(int id) const;
  void addEdge(int fromId, int toId, int weight = 1);
  /** For tree constraint: returns the node id that has an edge TO nodeId (i.e. parent in directed sense), or -1 if none. */
  int getParentId(int nodeId) const;
  bool hasEdge(int fromId, int toId) const;
  std::vector<int> getNeighborIds(int nodeId) const;
  const Edge* getEdge(int fromId, int toId) const;
  Edge* getEdge(int fromId, int toId);
  int edgeWeight(int fromId, int toId) const;
  void setEdgeWeight(int fromId, int toId, int weight);
  void clearStates();
  void setBlocked(int nodeId, bool blocked);
  bool isBlocked(int nodeId) const;
  void removeNode(int id);
  void removeEdge(int fromId, int toId);
  const std::vector<Node>& nodes() const { return nodes_; }
  const std::vector<Edge>& edges() const { return edges_; }
  size_t nodeCount() const { return nodes_.size(); }
  size_t edgeCount() const { return edges_.size(); }

private:
  std::vector<Node> nodes_;
  std::vector<Edge> edges_;
  std::unordered_map<int, size_t> idToIndex_;
  std::unordered_map<int, std::vector<size_t>> adjacency_; // nodeId -> indices into edges_
};
