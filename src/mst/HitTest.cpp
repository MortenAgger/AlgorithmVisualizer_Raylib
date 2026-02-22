#include "mst/HitTest.h"
#include <cmath>
#include <algorithm>

namespace mst {

float distanceToSegment(float px, float py, float ax, float ay, float bx, float by) {
  float dx = bx - ax, dy = by - ay;
  float len2 = dx * dx + dy * dy;
  if (len2 < 1e-10f) {
    float qx = px - ax, qy = py - ay;
    return std::sqrt(qx * qx + qy * qy);
  }
  float t = ((px - ax) * dx + (py - ay) * dy) / len2;
  t = std::max(0.f, std::min(1.f, t));
  float qx = ax + t * dx - px, qy = ay + t * dy - py;
  return std::sqrt(qx * qx + qy * qy);
}

int hitTestNode(const Graph& g, float mx, float my, float nodeRadius) {
  int best = -1;
  float bestD2 = nodeRadius * nodeRadius;
  for (const Node& n : g.nodes()) {
    float dx = mx - n.pos.x, dy = my - n.pos.y;
    float d2 = dx * dx + dy * dy;
    if (d2 <= bestD2) {
      bestD2 = d2;
      best = n.id;
    }
  }
  return best;
}

int hitTestEdge(const Graph& g, float mx, float my, float threshold) {
  int bestEdge = -1;
  float bestD = threshold;
  for (const Edge& e : g.edges()) {
    const Node* na = g.getNode(e.a);
    const Node* nb = g.getNode(e.b);
    if (!na || !nb) continue;
    float d = distanceToSegment(mx, my, na->pos.x, na->pos.y, nb->pos.x, nb->pos.y);
    if (d < bestD) {
      bestD = d;
      bestEdge = e.id;
    }
  }
  return bestEdge;
}

} // namespace mst
