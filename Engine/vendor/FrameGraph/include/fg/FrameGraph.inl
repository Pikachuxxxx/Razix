//
// FrameGraph class:
//

template <typename Data, typename Setup, typename Execute>
inline const Data &FrameGraph::addCallbackPass(const std::string_view name,
                                               Setup &&setup, Execute &&exec) {
  static_assert(std::is_invocable_v<Setup, Builder &, Data &>,
                "Invalid setup callback");
  static_assert(std::is_invocable_v<Execute, const Data &,
                                    FrameGraphPassResources &, void *>,
                "Invalid exec callback");
  static_assert(sizeof(Execute) < 1024, "Execute captures too much");

  auto *pass = new FrameGraphPass<Data, Execute>(std::forward<Execute>(exec));
  auto &passNode =
    _createPassNode(name, std::unique_ptr<FrameGraphPass<Data, Execute>>(pass));
  Builder builder{*this, passNode};
  std::invoke(setup, builder, pass->data);
  return pass->data;
}

_VIRTUALIZABLE_CONCEPT_IMPL
inline const typename T::Desc &
FrameGraph::getDescriptor(FrameGraphResource id) {
  return _getResourceEntry(id)._getModel<T>()->descriptor;
}

_VIRTUALIZABLE_CONCEPT_IMPL
inline FrameGraphResource FrameGraph::import(const std::string_view name,
                                             typename T::Desc &&desc,
                                             T &&resource) {
  const auto resourceId = static_cast<uint32_t>(m_resourceRegistry.size());
  m_resourceRegistry.emplace_back(
    ResourceEntry{resourceId, std::forward<typename T::Desc>(desc),
                  std::forward<T>(resource), kResourceInitialVersion, true});
  return _createResourceNode(name, resourceId).m_id;
}

_VIRTUALIZABLE_CONCEPT_IMPL
inline FrameGraphResource FrameGraph::_create(const std::string_view name,
                                              typename T::Desc &&desc) {
  const auto resourceId = static_cast<uint32_t>(m_resourceRegistry.size());
  m_resourceRegistry.emplace_back(
    ResourceEntry{resourceId, std::forward<typename T::Desc>(desc), T{},
                  kResourceInitialVersion});
  return _createResourceNode(name, resourceId).m_id;
}

//
// FrameGraph::Builder class:
//

_VIRTUALIZABLE_CONCEPT_IMPL
inline FrameGraphResource
FrameGraph::Builder::create(const std::string_view name,
                            typename T::Desc desc) {
  const auto id = m_frameGraph._create<T>(name, std::move(desc));
  return m_passNode.m_creates.emplace_back(id);
}

//
// FrameGraphPassResources class:
//

_VIRTUALIZABLE_CONCEPT_IMPL
inline T &FrameGraphPassResources::get(FrameGraphResource id) {
  assert(m_passNode.reads(id) || m_passNode.creates(id) ||
         m_passNode.writes(id));
  return m_frameGraph._getResourceEntry(id).get<T>();
}

_VIRTUALIZABLE_CONCEPT_IMPL
inline const typename T::Desc &
FrameGraphPassResources::getDescriptor(FrameGraphResource id) const {
  assert(m_passNode.reads(id) || m_passNode.creates(id) ||
         m_passNode.writes(id));
  return m_frameGraph._getResourceEntry(id).getDescriptor<T>();
}
