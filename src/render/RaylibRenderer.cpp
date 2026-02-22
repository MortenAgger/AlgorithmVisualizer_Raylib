#include "render/RaylibRenderer.h"
#include "raylib.h"
#include <cmath>

namespace {
::Vector2 toRaylib(const viz::Vector2& v) { return {v.x, v.y}; }
::Color toRaylib(const viz::Color& c) { return {c.r, c.g, c.b, c.a}; }
}

void RaylibRenderer::beginFrame() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
}

void RaylibRenderer::endFrame() {
  EndDrawing();
}

void RaylibRenderer::drawNode(const viz::Vector2& pos, float radius, viz::Color col) {
  DrawCircleV(toRaylib(pos), radius, toRaylib(col));
}

void RaylibRenderer::drawEdge(const viz::Vector2& a, const viz::Vector2& b, viz::Color col) {
  DrawLineV(toRaylib(a), toRaylib(b), toRaylib(col));
}

void RaylibRenderer::drawEdgeThick(const viz::Vector2& a, const viz::Vector2& b, viz::Color col, float thickness) {
  DrawLineEx(toRaylib(a), toRaylib(b), thickness, toRaylib(col));
}

void RaylibRenderer::drawText(const std::string& str, const viz::Vector2& pos, int fontSize, viz::Color col) {
  DrawText(str.c_str(), static_cast<int>(std::round(pos.x)), static_cast<int>(std::round(pos.y)), fontSize, toRaylib(col));
}

void RaylibRenderer::drawRectangle(float x, float y, float w, float h, viz::Color col) {
  DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h), toRaylib(col));
}
