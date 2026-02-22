#include <gtest/gtest.h>
#include "datastructures/Graph.h"
#include "algorithms/BFSAlgorithm.h"

TEST(BFSTest, FindsPathInSimpleGraph) {
  Graph g;
  g.addNode(1, 0, 0);
  g.addNode(2, 1, 0);
  g.addNode(3, 2, 0);
  g.addEdge(1, 2);
  g.addEdge(2, 3);
  BFSAlgorithm bfs(&g, 1, 3);
  while (!bfs.isFinished()) bfs.step();
  auto path = bfs.getPath();
  ASSERT_EQ(path.size(), 3u);
  EXPECT_EQ(path[0], 1);
  EXPECT_EQ(path[1], 2);
  EXPECT_EQ(path[2], 3);
}

TEST(BFSTest, NoPathWhenDisconnected) {
  Graph g;
  g.addNode(1, 0, 0);
  g.addNode(2, 1, 0);
  g.addEdge(1, 1); // self-loop only
  BFSAlgorithm bfs(&g, 1, 2);
  while (!bfs.isFinished()) bfs.step();
  auto path = bfs.getPath();
  EXPECT_TRUE(path.empty());
}

TEST(BFSTest, ResetRestarts) {
  Graph g;
  g.addEdge(1, 2);
  g.addEdge(2, 3);
  BFSAlgorithm bfs(&g, 1, 3);
  bfs.step();
  bfs.reset();
  auto path = bfs.getPath();
  EXPECT_TRUE(path.empty());
  while (!bfs.isFinished()) bfs.step();
  path = bfs.getPath();
  ASSERT_EQ(path.size(), 3u);
}
