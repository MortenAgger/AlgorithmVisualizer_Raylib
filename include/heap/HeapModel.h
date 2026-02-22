#pragma once

#include <vector>
#include <cstddef>

namespace heap {

/**
 * Domæne: Heap er altid et array. Ingen raylib, ingen UI.
 * Kun data – ikke ansvarlig for algoritmer eller animation.
 */
class HeapModel {
public:
  size_t size() const { return values_.size(); }
  bool empty() const { return values_.empty(); }
  int get(size_t i) const { return values_[i]; }
  void set(size_t i, int v);
  void swap(size_t i, size_t j);
  void pushBack(int v) { values_.push_back(v); }
  void popBack() { if (!values_.empty()) values_.pop_back(); }
  const std::vector<int>& values() const { return values_; }
  void clear() { values_.clear(); }
  std::vector<int> getState() const { return values_; }
  void loadState(const std::vector<int>& state) { values_ = state; }

private:
  std::vector<int> values_;
};

} // namespace heap
