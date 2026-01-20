#pragma once

#include "Razix/Core/Reflection/RZReflectionRegistry.h"

// This global instance automatically registers the type and its metadata into the type registry
// Usage:
// RAZIX_REFLECT_TYPE_START(MyStruct)
//    RAZIX_REFLECT_MEMBER(member1)
//    RAZIX_REFLECT_MEMBER(member2)
//    ...
//    RAZIX_REFLECT_TYPE_END(MyStruct)
// This will create a static instance that registers MyStruct's metadata at program startup
#define RAZIX_REFLECT_TYPE_START(Type)                      \
    namespace Type##_TypeRegistrationNS                     \
    {                                                       \
        struct Type##_TypeRegistration                      \
        {                                                   \
            Type##_TypeRegistration()                       \
            {                                               \
                using refType = Type;                       \
                TypeMetaData metaData;                      \
                metaData.name     = #Type;                  \
                metaData.typeName = typeid(refType).name(); \
                metaData.size     = sizeof(refType);

#define RAZIX_REFLECT_TYPE_END(Type)                                                        \
    Razix::RZTypeRegistry::registerType<refType>(metaData);                                 \
    }                                                                                       \
    }                                                                                       \
    ;                                                                                       \
    /* Instantiate the TypeRegistration struct inside a static block to register it once */ \
    static const Type##_TypeRegistration global_##Type##_registration_instance{};           \
    }


#define RAZIX_REFLECT_MEMBER(Member) \
    metaData.members.push_back({#Member, typeid(decltype(refType::Member)).name(), offsetof(refType, Member), sizeof(refType::Member)});

#define RAZIX_REFLECT_MEMBER(Member) \
    metaData.members.push_back({#Member, typeid(decltype(refType::Member)).name(), offsetof(refType, Member), sizeof(refType::Member)});
