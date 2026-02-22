#pragma once

#include <string>

/**
 * Algorithm strategy interface (OCP/LSP).
 * All algorithm implementations are interchangeable where IAlgorithm is used.
 */
class IAlgorithm {
public:
  virtual ~IAlgorithm() = default;
  virtual void step() = 0;
  virtual bool isFinished() const = 0;
  virtual void reset() = 0;
  virtual std::string name() const = 0;
};
