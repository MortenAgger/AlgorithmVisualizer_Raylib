#include "mst/PrimMst.h"
#include <queue>
#include <unordered_set>
#include <vector>

namespace mst {

std::vector<Step> PrimMst::generateSteps(const Graph& g, int startNodeId) {
  std::vector<Step> steps;
  if (g.nodeCount() == 0) {
    steps.push_back(Step{StepKind::Error, 0, 0, {}, "Graph is empty", 0});
    return steps;
  }
  if (!g.getNode(startNodeId)) {
    steps.push_back(Step{StepKind::Error, 0, 0, {}, "Start node not found", 0});
    return steps;
  }

  std::unordered_set<int> inMst;
  struct Entry { int w; int edgeId; };
  auto cmp = [](const Entry& a, const Entry& b) { return a.w > b.w; };
  std::priority_queue<Entry, std::vector<Entry>, decltype(cmp)> pq(cmp);

  inMst.insert(startNodeId);
  Step init;
  init.kind = StepKind::SetCurrentNode;
  init.nodeId = startNodeId;
  steps.push_back(init);

  for (int nid : g.getNeighborIds(startNodeId)) {
    const Edge* e = g.findEdge(startNodeId, nid);
    if (e) pq.push({e->weight, e->id});
  }

  int totalWeight = 0;
  while (!pq.empty()) {
    Entry top = pq.top();
    pq.pop();
    const Edge* edge = g.getEdge(top.edgeId);
    if (!edge) continue;

    std::vector<int> candIds;
    std::vector<Entry> rest;
    rest.push_back(top);
    while (!pq.empty()) { rest.push_back(pq.top()); pq.pop(); }
    for (const Entry& e : rest) candIds.push_back(e.edgeId);
    Step sc;
    sc.kind = StepKind::SetCandidates;
    sc.edgeIds = candIds;
    steps.push_back(sc);

    Step consider;
    consider.kind = StepKind::ConsiderEdge;
    consider.edgeId = top.edgeId;
    consider.reason = "Minimum weight candidate";
    steps.push_back(consider);

    int other = inMst.count(edge->a) ? edge->b : edge->a;
    if (inMst.count(other)) {
      Step rej;
      rej.kind = StepKind::RejectEdge;
      rej.edgeId = top.edgeId;
      rej.reason = "Would form cycle (already in MST)";
      steps.push_back(rej);
      for (size_t i = 1; i < rest.size(); ++i) pq.push(rest[i]);
      continue;
    }

    Step acc;
    acc.kind = StepKind::AcceptEdge;
    acc.edgeId = top.edgeId;
    acc.reason = "Add to MST";
    steps.push_back(acc);
    totalWeight += edge->weight;

    inMst.insert(other);
    Step setNode;
    setNode.kind = StepKind::SetCurrentNode;
    setNode.nodeId = other;
    steps.push_back(setNode);

    for (int nid : g.getNeighborIds(other)) {
      if (inMst.count(nid)) continue;
      const Edge* e = g.findEdge(other, nid);
      if (e) pq.push({e->weight, e->id});
    }

    for (size_t i = 1; i < rest.size(); ++i) pq.push(rest[i]);
  }

  if (inMst.size() < g.nodeCount()) {
    Step err;
    err.kind = StepKind::Error;
    err.reason = "Graph is disconnected";
    steps.push_back(err);
    return steps;
  }
  Step done;
  done.kind = StepKind::Done;
  done.totalWeight = totalWeight;
  steps.push_back(done);
  return steps;
}

} // namespace mst
