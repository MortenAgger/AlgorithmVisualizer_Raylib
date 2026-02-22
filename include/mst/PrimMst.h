#pragma once

#include "mst/IMstAlgorithm.h"

namespace mst {

class PrimMst : public IMstAlgorithm {
public:
  std::vector<Step> generateSteps(const Graph& g, int startNodeId) override;
  std::string name() const override { return "Prim"; }
};

} // namespace mst
