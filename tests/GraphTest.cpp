#include <gtest/gtest.h>
#include "datastructures/Graph.h"

TEST(GraphTest, AddNodeAndGetNode) {
  Graph g;
  Node* a = g.addNode(1, 10.f, 20.f);
  ASSERT_NE(a, nullptr);
  EXPECT_EQ(a->id, 1);
  EXPECT_EQ(a->x, 10.f);
  EXPECT_EQ(a->y, 20.f);
  Node* b = g.getNode(1);
  EXPECT_EQ(b, a);
  EXPECT_EQ(g.nodeCount(), 1u);
}

TEST(GraphTest, AddEdgeAndNeighbors) {
  Graph g;
  g.addNode(1, 0, 0);
  g.addNode(2, 1, 0);
  g.addNode(3, 2, 0);
  g.addEdge(1, 2, 5);
  g.addEdge(1, 3, 3);
  auto neighbors = g.getNeighborIds(1);
  EXPECT_EQ(neighbors.size(), 2u);
  EXPECT_EQ(g.edgeWeight(1, 2), 5);
  EXPECT_EQ(g.edgeWeight(1, 3), 3);
}

TEST(GraphTest, ClearStates) {
  Graph g;
  g.addNode(1, 0, 0);
  g.getNode(1)->state = Node::State::Visited;
  g.clearStates();
  EXPECT_EQ(g.getNode(1)->state, Node::State::Default);
}
