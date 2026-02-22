#pragma once

#include "mst/IMstAlgorithm.h"

namespace mst {

class KruskalMst : public IMstAlgorithm {
public:
  std::vector<Step> generateSteps(const Graph& g, int startNodeId) override;
  std::string name() const override { return "Kruskal"; }
};

} // namespace mst
