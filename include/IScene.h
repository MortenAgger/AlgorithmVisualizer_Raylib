#pragma once

/**
 * Abstract scene interface (SRP: each scene handles one screen).
 * Application depends on IScene only (DIP).
 */
class IScene {
public:
  virtual ~IScene() = default;
  virtual void handleInput() = 0;
  virtual void update() = 0;
  virtual void draw() = 0;
};
