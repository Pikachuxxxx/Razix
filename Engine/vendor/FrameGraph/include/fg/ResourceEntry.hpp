#pragma once

#include "fg/TypeTraits.hpp"
#include "fg/PassNode.hpp"
#include "fg/TypeTraits.hpp"

// http://www.cplusplus.com/articles/oz18T05o/
// https://www.modernescpp.com/index.php/c-core-guidelines-type-erasure-with-templates

// Wrapper around a virtual resource
class ResourceEntry final {
  friend class FrameGraph;

public:
  ResourceEntry() = delete;
  ResourceEntry(const ResourceEntry &) = delete;
  ResourceEntry(ResourceEntry &&) noexcept = default;

  ResourceEntry &operator=(const ResourceEntry &) = delete;
  ResourceEntry &operator=(ResourceEntry &&) noexcept = default;

  [[nodiscard]] std::string toString() const;

  void create(void *allocator);
  void destroy(void *allocator);

  void preRead(uint32_t flags, void *context) {
    m_concept->preRead(flags, context);
  }
  void preWrite(uint32_t flags, void *context) {
    m_concept->preWrite(flags, context);
  }

  [[nodiscard]] uint32_t getVersion() const;
  [[nodiscard]] bool isImported() const;
  [[nodiscard]] bool isTransient() const;

  template <typename T> [[nodiscard]] T &get();
  template <typename T>
  [[nodiscard]] const typename T::Desc &getDescriptor() const;

private:
  template <typename T>
  ResourceEntry(uint32_t id, typename T::Desc &&, T &&, uint32_t version,
                bool imported = false);

  struct Concept {
    virtual ~Concept() = default;

    virtual void create(void *) = 0;
    virtual void destroy(void *) = 0;

    virtual void preRead(uint32_t flags, void *) = 0;
    virtual void preWrite(uint32_t flags, void *) = 0;

    virtual std::string toString() const = 0;
  };
  template <typename T> struct Model : Concept {
    Model(typename T::Desc &&, T &&);

    void create(void *allocator) final;
    void destroy(void *allocator) final;

    void preRead(uint32_t flags, void *context) override {
#if _HAS_CXX20
      if constexpr (has_preRead<T>)
#else
      if constexpr (has_preRead<T>::value)
#endif
        resource.preRead(descriptor, flags, context);
    }
    void preWrite(uint32_t flags, void *context) override {
#if _HAS_CXX20
      if constexpr (has_preWrite<T>)
#else
      if constexpr (has_preWrite<T>::value)
#endif
        resource.preWrite(descriptor, flags, context);
    }

    std::string toString() const final;

    const typename T::Desc descriptor;
    T resource;
  };

  template <typename T> [[nodiscard]] auto *_getModel() const;

private:
  const uint32_t m_id;
  std::unique_ptr<Concept> m_concept;
  uint32_t m_version;    // Incremented on each (unique) write declaration
  const bool m_imported; // Imported or transient

  PassNode *m_producer{nullptr};
  PassNode *m_last{nullptr};
};

#include "ResourceEntry.inl"
