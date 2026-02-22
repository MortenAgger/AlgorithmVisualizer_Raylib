#pragma once

#include "IRenderer.h"
#include <string>
#include <functional>

/**
 * Simple button: bounds, label, click callback. SRP / ISP.
 */
class Button {
public:
  Button(float x, float y, float w, float h, const std::string& label);
  void setCallback(std::function<void()> cb) { onClick_ = std::move(cb); }
  void setLabel(const std::string& label) { label_ = label; }
  void draw(IRenderer* renderer) const;
  bool contains(float px, float py) const;
  void onMouseClick(float px, float py);

  float x() const { return x_; }
  float y() const { return y_; }
  float width() const { return w_; }
  float height() const { return h_; }

private:
  float x_, y_, w_, h_;
  std::string label_;
  std::function<void()> onClick_;
};
