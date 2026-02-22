#include "datastructures/Heap.h"

Heap::Heap(Comparator cmp) : cmp_(std::move(cmp)) {}

void Heap::push(int value) {
  data_.push_back(value);
  heapifyUp(data_.size() - 1);
}

int Heap::top() const {
  return data_.front();
}

void Heap::pop() {
  if (data_.empty()) return;
  data_[0] = data_.back();
  data_.pop_back();
  if (!data_.empty())
    heapifyDown(0);
}

void Heap::clear() {
  data_.clear();
}

void Heap::heapifyUp(size_t i) {
  while (i > 0) {
    size_t parent = (i - 1) / 2;
    if (!cmp_(data_[parent], data_[i])) break;
    std::swap(data_[parent], data_[i]);
    i = parent;
  }
}

void Heap::heapifyDown(size_t i) {
  size_t n = data_.size();
  while (true) {
    size_t best = i;
    size_t left = 2 * i + 1;
    size_t right = 2 * i + 2;
    if (left < n && cmp_(data_[best], data_[left])) best = left;
    if (right < n && cmp_(data_[best], data_[right])) best = right;
    if (best == i) break;
    std::swap(data_[i], data_[best]);
    i = best;
  }
}
