#pragma once

#include "fg/TypeTraits.hpp"
#include "fg/ResourceNode.hpp"
#include "fg/ResourceEntry.hpp"

constexpr auto kFlagsIgnored = ~0;

class FrameGraph {
  friend class FrameGraphPassResources;

public:
  FrameGraph() = default;
  FrameGraph(const FrameGraph &) = delete;
  FrameGraph(FrameGraph &&) noexcept = delete;

  FrameGraph &operator=(const FrameGraph &) = delete;
  FrameGraph &operator=(FrameGraph &&) noexcept = delete;

  friend std::ostream &operator<<(std::ostream &, const FrameGraph &);

  class Builder final {
    friend class FrameGraph;

  public:
    Builder() = delete;
    Builder(const Builder &) = delete;
    Builder(Builder &&) noexcept = delete;

    Builder &operator=(const Builder &) = delete;
    Builder &operator=(Builder &&) noexcept = delete;

    _VIRTUALIZABLE_CONCEPT
    /** Declares the creation of a resource. */
    [[nodiscard]] FrameGraphResource create(const std::string_view name,
                                            typename T::Desc);
    /** Declares read operation. */
    FrameGraphResource read(FrameGraphResource id,
                            uint32_t flags = kFlagsIgnored);
    /**
     * Declares write operation.
     * @remark Writing to imported resource counts as side-effect.
     */
    [[nodiscard]] FrameGraphResource write(FrameGraphResource id,
                                           uint32_t flags = kFlagsIgnored);

    /** Ensures that this pass is not culled during the compilation phase. */
    Builder &setSideEffect();

  private:
    Builder(FrameGraph &, PassNode &);

  private:
    FrameGraph &m_frameGraph;
    PassNode &m_passNode;
  };

  void reserve(uint32_t numPasses, uint32_t numResources);

  /**
   * @param setup Callback (lambda, may capture by reference), invoked
   * immediatly, declare operations here.
   * @param exec Execution of this lambda is deferred until execute() phase
   * (must capture by value due to this).
   */
  template <typename Data = std::monostate, typename Setup, typename Execute>
  const Data &addCallbackPass(const std::string_view name, Setup &&setup,
                              Execute &&exec);

  _VIRTUALIZABLE_CONCEPT
  [[nodiscard]] const typename T::Desc &getDescriptor(FrameGraphResource id);

  _VIRTUALIZABLE_CONCEPT
  /** Imports the given resource T into FrameGraph. */
  [[nodiscard]] FrameGraphResource import(const std::string_view name,
                                          typename T::Desc &&, T &&);

  /** @return True if the given resource is valid for read/write operation. */
  [[nodiscard]] bool isValid(FrameGraphResource id) const;

  /** Culls unreferenced resources and passes. */
  void compile();
  /** Invokes execution callbacks. */
  void execute(void *context = nullptr, void *allocator = nullptr);

  void exportGraphviz(std::ostream &) const;

private:
  [[nodiscard]] PassNode &
  _createPassNode(const std::string_view name,
                  std::unique_ptr<FrameGraphPassConcept> &&);

  _VIRTUALIZABLE_CONCEPT
  [[nodiscard]] FrameGraphResource _create(const std::string_view name,
                                           typename T::Desc &&);

  [[nodiscard]] ResourceNode &_createResourceNode(const std::string_view name,
                                                  uint32_t resourceId);
  /** Increments ResourceEntry version and produces a renamed handle. */
  [[nodiscard]] FrameGraphResource _clone(FrameGraphResource id);

  /** @param id ResourceNode id */
  [[nodiscard]] const ResourceNode &
  _getResourceNode(FrameGraphResource id) const;
  /** @param id ResourceNode id */
  [[nodiscard]] ResourceEntry &_getResourceEntry(FrameGraphResource id);

private:
  std::vector<PassNode> m_passNodes;
  std::vector<ResourceNode> m_resourceNodes;
  std::vector<ResourceEntry> m_resourceRegistry;
};

class FrameGraphPassResources {
  friend class FrameGraph;

public:
  FrameGraphPassResources() = delete;
  ~FrameGraphPassResources() = default;

  /**
   * @note Causes runtime-error with:
   * - Attempt to use obsolete handle (the one that has been renamed before)
   * - Incorrect resource type T
   */
  _VIRTUALIZABLE_CONCEPT
  [[nodiscard]] T &get(FrameGraphResource id);
  _VIRTUALIZABLE_CONCEPT
  [[nodiscard]] const typename T::Desc &
  getDescriptor(FrameGraphResource id) const;

private:
  FrameGraphPassResources(FrameGraph &, PassNode &);

private:
  FrameGraph &m_frameGraph;
  PassNode &m_passNode;
};

#include "fg/FrameGraph.inl"
