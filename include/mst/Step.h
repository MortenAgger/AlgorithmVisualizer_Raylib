#pragma once

#include <vector>
#include <string>

namespace mst {

enum class StepKind {
  ConsiderEdge,
  AcceptEdge,
  RejectEdge,
  SetCandidates,
  SetCurrentNode,
  Done,
  Error
};

struct Step {
  StepKind kind{StepKind::Done};
  int edgeId{0};
  int nodeId{0};
  std::vector<int> edgeIds;
  std::string reason;
  int totalWeight{0};
};

struct ViewState {
  std::vector<int> acceptedEdges;
  std::vector<int> rejectedEdges;
  std::vector<int> candidateEdges;
  int currentEdgeId{0};
  int currentNodeId{0};
  int totalWeight{0};
  std::string explanation;
  void clear() {
    acceptedEdges.clear();
    rejectedEdges.clear();
    candidateEdges.clear();
    currentEdgeId = 0;
    currentNodeId = 0;
    totalWeight = 0;
    explanation.clear();
  }
};

} // namespace mst
