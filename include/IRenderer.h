#pragma once

#include <string>

namespace viz {

// Own math types so algorithms don't depend on raylib.h (avoids redefinition when raylib.h is included)
struct Vector2 { float x, y; };
struct Color { unsigned char r, g, b, a; };

}

/**
 * Abstract renderer interface (DIP).
 * Algorithms and scenes depend on IRenderer, not Raylib.
 */
class IRenderer {
public:
  virtual ~IRenderer() = default;
  virtual void beginFrame() = 0;
  virtual void endFrame() = 0;
  virtual void drawNode(const viz::Vector2& pos, float radius, viz::Color col) = 0;
  virtual void drawEdge(const viz::Vector2& a, const viz::Vector2& b, viz::Color col) = 0;
  /** Draw a line with given thickness (e.g. for highlighting MST edges). Default: thin line. */
  virtual void drawEdgeThick(const viz::Vector2& a, const viz::Vector2& b, viz::Color col, float thickness) { (void)thickness; drawEdge(a, b, col); }
  virtual void drawText(const std::string& str, const viz::Vector2& pos, int fontSize, viz::Color col) = 0;
  virtual void drawRectangle(float x, float y, float w, float h, viz::Color col) = 0;
};
