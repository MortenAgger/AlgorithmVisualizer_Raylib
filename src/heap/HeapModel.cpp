#include "heap/HeapModel.h"
#include <algorithm>

namespace heap {

void HeapModel::set(size_t i, int v) {
  if (i < values_.size()) values_[i] = v;
}

void HeapModel::swap(size_t i, size_t j) {
  if (i < values_.size() && j < values_.size())
    std::swap(values_[i], values_[j]);
}

} // namespace heap
