#pragma once

class FrameGraphPassResources;

struct FrameGraphPassConcept {
  FrameGraphPassConcept() = default;
  FrameGraphPassConcept(const FrameGraphPassConcept &) = delete;
  FrameGraphPassConcept(FrameGraphPassConcept &&) noexcept = delete;
  virtual ~FrameGraphPassConcept() = default;

  FrameGraphPassConcept &operator=(const FrameGraphPassConcept &) = delete;
  FrameGraphPassConcept &operator=(FrameGraphPassConcept &&) noexcept = delete;

  virtual void operator()(FrameGraphPassResources &, void *) = 0;
};

template <typename Data, typename Execute>
struct FrameGraphPass final : FrameGraphPassConcept {
  explicit FrameGraphPass(Execute &&exec)
      : execFunction{std::forward<Execute>(exec)} {}

  void operator()(FrameGraphPassResources &resources, void *context) override {
    execFunction(data, resources, context);
  }

  Execute execFunction;
  Data data{};
};
