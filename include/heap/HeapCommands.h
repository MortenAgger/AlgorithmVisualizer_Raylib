#pragma once

#include <cstddef>
#include <vector>

namespace heap {

struct InsertCommand { int value{0}; };
struct DeleteRootCommand {};
struct UpdateNodeValueCommand { size_t index{0}; int newValue{0}; };
struct HeapifyCommand {};
struct SetHeapTypeCommand { bool isMaxHeap{false}; };
struct ClearHeapCommand {};
struct SetArrayCommand { std::vector<int> values; };
enum class BuildMode { InsertAll, BottomUp };
struct BuildFromArrayCommand { std::vector<int> values; BuildMode mode{BuildMode::InsertAll}; };

} // namespace heap
