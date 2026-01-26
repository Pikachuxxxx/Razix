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

// Friend helped to access private/protected members for reflection
#define RAZIX_REFLECT_FRIEND_FWD_DECL(Type) \
    namespace Type##_TypeRegistrationNS \
    {                                   \
        struct Type##_TypeRegistration; \
    }
#define RAZIX_REFLECT_FRIEND(Type) \
    friend struct Type##_TypeRegistrationNS::Type##_TypeRegistration;

#define RAZIX_REFLECT_PRIMITIVE(Member)           \
    metaData.members.push_back({                  \
        #Member,                                  \
        typeid(decltype(refType::Member)).name(), \
        offsetof(refType, Member),                \
        SerializeableDataType::kPrimitive,        \
        {0},                                      \
        {sizeof(refType::Member)},                \
    });

// Note: Anyone that isn't a kPrimitive is considered non-trivially serializable, it will dirty the flag

// Size is dynamic for blobs, use placeholder
#define RAZIX_REFLECT_BLOB(Member, Size)          \
    metaData.bIsTriviallySerializable = false;    \
    metaData.members.push_back({                  \
        #Member,                                  \
        typeid(decltype(refType::Member)).name(), \
        offsetof(refType, Member),                \
        SerializeableDataType::kBlob,             \
        0,                                        \
        {Size},                                   \
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

// TODO: define other types here

// NOTE: Dynamic size, queried at runtime, stored as 0 during reflection
#define RAZIX_REFLECT_ARRAY(Member)                                    \
    do {                                                               \
        metaData.bIsTriviallySerializable = false;                     \
        using MemberT                     = decltype(refType::Member); \
        MemberMetaData m{};                                            \
                                                                       \
        m.name     = #Member;                                          \
        m.typeName = typeid(MemberT).name();                           \
        m.offset   = offsetof(refType, Member);                        \
        m.dataType = SerializeableDataType::kArray;                    \
                                                                       \
        m.isStaticCompileSizedFixed = 0;                               \
                                                                       \
        m.array.elementSize =                                          \
            static_cast<u32>(MemberT::static_type_size());             \
                                                                       \
        m.array.elementCount = 0;                                      \
                                                                       \
        m.array.ops = make_array_ops<MemberT>();                       \
                                                                       \
        metaData.members.push_back(m);                                 \
    } while (0);

#define RAZIX_REFLECT_FIXED_ARRAY(Member)                              \
    do {                                                               \
        metaData.bIsTriviallySerializable = false;                     \
        using MemberT                     = decltype(refType::Member); \
        MemberMetaData m{};                                            \
                                                                       \
        m.name     = #Member;                                          \
        m.typeName = typeid(MemberT).name();                           \
        m.offset   = offsetof(refType, Member);                        \
        m.dataType = SerializeableDataType::kArray;                    \
                                                                       \
        m.isStaticCompileSizedFixed = 1;                               \
                                                                       \
        m.array.elementSize =                                          \
            static_cast<u32>(MemberT::static_type_size());             \
                                                                       \
        m.array.elementCount =                                         \
            static_cast<u32>(MemberT::static_capacity());              \
                                                                       \
        m.array.ops = make_array_ops<MemberT>();                       \
                                                                       \
        metaData.members.push_back(m);                                 \
    } while (0);

#define RAZIX_REFLECT_STRING(Member)                                   \
    do {                                                               \
        metaData.bIsTriviallySerializable = false;                     \
        using MemberT                     = decltype(refType::Member); \
        MemberMetaData m{};                                            \
                                                                       \
        m.name     = #Member;                                          \
        m.typeName = typeid(MemberT).name();                           \
        m.offset   = offsetof(refType, Member);                        \
        m.dataType = SerializeableDataType::kString;                   \
                                                                       \
        m.string.ops = make_array_ops<MemberT>();                      \
                                                                       \
        metaData.members.push_back(m);                                 \
    } while (0);

#define RAZIX_REFLECT_HASHMAP(Member)                                             \
    do {                                                                          \
        metaData.bIsTriviallySerializable = false;                                \
        using MemberT                     = decltype(refType::Member);            \
        MemberMetaData m{};                                                       \
                                                                                  \
        m.name     = #Member;                                                     \
        m.typeName = typeid(MemberT).name();                                      \
        m.offset   = offsetof(refType, Member);                                   \
        m.dataType = SerializeableDataType::kHashMap;                             \
                                                                                  \
        m.map.ops       = make_hashmap_ops<MemberT>();                            \
        m.map.keySize   = static_cast<u32>(sizeof(typename MemberT::key_type));   \
        m.map.valueSize = static_cast<u32>(sizeof(typename MemberT::value_type)); \
        m.map.keyType   = typeid(typename MemberT::key_type);                     \
        m.map.valueType = typeid(typename MemberT::value_type);                   \
                                                                                  \
        metaData.members.push_back(m);                                            \
    } while (0);

#define RAZIX_REFLECT_OBJECT(Member)                                                                                                                                                                                                        \
    do {                                                                                                                                                                                                                                    \
        metaData.bIsTriviallySerializable = false;                                                                                                                                                                                          \
        using MemberT                     = decltype(refType::Member);                                                                                                                                                                      \
        MemberMetaData m{};                                                                                                                                                                                                                 \
        RAZIX_CORE_ASSERT(Razix::RZTypeRegistry::getTypeMetaData(typeid(MemberT)) != NULL, "RAZIX_REFLECT_OBJECT: Type " #Member " is not registered in the Type Registry. Please register it using RAZIX_REFLECT_TYPE_START/END macros."); \
                                                                                                                                                                                                                                            \
        if (Razix::RZTypeRegistry::isTypeTriviallySerializable(typeid(MemberT))) {                                                                                                                                                          \
            metaData.bIsTriviallySerializable = true;                                                                                                                                                                                       \
        }                                                                                                                                                                                                                                   \
                                                                                                                                                                                                                                            \
        m.name     = #Member;                                                                                                                                                                                                               \
        m.typeName = typeid(MemberT).name();                                                                                                                                                                                                \
        m.offset   = offsetof(refType, Member);                                                                                                                                                                                             \
        m.dataType = SerializeableDataType::kObject;                                                                                                                                                                                        \
                                                                                                                                                                                                                                            \
        m.object.type = typeid(MemberT);                                                                                                                                                                                                    \
                                                                                                                                                                                                                                            \
        metaData.members.push_back(m);                                                                                                                                                                                                      \
    } while (0);

#define RAZIX_REFLECT_UUID(Member)                                     \
    do {                                                               \
        metaData.bIsTriviallySerializable = true;                      \
        using MemberT                     = decltype(refType::Member); \
        MemberMetaData m{};                                            \
                                                                       \
        m.name     = #Member;                                          \
        m.typeName = typeid(MemberT).name();                           \
        m.offset   = offsetof(refType, Member);                        \
        m.dataType = SerializeableDataType::kUUID;                     \
                                                                       \
        m.uuid.ops = make_uuid_ops();                                  \
                                                                       \
        metaData.members.push_back(m);                                 \
    } while (0);

#endif    // _RZ_REFLECTION_H_
