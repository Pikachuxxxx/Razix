

//-----------------------------------------------------------------------------------
// Builder Class
//-----------------------------------------------------------------------------------

ENFORCE_CONCEPT_IMPL inline RZFrameGraphResource RZFrameGraph::RZBuilder::create(const std::string_view name, typename T::Desc &&desc)
{
    const auto id = m_FrameGraph.createResource<T>(name, std::move(desc));
    return m_PassNode.m_Creates.emplace_back(id);
}

//-----------------------------------------------------------------------------------
// Frame Graph Class
//-----------------------------------------------------------------------------------

template<typename PassData, typename SetupFunc, typename ExecuteFunc>
inline const PassData &RZFrameGraph::addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc)
{
    static_assert(std::is_invocable<SetupFunc, RZBuilder &, PassData &>::value,
        "Invalid setup callback");
    static_assert(std::is_invocable<ExecuteFunc, const PassData &, RZFrameGraphPassResources &, void *>::value,
        "Invalid exec callback");
    static_assert(sizeof(ExecuteFunc) < 1024, "Execute captures too much");

    auto *pass = new RZFrameGraphPass<PassData, ExecuteFunc>(std::forward<ExecuteFunc>(executeFunc));
    auto &passNode =
        createPassNode(name, std::unique_ptr<RZFrameGraphPass<PassData, ExecuteFunc>>(pass));
    RZBuilder builder{*this, passNode};
    std::invoke(setupFunc, builder, pass->data);
    return pass->data;
}

template<typename SetupFunc, typename ExecuteFunc>
inline void RZFrameGraph::addCallbackPass(const std::string_view name, SetupFunc &&setupFunc, ExecuteFunc &&executeFunc)
{
    struct NoData
    {};
    addCallbackPass<NoData>(name, setupFunc, std::forward<ExecuteFunc>(executeFunc));
}

ENFORCE_CONCEPT_IMPL inline RZFrameGraphResource RZFrameGraph::import(const std::string_view name, typename T::Desc &&desc, T &&resource)
{
    const auto resourceId = static_cast<u32>(m_ResourceRegistry.size());
    m_ResourceRegistry.emplace_back(
        RZResourceEntry{resourceId, std::forward<typename T::Desc>(desc), std::forward<T>(resource), kResourceInitialVersion, true});
    return createResourceNode(name, resourceId).m_ID;
}
ENFORCE_CONCEPT_IMPL inline typename const T::Desc &RZFrameGraph::getDescriptor(RZFrameGraphResource id)
{
    return getResourceEntry(id).getTypeResource<T>()->descriptor;
}

ENFORCE_CONCEPT_IMPL inline RZFrameGraphResource RZFrameGraph::createResource(const std::string_view name, typename T::Desc &&desc)
{
    const auto resourceId = static_cast<u32>(m_ResourceRegistry.size());
    m_ResourceRegistry.emplace_back(RZResourceEntry{resourceId, std::forward<typename T::Desc>(desc), T{}, kResourceInitialVersion});
    return createResourceNode(name, resourceId).m_ID;
}

//-----------------------------------------------------------------------------------
// RZFrameGraphPassResources Class
//-----------------------------------------------------------------------------------

ENFORCE_CONCEPT_IMPL inline T &RZFrameGraphPassResources::get(RZFrameGraphResource id)
{
    assert(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) ||
           m_PassNode.canWriteResouce(id));
    return m_FrameGraph.getResourceEntry(id).get<T>();
}

ENFORCE_CONCEPT_IMPL inline typename const T::Desc &RZFrameGraphPassResources::getDescriptor(RZFrameGraphResource id) const
{
    assert(m_PassNode.canReadResouce(id) || m_PassNode.canCreateResouce(id) ||
           m_PassNode.canWriteResouce(id));
    return m_FrameGraph.getResourceEntry(id).getDescriptor<T>();
}
