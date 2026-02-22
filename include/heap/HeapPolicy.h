#pragma once

#include <functional>

namespace heap {

/**
 * Strategi: afgør om forælder "dominerer" barn.
 * Min-heap: parent ≤ child  =>  dominator(a,b) = (a <= b)
 * Max-heap: parent ≥ child  =>  dominator(a,b) = (a >= b)
 * Samme heap-kode kan arbejde for begge (OCP).
 */
class HeapPolicy {
public:
  using Comparator = std::function<bool(int parent, int child)>;
  static HeapPolicy minHeap() { return HeapPolicy([](int a, int b) { return a <= b; }); }
  static HeapPolicy maxHeap() { return HeapPolicy([](int a, int b) { return a >= b; }); }

  explicit HeapPolicy(Comparator cmp) : cmp_(std::move(cmp)) {}
  bool dominates(int parentValue, int childValue) const { return cmp_(parentValue, childValue); }
  bool shouldSiftUp(int childValue, int parentValue) const { return !dominates(parentValue, childValue); }
  bool shouldSwapWithChild(int parentValue, int childValue) const { return !dominates(parentValue, childValue); }
  /** 0 = left child, 1 = right child – den der skal byttes med forælder ved sift-down. */
  int betterChildIndex(int leftValue, int rightValue) const {
    return cmp_(leftValue, rightValue) ? 0 : 1;
  }

private:
  Comparator cmp_;
};

} // namespace heap
