#pragma once

namespace Razix {

    // Define a counter for resource types
    enum ResourceType
    {
        RAZIX_RESOURCE_TYPE_NONE = 0,
        // Add more resource types here as they are registered
    };

// Macro to register a resource type and generate necessary functions
#define RAZIX_REGISTER_RESOURCE(resourceName, descType, ...) \
    REGISTER_RESOURCE_INTERNAL(resourceName, descType, ##__VA_ARGS__)

// Macro to generate RAZIX_REGISTERED_RESOURCES
#define REGISTER_RESOURCE_INTERNAL(resourceName, descType, ...) \
    RAZIX_RESOURCE_TYPE(resourceName)                           \
    RAZIX_REGISTER_RESOURCE_##resourceName(descType, ##__VA_ARGS__)

// Example of individual resource registration macros
// Replace these with actual resource types and definitions
#define RAZIX_REGISTER_RESOURCE_Texture(descType, ...) \
    RAZIX_REGISTER_RESOURCE_IMPL(Texture, descType, ##__VA_ARGS__)

#define RAZIX_REGISTER_RESOURCE_Shader(descType, ...) \
    RAZIX_REGISTER_RESOURCE_IMPL(Shader, descType, ##__VA_ARGS__)

#define RAZIX_REGISTER_RESOURCE_Pipeline(descType, ...) \
    RAZIX_REGISTER_RESOURCE_IMPL(Pipeline, descType, ##__VA_ARGS__)

// Macro to generate the actual resource registration
#define RAZIX_REGISTER_RESOURCE_IMPL(resourceName, ...)                                    \
public:                                                                                    \
    RZ##resourceName##Handle create##resourceName(__VA_ARGS__);                            \
    void                     destroy##resourceName(RZ##resourceName##Handle handle);       \
    RZ##resourceName*        get##resourceName##Resource(RZ##resourceName##Handle handle); \
                                                                                           \
private:                                                                                   \
    RZResourcePoolTyped<RZ##resourceName> m_##resourceName##Pool;                          \
                                                                                           \
public:                                                                                    \
    static constexpr const char*  resourceName##_type_name = #resourceName;                \
    static constexpr ResourceType resourceName##_type_id   = __COUNTER__ + 1;

// Generate the resource type enumeration based on registered resources
#define RAZIX_RESOURCE_TYPE(resourceName) \
    static_assert(resourceName##_type_id != RAZIX_RESOURCE_TYPE_NONE, "Resource type ID cannot be zero");

    // Define a structure to hold pool initialization parameters
    struct RZResourcePoolInitParams
    {
        size_t capacity;
        size_t instanceSize;
    };

    // Define a map to store initialization parameters for each resource type
    static std::unordered_map<RZString, RZResourcePoolInitParams> resourceInitParamsMap;

// Macro to register resource pools
#define RAZIX_REGISTER_RESOURCE_POOL(resourceName, capacity, instanceSize, ...)            \
public:                                                                                    \
    template<>                                                                             \
    RZResourcePoolTyped<RZ##resourceName>& getPool()                                       \
    {                                                                                      \
        static RZResourcePoolTyped<RZ##resourceName> m_##resourceName##TypedPool;          \
        static bool                                  initialized = false;                  \
        if (!initialized) {                                                                \
            initialized = true;                                                            \
            m_##resourceName##TypedPool.init(capacity, instanceSize);                      \
        }                                                                                  \
        return m_##resourceName##TypedPool;                                                \
    }                                                                                      \
                                                                                           \
public:                                                                                    \
    RZ##resourceName##Handle create##resourceName(__VA_ARGS__);                            \
    void                     destroy##resourceName(RZ##resourceName##Handle handle);       \
    RZ##resourceName*        get##resourceName##Resource(RZ##resourceName##Handle handle); \
                                                                                           \
private:                                                                                   \
    static void registerResourceInitParams_##resourceName()                                \
    {                                                                                      \
        resourceInitParamsMap[#resourceName] = {capacity, sizeof(RZ##resourceName)};       \
        initResourcePool<RZ##resourceName>(capacity, instanceSize);                        \
    }                                                                                      \
    static void unregisterResourceInitParams_##resourceName()                              \
    {                                                                                      \
        resourceInitParamsMap.erase(#resourceName);                                        \
    }                                                                                      \
    friend class ResourceManager;                                                          \
                                                                                           \
    template<typename T>                                                                   \
    static void initResourcePool(size_t capacity, size_t instanceSize)                     \
    {                                                                                      \
        ResourceManager::get().getPool<T>().init(capacity, instanceSize);                  \
    }

}    // namespace Razix