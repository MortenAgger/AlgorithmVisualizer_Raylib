#pragma once

class IScene;

/**
 * Application: main loop and scene management.
 * Depends only on IScene (DIP). Owns current scene and runs input/update/draw.
 */
class Application {
public:
  Application();
  ~Application();
  void run();
  void setScene(IScene* scene);
  /** Call to request application exit (e.g. from main menu "Luk program"). */
  void requestQuit() { shouldQuit_ = true; }

private:
  IScene* currentScene_{nullptr};
  bool initialized_{false};
  bool shouldQuit_{false};
};
