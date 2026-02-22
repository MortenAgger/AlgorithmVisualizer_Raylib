#pragma once

#include <vector>
#include <functional>

/**
 * Min-heap eller max-heap via comparator. SRP: kun heap-struktur.
 */
class Heap {
public:
  using Comparator = std::function<bool(int a, int b)>;
  static Comparator minHeap() { return [](int a, int b) { return a > b; }; }
  static Comparator maxHeap() { return [](int a, int b) { return a < b; }; }

  explicit Heap(Comparator cmp = minHeap());
  void push(int value);
  int top() const;
  void pop();
  bool empty() const { return data_.empty(); }
  size_t size() const { return data_.size(); }
  void clear();
  const std::vector<int>& data() const { return data_; }

private:
  void heapifyUp(size_t i);
  void heapifyDown(size_t i);
  std::vector<int> data_;
  Comparator cmp_;
};
