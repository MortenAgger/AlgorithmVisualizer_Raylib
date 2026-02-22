#pragma once

#include "datastructures/Graph.h"
#include <string>
#include <memory>

/**
 * Saves/loads graph (and optionally algorithm state) to/from JSON.
 */
class StateSerializer {
public:
  bool save(const Graph& graph, const std::string& filename) const;
  std::unique_ptr<Graph> load(const std::string& filename) const;
};
