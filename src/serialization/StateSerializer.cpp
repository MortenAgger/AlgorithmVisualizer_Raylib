#include "serialization/StateSerializer.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

bool StateSerializer::save(const Graph& graph, const std::string& filename) const {
  json j;
  j["nodes"] = json::array();
  for (const Node& n : graph.nodes()) {
    j["nodes"].push_back({
      {"id", n.id},
      {"x", n.x},
      {"y", n.y}
    });
  }
  j["edges"] = json::array();
  for (const Edge& e : graph.edges()) {
    j["edges"].push_back({
      {"from", e.fromId},
      {"to", e.toId},
      {"weight", e.weight}
    });
  }
  std::ofstream f(filename);
  if (!f) return false;
  f << j.dump(2);
  return true;
}

std::unique_ptr<Graph> StateSerializer::load(const std::string& filename) const {
  std::ifstream f(filename);
  if (!f) return nullptr;
  json j;
  try {
    f >> j;
  } catch (...) {
    return nullptr;
  }
  auto graph = std::make_unique<Graph>();
  if (j.contains("nodes")) {
    for (const auto& n : j["nodes"]) {
      int id = n.value("id", 0);
      float x = n.value("x", 0.f);
      float y = n.value("y", 0.f);
      graph->addNode(id, x, y);
    }
  }
  if (j.contains("edges")) {
    for (const auto& e : j["edges"]) {
      int from = e.value("from", 0);
      int to = e.value("to", 0);
      int weight = e.value("weight", 1);
      graph->addEdge(from, to, weight);
    }
  }
  return graph;
}
