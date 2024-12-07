#pragma once

#include "Razix/Core/Reflection/RZReflectionRegistry.h"

#define REFLECT_TYPE_START(Type)                            \
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

#define REFLECT_MEMBER(Member) \
    metaData.members.push_back({#Member, typeid(decltype(refType::Member)).name(), offsetof(refType, Member), sizeof(refType::Member)});

#define REFLECT_TYPE_END(Type)                                                              \
    Razix::RZTypeRegistry::registerType<refType>(metaData);                                 \
    }                                                                                       \
    }                                                                                       \
    ;                                                                                       \
    /* Instantiate the TypeRegistration struct inside a static block to register it once */ \
    static const Type##_TypeRegistration global_##Type##_registration_instance{};           \
    }