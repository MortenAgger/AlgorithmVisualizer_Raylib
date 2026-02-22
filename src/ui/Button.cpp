#include "ui/Button.h"

Button::Button(float x, float y, float w, float h, const std::string& label)
  : x_(x), y_(y), w_(w), h_(h), label_(label) {}

void Button::draw(IRenderer* renderer) const {
  viz::Color bg{70, 130, 180, 255};
  viz::Color textCol{255, 255, 255, 255};
  renderer->drawRectangle(x_, y_, w_, h_, bg);
  float tx = x_ + 8;
  float ty = y_ + (h_ - 20) / 2;
  if (ty < y_) ty = y_;
  renderer->drawText(label_, viz::Vector2{tx, ty}, 18, textCol);
}

bool Button::contains(float px, float py) const {
  return px >= x_ && px <= x_ + w_ && py >= y_ && py <= y_ + h_;
}

void Button::onMouseClick(float px, float py) {
  if (contains(px, py) && onClick_) onClick_();
}
