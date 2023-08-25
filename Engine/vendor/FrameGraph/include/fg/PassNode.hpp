#pragma once

#include "fg/GraphNode.hpp"
#include "fg/PassEntry.hpp"
#include "fg/FrameGraphResource.hpp"
#include <memory>
#include <vector>

class PassNode final : public GraphNode {
  friend class FrameGraph;

public:
  struct AccessDeclaration {
    FrameGraphResource id;
    uint32_t flags;

    bool operator==(const AccessDeclaration &) const = default;
  };

  [[nodiscard]] bool creates(FrameGraphResource id) const;
  [[nodiscard]] bool reads(FrameGraphResource id) const;
  [[nodiscard]] bool writes(FrameGraphResource id) const;

  [[nodiscard]] bool hasSideEffect() const;
  [[nodiscard]] bool canExecute() const;

private:
  PassNode(const std::string_view name, uint32_t id,
           std::unique_ptr<FrameGraphPassConcept> &&);

  FrameGraphResource _read(FrameGraphResource id, uint32_t flags);
  [[nodiscard]] FrameGraphResource _write(FrameGraphResource id,
                                          uint32_t flags);

private:
  std::unique_ptr<FrameGraphPassConcept> m_exec;

  std::vector<FrameGraphResource> m_creates;

  std::vector<AccessDeclaration> m_reads;
  std::vector<AccessDeclaration> m_writes;

  bool m_hasSideEffect{false};
};
