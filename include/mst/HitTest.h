#pragma once

#include "mst/Graph.h"

namespace mst {

int hitTestNode(const Graph& g, float mx, float my, float nodeRadius);
int hitTestEdge(const Graph& g, float mx, float my, float threshold = 6.f);
float distanceToSegment(float px, float py, float ax, float ay, float bx, float by);

} // namespace mst
