#pragma once

/****************************************************************************************************
 *                                  Settings based on OS/Compiler                                   *
 ****************************************************************************************************/
// Settings for Windows OS
#ifdef RAZIX_PLATFORM_WINDOWS

    // Disable any MSVC specific warnings
    #pragma warning(disable : 4251)

    // 16-byte Memory alignment for the Windows OS/Engine
    // Well I mean we might choose different alignments on different platforms so this is set per platform explicitly
    #define MEM_DEF_ALIGNMENT_16 16
    //#define RAZIX_MEM_ALIGN __declspec(align(MEM_ALIGNMENT))) // This is MSVC pre c++11 extension
    #define RAZIX_MEM_ALIGN_16 alignas(MEM_DEF_ALIGNMENT_16)    // This is from std c++ that is supposed to be available everywhere

    // Symbols Export settings for Engine
    #ifdef RAZIX_BUILD_DLL
        #define RAZIX_API __declspec(dllexport)
    #else
        #define RAZIX_API __declspec(dllimport)
    #endif

    // Debug functions for breaking the debugger or interrupting the code
    #define RAZIX_DEBUG_BREAK() __debugbreak()    // MSVC

    // IDK how to make a symbol to be hidden during exporting in MSVC I guess it does that explicitly where as MacOS/clang needs something explicit as shown below
    #define RAZIX_HIDDEN

#else
    #define MEM_DEF_ALIGNMENT_16 16

    #define RAZXI_API           __attribute__((visibility("default")))
    #define RAZIX_HIDDEN        __attribute__((visibility("hidden")))
    #define RAZIX_DEBUG_BREAK() raise(SIGTRAP);
    #define RAZIX_MEM_ALIGN_16  alignas(MEM_ALIGNMENT)

#endif

/****************************************************************************************************
 *                              Settings based on Configuration                                     *
 ****************************************************************************************************/
#ifdef RAZIX_DEBUG
    #define RAZIX_ENABLE_ASSERTS
#endif

// TODO: Add Debug methods calls to strip them out in release and production builds
/****************************************************************************************************
  *                                         Core Defines                                            *
  ****************************************************************************************************/

// Asserts
#ifdef RAZIX_ENABLE_ASSERTS    // Only available in Debug Builds (predefined by the build system only in Debug mode) \
                               // Predefined conditioned Assert                                                      \
                               // TODO: Use the generic condition assert macro and pass this specific condition

    #define RAZIX_CONSTEXPR_ASSERT(x, ...)                                                                           \
        {                                                                                                            \
            if constexpr (!(x)) {                                                                                    \
                RAZIX_CORE_ERROR("Assertions Failed: {0} at Line {1} in File {2}", __VA_ARGS__, __LINE__, __FILE__); \
                RAZIX_DEBUG_BREAK();                                                                                 \
            }                                                                                                        \
        }

// TODO: Define a Razix Static Assert

    #define RAZIX_CORE_ASSERT(x, ...)                                                                                \
        {                                                                                                            \
            if (!(x)) {                                                                                              \
                RAZIX_CORE_ERROR("Assertions Failed: {0} at Line {1} in File {2}", __VA_ARGS__, __LINE__, __FILE__); \
                RAZIX_DEBUG_BREAK();                                                                                 \
            }                                                                                                        \
        }
    #define RAZIX_ASSERT(x, ...)                                                                                \
        {                                                                                                       \
            if (!(x)) {                                                                                         \
                RAZIX_ERROR("Assertions Failed: {0} at Line {1} in File {2}", __VA_ARGS__, __LINE__, __FILE__); \
                RAZIX_DEBUG_BREAK();                                                                            \
            }                                                                                                   \
        }
    // Generic conditioned Assertions
    #define RAZIX_ASSERT_NO_MESSAGE(condition)                                                     \
        {                                                                                          \
            if (!(condition)) {                                                                    \
                RAZIX_CORE_ERROR("Assertion Failed: at Line {0} in File {1}", __LINE__, __FILE__); \
                RAZIX_DEBUG_BREAK();                                                               \
            }                                                                                      \
        }

    #define RAZIX_ASSERT_MESSAGE(condition, ...)                                                                     \
        {                                                                                                            \
            if (!(condition)) {                                                                                      \
                RAZIX_CORE_ERROR("Assertions Failed: {0} at Line {1} in File {2}", __VA_ARGS__, __LINE__, __FILE__); \
                RAZIX_DEBUG_BREAK();                                                                                 \
            }                                                                                                        \
        }
#else
    #define RAZIX_CORE_ASSERT(x, ...)            x
    #define RAZIX_ASSERT(x, ...)                 x
    #define RAZIX_ASSERT_NO_MESSAGE(condition)   condition
    #define RAZIX_ASSERT_MESSAGE(condition, ...) condition
#endif

// Max number of objects in a scene
#define MAX_OBJECTS 2048

// Stringize
#define STRINGIZE2(s)         #s
#define RAZIX_STRINGIZE(s)    STRINGIZE2(s)
#define RAZIX_ENGINE_ROOT_DIR STRINGIZE(RAZIX_ROOT_DIR)

// Concatenate two preprocessor tokens, indirectly to allow for use of macros like __LINE__
#define RZ_CONCATENATE(a, b)  CONCATENATE2(a, b)
#define RZ_CONCATENATE2(a, b) a##b

// pragma messages with more information
//#define TODO(x)         message( __FILE__ "(" CORE_TO_STRING(__LINE__) ") : TODO: " x )
#define RZ_TODO(x) message("TODO: " x)

// Function Bind macro
#define RAZIX_BIND_CB_EVENT_FN(x) std::bind(&Razix::RZApplication::x, this, std::placeholders::_1)

#define CAST_TO_FG_DESC(t)  (Razix::Gfx::FrameGraph::t::Desc)
#define CAST_TO_FG_TEX_DESC (Razix::Gfx::FrameGraph::RZFrameGraphTexture::Desc)
#define CAST_TO_FG_BUF_DESC (Razix::Gfx::FrameGraph::RZFrameGraphBuffer::Desc)

// right bit shift (useful for converting integer based color to hex)
#define RZ_BIT_SHIFT(x) (1 << x)
#define RETURN_IF_BIT_NOT_SET(val, b) \
    if ((val & b) != b) return;
#define RETURN_IF_BIT_SET(val, b) \
    if ((val & b) == b) return;
#define CONTINUE_IF_BIT_NOT_SET(val, b) \
    if ((val & b) != b) continue;
#define CONTINUE_IF_BIT_SET(val, b) \
    if ((val & b) == b) continue;

#define IS_BIT_NOT_SET(val, b) (val & b) != b
#define IS_BIT_SET(val, b)     (val & b) == b

// Convert hex to character
#define HEX2CHR(m_hex) ((m_hex >= '0' && m_hex <= '9') ? (m_hex - '0') : ((m_hex >= 'A' && m_hex <= 'F') ? (10 + m_hex - 'A') : ((m_hex >= 'a' && m_hex <= 'f') ? (10 + m_hex - 'a') : 0)))

// Unimplemented Function Breaker
#define RAZIX_UNIMPLEMENTED_METHOD                                                         \
    {                                                                                      \
        RAZIX_CORE_ERROR("Unimplemented : {0} : {1} : {2}", __func__, __FILE__, __LINE__); \
        RAZIX_DEBUG_BREAK();                                                               \
    }

#define RAZIX_UNIMPLEMENTED_METHOD_MARK                                                                   \
    {                                                                                                     \
        RAZIX_CORE_ERROR("Manchidi...!!! Unimplemented : {0} : {1} : {2}", __func__, __FILE__, __LINE__); \
    }

#define RAZIX_DELETE_PUBLIC_CONSTRUCTOR(type_identifier) \
public:                                                  \
    type_identifier() = delete;

#define RAZIX_VIRTUAL_DESCTURCTOR(type_identifier) \
    ~type_identifier() = default;

// Make the Class/Struct Object Non-Copyable/Assignable
#define RAZIX_NONCOPYABLE_CLASS(type_identifier)                 \
    type_identifier(const type_identifier&)            = delete; \
    type_identifier& operator=(const type_identifier&) = delete;

#define RAZIX_IMMOVABLE_CLASS(type_identifier)                       \
    type_identifier(type_identifier&&) noexcept            = delete; \
    type_identifier& operator=(type_identifier&&) noexcept = delete;

#define RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(type_identifier) \
    RAZIX_NONCOPYABLE_CLASS(type_identifier)               \
    RAZIX_IMMOVABLE_CLASS(type_identifier)

// Make the Class/Struct Object Copyable/Assignable Explicit default declaration
#define RAZIX_DEFAULT_COPYABLE_CLASS(type_identifier)             \
    type_identifier(const type_identifier&)            = default; \
    type_identifier& operator=(const type_identifier&) = default;

#define RAZIX_DEFAULT_MOVABLE_CLASS(type_identifier)                  \
    type_identifier(type_identifier&&) noexcept            = default; \
    type_identifier& operator=(type_identifier&&) noexcept = default;

#define RAZIX_DEFAULT_COPYABLE_MOVABLE_CLASS(type_identifier) \
    RAZIX_DEFAULT_COPYABLE_CLASS(type_identifier)             \
    RAZIX_DEFAULT_MOVABLE_CLASS(type_identifier)

// Deprecation error macros
#ifdef _MSC_VER
    #define RAZIX_DEPRECATED(msg_str) __declspec(deprecated(msg_str))
#elif defined(__clang__)
    #define RAZIX_DEPRECATED(msg_str) __attribute__((deprecated(msg_str)))
#elif defined(__GNUC__)
    #if GCC_VERSION >= 40500
        #define RAZIX_DEPRECATED(msg_str) __attribute__((deprecated(msg_str)))
    #else
        #define RAZIX_DEPRECATED(msg_str) __attribute__((deprecated))
    #endif
#else
    #define RAZIX_DEPRECATED(msg_str)
#endif

// Mark something (type, identifier, etc) as deprecated
#define RAZIX_DEPRECATED_TYPE(identifier) __pragma(deprecated(identifier))

// NO discard values for functions
#define RAZIX_NO_DISCARD [[nodiscard]]
// Marks a variable as unused (to avoid a compile warning)
#define UNREFERENCED_VARIABLE(P) (void) (P)

// Functions Calling Conventions for Razix Engine depending on the OS and compiler configuration
// On Windows we use __cdecl by default however __stdcall might be necessary for interop API with Razix Engine and C#
// TODO: Use build system macros to choose the function calling convention, also check with compilers and versions before choosing the appropriate function calling convention
#define RAZIX_CALLEE
#define RAZIX_CALLER
#undef RAZIX_CALLEE
#ifdef RAZIX_CALLEE
    #define RAZIX_CALLING_CONVENTION __stdcall
#endif
#ifdef RAZIX_CALLER
    #define RAZIX_CALLING_CONVENTION __cdecl
#endif
#define RAZIX_CALL RAZIX_CALLING_CONVENTION

// Inline macros
#define RAZIX_INLINE       inline
#define RAZIX_FORCE_INLINE __forceinline

/**
 * We need ways to emulate pure virtual function verification
 * We use SFINAE idiom and type traits as the base concept to do this 
 * 
 * Core Concept : SFINAE failure trigger redirection
 * 
 * SFINAE also does Struct/Type verification in addition to functions exist checks
 * 
 * SFINAE using safe substitution failure we manually trigger based on condition to choose the test we want
 * 
 * Note: These check don't consider the function signature
 * 
 * Note: We can do return type checks specialization inside the functions instead 
 * of just returning{}
 */

// TODO: Add signature check when doing CHECK_TYPE_HAS_FUNCTION

/**
 * RAZIX_CHECK_TYPE_HAS_FUNCTION
 * 
 * Working: Given a function name, it will check whether it has that function in it or not
 * If it's successful it will select the first specialization and return a true_type 
 * if not the first one will fail due to SFINAE and select the second type and return false_type
 */

#define RAZIX_CHECK_TYPE_HAS_FUNCTION(T, funcName)                       \
    template<typename T>                                                 \
    class has_##funcName                                                 \
    {                                                                    \
    private:                                                             \
        template<typename C>                                             \
        static constexpr ::std::true_type test(decltype(&C::##funcName)) \
        {                                                                \
            return {};                                                   \
        }                                                                \
                                                                         \
        template<typename C>                                             \
        static constexpr ::std::false_type test(...)                     \
        {                                                                \
            return {};                                                   \
        }                                                                \
                                                                         \
    public:                                                              \
        static constexpr bool value = test<T>(0);                        \
    };                                                                   \
    template<typename T>                                                 \
    inline constexpr bool has_##funcName##_v = has_##funcName<T>::value;

#define RAZIX_TYPE_HAS_FUNCTION_V(T, funcName) \
    has_##funcName##_v<T>

/**
 * RAZIX_CHECK_TYPE_HAS_SUBTYPE
 * 
 * Working: Given a type and subtype, if the type has the subtype it will choose the second 
 * specialization and return true_type if not it will choose the first one as default and 
 * return false_type
 * 
 */

#define RAZIX_CHECK_TYPE_HAS_SUBTYPE(T, U)                               \
    template<typename T, typename = void>                                \
    struct has_##U : ::std::false_type                                   \
    {                                                                    \
    };                                                                   \
    template<typename T>                                                 \
    struct has_##U<T, ::std::void_t<typename T::##U>> : ::std::true_type \
    {                                                                    \
    };                                                                   \
    template<typename T>                                                 \
    inline constexpr bool has_##U##_v = has_##U<T>::value

#define RAZIX_TYPE_HAS_SUB_TYPE_V(T, U) \
    has_##U##_v<T>

/**
 * SFINAE_TYPE_ERASURE_CONCEPT_CHECK
 */

/**
 * SFINAE ENUM CLASS |/& OPERATOR CHECK
 */
#define RAZIX_ENUM_CHECK_FOR_BITWISE_OPS(E)                                     \
    template<typename T, bool = std::is_enum<T>::value>                         \
    struct E;                                                                   \
                                                                                \
    template<typename T>                                                        \
    struct E<T, true> : std::false_type                                         \
    {};                                                                         \
                                                                                \
    template<typename T, typename std::enable_if<E<T>::value>::type* = nullptr> \
    T operator|(T lhs, T rhs)                                                   \
    {                                                                           \
        using u_t = typename std::underlying_type<T>::type;                     \
        return static_cast<T>(static_cast<u_t>(lhs) | static_cast<u_t>(rhs));   \
    }

/**
 * Bitwise OR and AND for enum class type
 */
#define RAZIX_ENUM_CLASS_BITWISE_COMPATIBLE(E)                                      \
    E operator|(E a, E b)                                                           \
    {                                                                               \
        return static_cast<E>(static_cast<unsigned>(a) | static_cast<unsigned>(b)); \
    }                                                                               \
                                                                                    \
    E operator&(E a, E b)                                                           \
    {                                                                               \
        return static_cast<E>(static_cast<unsigned>(a) & static_cast<unsigned>(b)); \
    }

// Warning push/pop as per compiler convention
#define RAZIX_WARNING_PUSH()     __pragma(warning(push))
#define RAZIX_WARNING_POP()      __pragma(warning(pop))
#define RAZIX_WARNING_DISABLE(x) __pragma warning(disable \
                                                  : x)
#define RAZIX_ENUM_NAMES_ASSERT(arrayName, enumName) static_assert(sizeof(arrayName) / sizeof(const char*) == (u32) enumName::COUNT)

/**
 * Memory Related stuff & Alignment Macros
 */
#define RZ_ALIGN_ARB(n, a) (((size_t) (n) + ((size_t) (a) -1)) & ~(size_t) ((a) -1))    // 'a' needs to be a power of 2

#define RZ_ALIGN_64K(n) ((((size_t) (n)) + 0xffff) & ~0xffff)

#define RZ_ALIGN_4096(n) ALIGN_ARB(n, 4096)

#define RZ_ALIGN_512(n) ((((size_t) (n)) + 511) & ~511)
#define RZ_ALIGN_256(n) ((((size_t) (n)) + 255) & ~255)
#define RZ_ALIGN_128(n) ((((size_t) (n)) + 127) & ~127)
#define RZ_ALIGN_64(n)  ((((size_t) (n)) + 63) & ~63)
#define RZ_ALIGN_32(n)  ((((size_t) (n)) + 31) & ~31)
#define RZ_ALIGN_16(n)  ((((size_t) (n)) + 15) & ~15)
#define RZ_ALIGN_8(n)   ((((size_t) (n)) + 7) & ~7)
#define RZ_ALIGN_4(n)   ((((size_t) (n)) + 3) & ~3)
#define RZ_ALIGN_2(n)   ((((size_t) (n)) + 1) & ~1)

#define RZ_IS_ALIGNED_ARB(n, a) (((size_t) (n) & ((size_t) (a) -1)) == 0)    // 'a' needs to be a power of 2

#define RZ_IS_ALIGNED_512(n) (((size_t) (n) & 511) == 0)
#define RZ_IS_ALIGNED_256(n) (((size_t) (n) & 255) == 0)
#define RZ_IS_ALIGNED_128(n) (((size_t) (n) & 127) == 0)
#define RZ_IS_ALIGNED_64(n)  (((size_t) (n) & 63) == 0)
#define RZ_IS_ALIGNED_32(n)  (((size_t) (n) & 31) == 0)
#define RZ_IS_ALIGNED_16(n)  (((size_t) (n) & 15) == 0)
#define RZ_IS_ALIGNED_8(n)   (((size_t) (n) & 7) == 0)
#define RZ_IS_ALIGNED_4(n)   (((size_t) (n) & 3) == 0)
#define RZ_IS_ALIGNED_2(n)   (((size_t) (n) & 1) == 0)

#define RZ_ALIGN_DOWN_ARB(n, a) ((size_t) (n) & ~(size_t) ((a) -1))    // 'a' needs to be a power of 2

#define RZ_ALIGN_DOWN_512(n) (size_t(n) & ~511)
#define RZ_ALIGN_DOWN_256(n) (size_t(n) & ~255)
#define RZ_ALIGN_DOWN_128(n) (size_t(n) & ~127)
#define RZ_ALIGN_DOWN_64(n)  (size_t(n) & ~63)
#define RZ_ALIGN_DOWN_32(n)  (size_t(n) & ~31)
#define RZ_ALIGN_DOWN_16(n)  (size_t(n) & ~15)
#define RZ_ALIGN_DOWN_8(n)   (size_t(n) & ~7)
#define RZ_ALIGN_DOWN_4(n)   (size_t(n) & ~3)
#define RZ_ALIGN_DOWN_2(n)   (size_t(n) & ~1)

// Align a struct as specified
#define RZ_ALIGN_TO(a) __declspec(align(a))
#define RZ_ALIGN_AS(a) alignas(a)

#define Gib(x) x * (1 << 30)
#define Mib(x) x * (1 << 20)
#define Kib(x) x * (1 << 10)

#define in_Gib(x) (x / (1 << 30))
#define in_Mib(x) (x / (1 << 20))
#define in_Kib(x) (x / 1024)

// Operator overload for quick expression without using macros
constexpr size_t operator""_Gib(unsigned long long int x)
{
    return x * 1 << 30;
}

constexpr size_t operator""_Mib(unsigned long long int x)
{
    return x * 1 << 20;
}

constexpr size_t operator""_Kib(unsigned long long int x)
{
    return x * 1 << 10;
}

constexpr float operator""_inGib(unsigned long long int x)
{
    return (float) x / (1 << 30);
}

constexpr float operator""_inMib(unsigned long long int x)
{
    return (float) x / (1 << 20);
}

constexpr float operator""_inKib(unsigned long long int x)
{
    return (float) x / (1 << 10);
}

// A macro to call a member function pointer
#define RAZIX_CALL_MEMBER_FUNC(object, member_func) ((object)->*(member_func))

// Macro to define DestroyResource for IRZResource clean up in a common way in the entire engine
#define RAZIX_CLEANUP_RESOURCE            virtual void DestroyResource() override;
#define RAZIX_CLEANUP_RESOURCE_IMPL(type) void type::DestroyResource()
#define RAZIX_CLEANUP_RESOURCE_IMPL_BEGIN(type) \
    void type::DestroyResource()                \
    {
#define RAZIX_CLEANUP_RESOURCE_IMPL_END }

// TODO: Add Safe memory delete and unloading macros
/****************************************************************************************************
 *                                         Graphics Settings                                        *
 ****************************************************************************************************/

/* Triple buffering is enabled by default */
#define RAZIX_ENABLE_TRIPLE_BUFFERING
/* The total number of images that the swapchain can render/present to, by default we use triple buffering, defaults to d32 buffering if disabled */
#ifdef RAZIX_ENABLE_TRIPLE_BUFFERING
    /* Frames in FLight defines the number of frames that will be rendered to while another frame is being presented (used for triple buffering)*/
    #define RAZIX_MAX_FRAMES_IN_FLIGHT  2
    #define RAZIX_MAX_SWAP_IMAGES_COUNT 3
    #define RAZIX_MAX_FRAMES            RAZIX_MAX_SWAP_IMAGES_COUNT
#elif
    #define RAZIX_MAX_SWAP_IMAGES_COUNT 2
#endif

/* Whether or not to use VMA as memory backend */
#define RAZIX_USE_VMA 1

/* Size of indices in Razix Engine, change here for global configuration */
#define RAZIX_INDICES_SIZE         sizeof(u32)    // we use 32-bit indices for now
#define RAZIX_INDICES_FORMAT       R32_UINT
#define RAZIX_INDICES_FORMAT_VK    VK_INDEX_TYPE_UINT32
#define RAZIX_INDICES_FORMAT_D3D12 DXGI_FORMAT_R32_UINT
#define RAZIX_INDICES_FORMAT_AGC   sce::Agc::IndexSize::k32

/****************************************************************************************************
 *                                         Vendor Settings                                          * 
 ****************************************************************************************************/

#define RAZIX_DEFINE_SAVE_LOAD   \
    template<class Archive>      \
    void load(Archive& archive); \
                                 \
    template<class Archive>      \
    void save(Archive& archive) const;

#define RAZIX_SEREALIZE     \
    template<class Archive> \
    void serialize(Archive& archive);

/****************************************************************************************************
 *                                                  Misc                                            * 
 ****************************************************************************************************/
#define RAZIX_MSG_BUFFER_SIZE 256