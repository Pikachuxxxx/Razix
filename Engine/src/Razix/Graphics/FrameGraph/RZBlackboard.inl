

template<typename T, typename... Args>
inline T& RZBlackboard::add(Args&&... args)
{
    assert(!has<T>());
    return m_Storage[typeid(T)].emplace<T>(T{std::forward<Args>(args)...});
}

template<typename T>
inline bool RZBlackboard::has() const
{
    return m_Storage.find(typeid(T)) != m_Storage.cend();
}

template<typename T>
inline const T* RZBlackboard::try_get()
{
    return const_cast<T*>(
        const_cast<const RZBlackboard*>(this)->try_get<T>());
}

template<typename T>
inline const T& RZBlackboard::get()
{
    return const_cast<T&>(
        const_cast<const RZBlackboard*>(this)->get<T>());
}

template<typename T>
inline const T* RZBlackboard::try_get() const
{
    auto it = m_Storage.find(typeid(T));
    return it != m_Storage.cend() ? std::any_cast<const T>(&it->second) : nullptr;
}

template<typename T>
inline const T& RZBlackboard::get() const
{
    assert(has<T>());
    return std::any_cast<const T&>(m_Storage.at(typeid(T)));
}
