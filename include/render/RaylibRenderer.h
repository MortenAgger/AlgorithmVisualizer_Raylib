#pragma once

#include "IRenderer.h"

class RaylibRenderer : public IRenderer {
public:
  void beginFrame() override;
  void endFrame() override;
  void drawNode(const viz::Vector2& pos, float radius, viz::Color col) override;
  void drawEdge(const viz::Vector2& a, const viz::Vector2& b, viz::Color col) override;
  void drawEdgeThick(const viz::Vector2& a, const viz::Vector2& b, viz::Color col, float thickness) override;
  void drawText(const std::string& str, const viz::Vector2& pos, int fontSize, viz::Color col) override;
  void drawRectangle(float x, float y, float w, float h, viz::Color col) override;
};
