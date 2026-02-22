#pragma once

#include "mst/Graph.h"
#include "mst/Step.h"
#include <vector>

namespace mst {

class IMstAlgorithm {
public:
  virtual ~IMstAlgorithm() = default;
  virtual std::vector<Step> generateSteps(const Graph& g, int startNodeId) = 0;
  virtual std::string name() const = 0;
};

} // namespace mst
