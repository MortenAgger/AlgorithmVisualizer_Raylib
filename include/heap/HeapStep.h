#pragma once

#include <vector>
#include <cstddef>
#include <string>

namespace heap {

enum class StepType { Compare, Swap, SetValue, PopBack, Mark, Message, Done };

enum class NodeMark { Default, Highlight, Comparing, Swapping, Updated };

struct HeapStep {
  StepType type{StepType::Done};
  size_t i{0};
  size_t j{0};
  int value{0};
  NodeMark mark{NodeMark::Default};
  std::string message;
};

} // namespace heap
