#include "fg/ResourceEntry.hpp"

std::string ResourceEntry::toString() const { return m_concept->toString(); }

void ResourceEntry::create(void *allocator) {
  assert(isTransient());
  m_concept->create(allocator);
}
void ResourceEntry::destroy(void *allocator) {
  assert(isTransient());
  m_concept->destroy(allocator);
}

uint32_t ResourceEntry::getVersion() const { return m_version; }
bool ResourceEntry::isImported() const { return m_imported; }
bool ResourceEntry::isTransient() const { return !m_imported; }
