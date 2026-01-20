#ifndef _RZ_REFLECTION_H_
#define _RZ_REFLECTION_H_

#include "Razix/Core/Reflection/RZReflectionRegistry.h"

#ifndef __cplusplus
    #error "This header requires C++"
#endif    // __cplusplus

// TODO: use fixed sizes instead of sizeof for compatibility across platforms

// This global instance automatically registers the type and its metadata into the type registry
// Usage:
// RAZIX_REFLECT_TYPE_START(MyStruct)
//    RAZIX_REFLECT_PRIMITIVE(member1)
//    RAZIX_REFLECT_PRIMITIVE(member2)
//    ...
//    RAZIX_REFLECT_TYPE_END(MyStruct)
// This will create a static instance that registers MyStruct's metadata at program startup
// clang-format off
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

// RAZIX_REFLECT_XXX macros defined by the user go here inside the struct constructor

#define RAZIX_REFLECT_TYPE_END(Type)                                                                \
                Razix::RZTypeRegistry::registerType<refType>(metaData);                             \
            }                                                                                       \
        };                                                                                          \
        /* Instantiate the TypeRegistration struct inside a static block to register it once */    \
        static const Type##_TypeRegistration global_##Type##_registration_instance{};               \
    }
// clang-format on

#define RAZIX_REFLECT_PRIMITIVE(Member)           \
    metaData.members.push_back({                  \
        #Member,                                  \
        typeid(decltype(refType::Member)).name(), \
        offsetof(refType, Member),                \
        sizeof(refType::Member),                  \
        SerializeableDataType::kPrimitive,        \
        0,                                        \
    });

// Note: Anyone that isn't a kPrimitive is considered non-trivially serializable, it will dirty the flag

// Size is dynamic for blobs, use placeholder
#define RAZIX_REFLECT_BLOB(Member, Size)          \
    metaData.bIsTriviallySerializable = false;    \
    metaData.members.push_back({                  \
        #Member,                                  \
        typeid(decltype(refType::Member)).name(), \
        offsetof(refType, Member),                \
        Size,                                     \
        SerializeableDataType::kBlob,             \
        0,                                        \
    });

// TODO: register a function pointer to get the dynamic size at runtime
// TODO: Define a new type for functor-based size retrieval if needed, kDynamicBlob?
// This is just theoretical for now
//#define RAZIX_REFLECT_DYNAMIC_BLOB(Member, SizeFunctor) \
//    metaData.members.push_back({                        \
//        #Member,                                        \
//        typeid(decltype(refType::Member)).name(),       \
//        offsetof(refType, Member),                      \
//        SizeFunctor,                                    \
//        SerializeableDataType::kBlob,                   \
//        0,                                              \
//    });

// TODO define other types here

#endif    // _RZ_REFLECTION_H_