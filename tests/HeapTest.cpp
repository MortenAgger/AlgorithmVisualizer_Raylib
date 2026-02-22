#include <gtest/gtest.h>
#include "datastructures/Heap.h"

TEST(HeapTest, MinHeapOrder) {
  Heap h(Heap::minHeap());
  h.push(30);
  h.push(10);
  h.push(20);
  EXPECT_EQ(h.top(), 10);
  h.pop();
  EXPECT_EQ(h.top(), 20);
  h.pop();
  EXPECT_EQ(h.top(), 30);
  h.pop();
  EXPECT_TRUE(h.empty());
}

TEST(HeapTest, MaxHeapOrder) {
  Heap h(Heap::maxHeap());
  h.push(10);
  h.push(30);
  h.push(20);
  EXPECT_EQ(h.top(), 30);
  h.pop();
  EXPECT_EQ(h.top(), 20);
  h.pop();
  EXPECT_EQ(h.top(), 10);
  h.pop();
  EXPECT_TRUE(h.empty());
}

TEST(HeapTest, SizeAndClear) {
  Heap h(Heap::minHeap());
  h.push(1);
  h.push(2);
  EXPECT_EQ(h.size(), 2u);
  h.clear();
  EXPECT_TRUE(h.empty());
  EXPECT_EQ(h.size(), 0u);
}
