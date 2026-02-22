#include "heap/HeapEngine.h"
#include <algorithm>
#include <string>

namespace heap {

HeapEngine::HeapEngine(HeapModel& model, HeapPolicy policy)
  : model_(model), policy_(std::move(policy)) {}

void HeapEngine::siftUp(size_t i, std::vector<HeapStep>& out) {
  while (i > 0) {
    size_t p = parent(i);
    int pv = model_.get(p);
    int cv = model_.get(i);
    HeapStep cmp; cmp.type = StepType::Compare; cmp.i = p; cmp.j = i;
    out.push_back(cmp);
    if (!policy_.shouldSiftUp(cv, pv)) break;
    HeapStep sw; sw.type = StepType::Swap; sw.i = p; sw.j = i;
    out.push_back(sw);
    model_.swap(p, i);
    i = p;
  }
}

void HeapEngine::siftDown(size_t i, std::vector<HeapStep>& out) {
  size_t n = model_.size();
  while (true) {
    size_t l = left(i), r = right(i);
    size_t best = i;
    if (l < n) {
      int bestVal = model_.get(best);
      int leftVal = model_.get(l);
      if (r >= n) {
        HeapStep cmp; cmp.type = StepType::Compare; cmp.i = i; cmp.j = l;
        out.push_back(cmp);
        if (policy_.shouldSwapWithChild(bestVal, leftVal)) best = l;
      } else {
        int rightVal = model_.get(r);
        HeapStep cmp; cmp.type = StepType::Compare; cmp.i = l; cmp.j = r;
        out.push_back(cmp);
        int which = policy_.betterChildIndex(leftVal, rightVal);
        size_t childIdx = (which == 0) ? l : r;
        int childVal = (which == 0) ? leftVal : rightVal;
        HeapStep cmp2; cmp2.type = StepType::Compare; cmp2.i = i; cmp2.j = childIdx;
        out.push_back(cmp2);
        if (policy_.shouldSwapWithChild(model_.get(i), childVal)) best = childIdx;
      }
    }
    if (best == i) break;
    HeapStep sw; sw.type = StepType::Swap; sw.i = i; sw.j = best;
    out.push_back(sw);
    model_.swap(i, best);
    i = best;
  }
}

std::vector<HeapStep> HeapEngine::insert(int x) {
  std::vector<HeapStep> out;
  model_.pushBack(x);
  size_t i = model_.size() - 1;
  HeapStep set; set.type = StepType::SetValue; set.i = i; set.value = x;
  set.message = "Insert " + std::to_string(x) + ": placed at index " + std::to_string(i);
  out.push_back(set);
  siftUp(i, out);
  HeapStep done; done.type = StepType::Done; done.message = "Done";
  out.push_back(done);
  return out;
}

std::vector<HeapStep> HeapEngine::deleteRoot() {
  std::vector<HeapStep> out;
  if (model_.empty()) return out;
  if (model_.size() == 1) {
    model_.popBack();
    HeapStep pop; pop.type = StepType::PopBack; out.push_back(pop);
    HeapStep done; done.type = StepType::Done; out.push_back(done);
    return out;
  }
  size_t last = model_.size() - 1;
  HeapStep sw; sw.type = StepType::Swap; sw.i = 0; sw.j = last;
  sw.message = "DeleteRoot: byt rod med sidste, sift ned";
  out.push_back(sw);
  model_.swap(0, last);
  model_.popBack();
  HeapStep pop; pop.type = StepType::PopBack; out.push_back(pop);
  siftDown(0, out);
  HeapStep done; done.type = StepType::Done; done.message = "Done";
  out.push_back(done);
  return out;
}

std::vector<HeapStep> HeapEngine::updateValue(size_t i, int newValue) {
  std::vector<HeapStep> out;
  if (i >= model_.size()) return out;
  int oldVal = model_.get(i);
  model_.set(i, newValue);
  HeapStep set; set.type = StepType::SetValue; set.i = i; set.value = newValue;
  out.push_back(set);
  (void)oldVal;
  if (i > 0 && policy_.shouldSiftUp(newValue, model_.get(parent(i))))
    siftUp(i, out);
  else
    siftDown(i, out);
  HeapStep d; d.type = StepType::Done;
  out.push_back(d);
  return out;
}

std::vector<HeapStep> HeapEngine::heapify() {
  std::vector<HeapStep> out;
  size_t n = model_.size();
  if (n <= 1) { HeapStep d; d.type = StepType::Done; out.push_back(d); return out; }
  for (size_t i = (n / 2); i > 0; --i)
    siftDown(i - 1, out);
  HeapStep d; d.type = StepType::Done;
  out.push_back(d);
  return out;
}

std::vector<HeapStep> HeapEngine::buildFromArray(const std::vector<int>& values, BuildMode mode) {
  std::vector<HeapStep> out;
  model_.clear();
  if (mode == BuildMode::InsertAll) {
    for (int v : values) {
      auto steps = insert(v);
      for (auto& s : steps) out.push_back(s);
    }
  } else {
    for (int v : values) model_.pushBack(v);
    HeapStep msg; msg.type = StepType::Message; msg.message = "Bottom-up heapify";
    out.push_back(msg);
    size_t n = model_.size();
    if (n > 1) {
      for (size_t i = (n / 2); i > 0; --i)
        siftDown(i - 1, out);
    }
    HeapStep d; d.type = StepType::Done; out.push_back(d);
  }
  return out;
}

void HeapEngine::applyStep(HeapModel& model, const HeapStep& step) {
  switch (step.type) {
    case StepType::Swap:
      if (step.i < model.size() && step.j < model.size()) model.swap(step.i, step.j);
      break;
    case StepType::SetValue:
      if (step.i >= model.size()) {
        while (model.size() <= step.i) model.pushBack(step.value);
      } else {
        model.set(step.i, step.value);
      }
      break;
    case StepType::PopBack:
      if (!model.empty()) model.popBack();
      break;
    default:
      break;
  }
}

} // namespace heap
