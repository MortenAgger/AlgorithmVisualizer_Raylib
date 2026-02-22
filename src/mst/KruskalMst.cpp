#include "mst/KruskalMst.h"
#include "mst/UnionFind.h"
#include <algorithm>
#include <vector>

namespace mst {

std::vector<Step> KruskalMst::generateSteps(const Graph& g, int startNodeId) {
  std::vector<Step> steps;
  (void)startNodeId;
  if (g.nodeCount() == 0) {
    steps.push_back(Step{StepKind::Error, 0, 0, {}, "Graph is empty", 0});
    return steps;
  }

  std::vector<int> nodeIds;
  for (const Node& n : g.nodes()) nodeIds.push_back(n.id);
  UnionFind uf;
  uf.reset(nodeIds);

  std::vector<int> edgeIds;
  for (const Edge& e : g.edges()) edgeIds.push_back(e.id);
  std::sort(edgeIds.begin(), edgeIds.end(), [&g](int a, int b) {
    const Edge* ea = g.getEdge(a);
    const Edge* eb = g.getEdge(b);
    if (!ea || !eb) return a < b;
    return ea->weight < eb->weight;
  });

  int totalWeight = 0;
  for (int eid : edgeIds) {
    const Edge* e = g.getEdge(eid);
    if (!e) continue;

    Step consider;
    consider.kind = StepKind::ConsiderEdge;
    consider.edgeId = eid;
    consider.reason = "Next edge in sorted order (weight " + std::to_string(e->weight) + ")";
    steps.push_back(consider);

    if (uf.connected(e->a, e->b)) {
      Step rej;
      rej.kind = StepKind::RejectEdge;
      rej.edgeId = eid;
      rej.reason = "Would form cycle";
      steps.push_back(rej);
      continue;
    }

    uf.unite(e->a, e->b);
    Step acc;
    acc.kind = StepKind::AcceptEdge;
    acc.edgeId = eid;
    acc.reason = "Add to MST";
    steps.push_back(acc);
    totalWeight += e->weight;
  }

  Step done;
  done.kind = StepKind::Done;
  done.totalWeight = totalWeight;
  steps.push_back(done);
  return steps;
}

} // namespace mst
