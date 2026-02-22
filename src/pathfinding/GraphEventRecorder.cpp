#include "pathfinding/GraphEventRecorder.h"
#include <algorithm>

namespace pathfinding {

GraphEventRecorder::GraphEventRecorder(EventPlayer& player) : player_(&player) {}

void GraphEventRecorder::reset() {
  prevVisited_.clear();
  prevInQueue_.clear();
  prevPath_.clear();
}

void GraphEventRecorder::recordFromGraph(const Graph* graph, int startId, int targetId) {
  (void)startId;
  (void)targetId;
  std::unordered_set<int> visited, inQueue;
  std::vector<int> path;
  for (const Node& n : graph->nodes()) {
    if (n.state == Node::State::Visited || n.state == Node::State::Current) visited.insert(n.id);
    else if (n.state == Node::State::InQueue) inQueue.insert(n.id);
    if (n.state == Node::State::Path) path.push_back(n.id);
  }
  std::sort(path.begin(), path.end(), [&](int a, int b) {
    size_t ia = 0, ib = 0;
    for (size_t i = 0; i < path.size(); ++i) { if (path[i] == a) ia = i; if (path[i] == b) ib = i; }
    return ia < ib;
  });
  for (int id : visited)
    if (prevVisited_.count(id) == 0) {
      PathEvent e; e.type = EventType::VisitNode; e.idA = id;
      player_->push(e);
    }
  for (int id : inQueue)
    if (prevInQueue_.count(id) == 0 && prevVisited_.count(id) == 0) {
      PathEvent e; e.type = EventType::Enqueue; e.idA = id;
      player_->push(e);
    }
  prevVisited_ = visited;
  prevInQueue_ = inQueue;
  prevPath_ = path;
}

void GraphEventRecorder::pushFinalPath(const std::vector<int>& path) {
  PathEvent e; e.type = EventType::FinalPath; e.path = path;
  player_->push(e);
}

} // namespace pathfinding
