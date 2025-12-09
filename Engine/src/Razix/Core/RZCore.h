#pragma once

/****************************************************************************************************
 *                                  Settings based on OS/Compiler                                   *
 ****************************************************************************************************/
#if defined(_MSC_VER)
    #define RAZIX_COMPILER_MSVC
#elif defined(__clang__)
    #define RAZIX_COMPILER_CLANG
#elif defined(__GNUC__)
    #define RAZIX_COMPILER_GCC
#else
    #error "Unsupported compiler"
#endif

#ifdef __APPLE__
    #include <TargetConditionals.h>
#endif

// Architecture Detection
#if defined(__x86_64__) || defined(_M_X64)
    #define RAZIX_ARCHITECTURE_X64
#elif defined(__aarch64__) || defined(_M_ARM64) || defined(TARGET_CPU_ARM64) || defined(__arm64__)
    #define RAZIX_ARCHITECTURE_ARM64
#elif defined(__i386__) || defined(_M_IX86)
    #define RAZIX_ARCHITECTURE_X86
#elif defined(__arm__) || defined(_M_ARM)
    #define RAZIX_ARCHITECTURE_ARM
#else
    #error "Unsupported architecture"
#endif

// Platform + Architecture combos (for convenience)
#if defined(RAZIX_PLATFORM_WINDOWS)
    #if defined(RAZIX_ARCHITECTURE_X64)
        #define RAZIX_PLATFORM_WINDOWS_X64
    #elif defined(RAZIX_ARCHITECTURE_ARM64)
        #define RAZIX_PLATFORM_WINDOWS_ARM64
    #elif defined(RAZIX_ARCHITECTURE_X86)
        #define RAZIX_PLATFORM_WINDOWS_X86
    #elif defined(RAZIX_ARCHITECTURE_ARM)
        #define RAZIX_PLATFORM_WINDOWS_ARM
    #endif
#elif defined(RAZIX_PLATFORM_LINUX)
    #if defined(RAZIX_ARCHITECTURE_X64)
        #define RAZIX_PLATFORM_LINUX_X64
    #elif defined(RAZIX_ARCHITECTURE_ARM64)
        #define RAZIX_PLATFORM_LINUX_ARM64
    #elif defined(RAZIX_ARCHITECTURE_X86)
        #define RAZIX_PLATFORM_LINUX_X86
    #elif defined(RAZIX_ARCHITECTURE_ARM)
        #define RAZIX_PLATFORM_LINUX_ARM
    #endif
#elif defined(RAZIX_PLATFORM_MACOS)
    #if defined(RAZIX_ARCHITECTURE_X64)
        #define RAZIX_PLATFORM_MACOS_X64
    #elif defined(RAZIX_ARCHITECTURE_ARM64)
        #define RAZIX_APPLE_SILICON
        #define RAZIX_PLATFORM_MACOS_ARM64
    #endif
#elif defined(RAZIX_PLATFORM_FREEBSD)
    #if defined(RAZIX_ARCHITECTURE_X64)
        #define RAZIX_PLATFORM_FREEBSD_X64
    #elif defined(RAZIX_ARCHITECTURE_ARM64)
        #define RAZIX_PLATFORM_FREEBSD_ARM64
    #endif
#endif

#if defined(_MSC_VER)
    #define RAZIX_LITTLE_ENDIAN 1

#elif defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define RAZIX_LITTLE_ENDIAN 1
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define RAZIX_BIG_ENDIAN 1
    #else
        #error "Unknown endianness"
    #endif

#elif defined(__ARMEB__) || defined(__BIG_ENDIAN__)
    #define RAZIX_BIG_ENDIAN 1
#elif defined(__ARMEL__) || defined(__LITTLE_ENDIAN__)
    #define RAZIX_LITTLE_ENDIAN 1

#elif defined(RAZIX_ARCHITECTURE_X64) || defined(RAZIX_ARCHITECTURE_X86)
    #define RAZIX_LITTLE_ENDIAN 1
#elif defined(RAZIX_ARCHITECTURE_ARM64)
    #define RAZIX_LITTLE_ENDIAN 1
#elif defined(RAZIX_ARCHITECTURE_ARM)
    #if defined(__ARMEB__)
        #define RAZIX_BIG_ENDIAN 1
    #else
        #define RAZIX_LITTLE_ENDIAN 1
    #endif

#else
    #error "Unable to determine endianness for this platform"
#endif

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
#elif defined RAZIX_PLATFORM_MACOS
    #define MEM_DEF_ALIGNMENT_16 16
    #define RAZIX_MEM_ALIGN_16   alignas(MEM_DEF_ALIGNMENT_16)

    #ifdef RAZIX_BUILD_DLL
        #define RAZIX_API __attribute__((visibility("default")))
    #else
        #define RAZIX_API
    #endif

    #define RAZIX_DEBUG_BREAK() __builtin_debugtrap()    // clang

    #define RAZIX_HIDDEN __attribute__((visibility("hidden")))
#else
    #define MEM_DEF_ALIGNMENT_16 16

    #define RAZIX_API    __attribute__((visibility("default")))
    #define RAZIX_HIDDEN __attribute__((visibility("hidden")))
    #define RAZIX_DEBUG_BREAK()
    #define RAZIX_MEM_ALIGN_16 alignas(MEM_DEF_ALIGNMENT_16)

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
    #define RAZIX_ASSERT(x, ...)                                                                                     \
        {                                                                                                            \
            if (!(x)) {                                                                                              \
                RAZIX_CORE_ERROR("Assertions Failed: {0} at Line {1} in File {2}", __VA_ARGS__, __LINE__, __FILE__); \
                RAZIX_DEBUG_BREAK();                                                                                 \
            }                                                                                                        \
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
    #define RAZIX_CORE_ASSERT(x, ...)
    #define RAZIX_ASSERT(x, ...)
    #define RAZIX_ASSERT_NO_MESSAGE(condition)
    #define RAZIX_ASSERT_MESSAGE(condition, ...)
#endif

#define RAZIX_FIXME_STUB_COMPILE(...) static_assert(false, "[RAZIX] FIXME_STUB: This function is not implemented!"##__VA_ARGS__)
#define RAZIX_FIXME_STUB(...)                                                                              \
    {                                                                                                      \
        RAZIX_CORE_ERROR("[RAZIX] FIXME_STUB: {0} at Line {1} in File {2}", __func__, __LINE__, __FILE__); \
        RAZIX_DEBUG_BREAK();                                                                               \
    }

#define RAZIX_TODO_STUB_COMPILE(...) static_assert(false, "[RAZIX] TODO_STUB: This function is not implemented!")
#define RAZIX_TODO_STUB(...)                                                                              \
    {                                                                                                     \
        RAZIX_CORE_ERROR("[RAZIX] TODO_STUB: {0} at Line {1} in File {2}", __func__, __LINE__, __FILE__); \
        RAZIX_DEBUG_BREAK();                                                                              \
    }

// Max number of objects in a scene
#define MAX_OBJECTS 2048

// Stringize
#define STRINGIZE2(s)         #s
#define RAZIX_STRINGIZE(s)    STRINGIZE2(s)
#define RAZIX_ENGINE_ROOT_DIR RAZIX_STRINGIZE(RAZIX_ROOT_DIR)

// Concatenate two preprocessor tokens, indirectly to allow for use of macros like __LINE__
#define RZ_CONCATENATE(a, b)  CONCATENATE2(a, b)
#define RZ_CONCATENATE2(a, b) a##b

// pragma messages with more information
//#define TODO(x)         message( __FILE__ "(" CORE_TO_STRING(__LINE__) ") : TODO: " x )
#define RZ_TODO(x) message("TODO: " x)

// Function Bind macro
#define RAZIX_BIND_CB_EVENT_FN(x) std::bind(&Razix::RZApplication::x, this, std::placeholders::_1)

#define CAST_TO_FG_DESC(t)  (Razix::Gfx::t::Desc)
#define CAST_TO_FG_TEX_DESC (Razix::Gfx::RZFrameGraphTexture::Desc)
#define CAST_TO_FG_BUF_DESC (Razix::Gfx::RZFrameGraphBuffer::Desc)

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

#define BIT_INSERT(bits, value, offset, size) \
    ((bits & ~(((1u << (size)) - 1u) << (offset))) | ((value & ((1u << (size)) - 1u)) << (offset)))

#define BIT_EXTRACT(bits, offset, size) \
    ((bits >> (offset)) & ((1u << (size)) - 1u))

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
#define RAZIX_UNREF_VAR(P) (void) (P)

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
#define RAZIX_INLINE inline
#if defined(RAZIX_COMPILER_MSVC)
    #define RAZIX_FORCE_INLINE __forceinline
#elif defined(RAZIX_COMPILER_CLANG) || defined(RAZIX_COMPILER_GCC)
    #define RAZIX_FORCE_INLINE __attribute__((always_inline))
#else
    #define RAZIX_FORCE_INLINE inline
#endif

#define RAZIX_LIKELY   [[likely]]
#define RAZIX_UNLIKELY [[unlikely]]

#define RAZIX_UNUSED(x) (void) (x)

// Warning push/pop as per compiler convention
// MSVC-specific: __pragma(warning(push)) etc.
#ifdef _MSC_VER
    #define RAZIX_WARNING_PUSH()     __pragma(warning(push))
    #define RAZIX_WARNING_POP()      __pragma(warning(pop))
    #define RAZIX_WARNING_DISABLE(x) __pragma(warning(disable : x))

//// GCC/Clang-specific: using pragmas for warning control
//#elif defined(__GNUC__) || defined(__clang__)
//    #define RAZIX_WARNING_PUSH()     _Pragma("GCC diagnostic push")
//    #define RAZIX_WARNING_POP()      _Pragma("GCC diagnostic pop")
//    #define RAZIX_WARNING_DISABLE(x) _Pragma("GCC diagnostic ignored \"-W" #x "\"")
//
//// Default fallback for other compilers, no warning control.
#else
    #define RAZIX_WARNING_PUSH()
    #define RAZIX_WARNING_POP()
    #define RAZIX_WARNING_DISABLE(x)
#endif

#ifdef __cplusplus

    #define RAZIX_DELETE_PUBLIC_CONSTRUCTOR(type_identifier) \
    public:                                                  \
        type_identifier() = delete;

    #define RAZIX_VIRTUAL_DESCTURCTOR(type_identifier) \
        virtual ~type_identifier() = default;

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

    #define RAZIX_PRIVATE_INSTANTIABLE_CLASS(type_identifier) \
    private:                                                  \
        type_identifier()                                     \
        {                                                     \
        }                                                     \
        RAZIX_NONCOPYABLE_IMMOVABLE_CLASS(type_identifier)

    // Enum names array size checker
    #define RAZIX_ENUM_NAMES_ASSERT(arrayName, enumName) static_assert(sizeof(arrayName) / sizeof(const char*) == (u32) enumName::COUNT)

#endif    // __cplusplus

/**
 * Memory Related stuff & Alignment Macros
 */
#define RZ_ALIGN_ARB(n, a) (((size_t) (n) + ((size_t) (a) - 1)) & ~(size_t) ((a) - 1))    // 'a' needs to be a power of 2

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

#define RZ_IS_ALIGNED_ARB(n, a) (((size_t) (n) & ((size_t) (a) - 1)) == 0)    // 'a' needs to be a power of 2

#define RZ_IS_ALIGNED_512(n) (((size_t) (n) & 511) == 0)
#define RZ_IS_ALIGNED_256(n) (((size_t) (n) & 255) == 0)
#define RZ_IS_ALIGNED_128(n) (((size_t) (n) & 127) == 0)
#define RZ_IS_ALIGNED_64(n)  (((size_t) (n) & 63) == 0)
#define RZ_IS_ALIGNED_32(n)  (((size_t) (n) & 31) == 0)
#define RZ_IS_ALIGNED_16(n)  (((size_t) (n) & 15) == 0)
#define RZ_IS_ALIGNED_8(n)   (((size_t) (n) & 7) == 0)
#define RZ_IS_ALIGNED_4(n)   (((size_t) (n) & 3) == 0)
#define RZ_IS_ALIGNED_2(n)   (((size_t) (n) & 1) == 0)

#define RZ_ALIGN_DOWN_ARB(n, a) ((size_t) (n) & ~(size_t) ((a) - 1))    // 'a' needs to be a power of 2

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

#define RAZIX_CACHE_LINE_SIZE  64    // typical size of a cache line
#define RAZIX_16B_ALIGN        16
#define RAZIX_32B_ALIGN        32
#define RAZIX_CACHE_LINE_ALIGN RAZIX_CACHE_LINE_SIZE
#define RAZIX_128B_ALIGN       128

// C99 valid alignas for structs
#define RAZIX_ALIGN_AS(x) __attribute__((aligned(x)))

#ifdef __cplusplus

    #ifdef RAZIX_PLATFORM_UNIX
        #include <stddef.h>    // for size_t
    #endif

static constexpr size_t operator""_Gib(unsigned long long int x)
{
    return Gib(x);
}

static constexpr size_t operator""_Mib(unsigned long long int x)
{
    return Mib(x);
}

static constexpr size_t operator""_Kib(unsigned long long int x)
{
    return Kib(x);
}

static constexpr float operator""_inGib(unsigned long long int x)
{
    return (float) x / (1 << 30);
}

static constexpr float operator""_inMib(unsigned long long int x)
{
    return (float) x / (1 << 20);
}

static constexpr float operator""_inKib(unsigned long long int x)
{
    return (float) x / (1 << 10);
}

#endif    //   __cplusplus

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

#define RAZIX_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Busy-wait hint: PAUSE/YIELD instruction, stays scheduled */
#if defined(RAZIX_PLATFORM_WINDOWS)
    // [Source]: https://learn.microsoft.com/en-us/windows/win32/api/winnt/nf-winnt-yieldprocessor
    #define RAZIX_BUSY_WAIT() YieldProcessor()
#elif defined(RAZIX_PLATFORM_UNIX)
    #if defined(__x86_64__) || defined(__i386__)
        #define RAZIX_BUSY_WAIT() _mm_pause()
    #elif defined(__aarch64__) || defined(__arm__)
        #define RAZIX_BUSY_WAIT() __asm__ __volatile__("yield" ::: "memory")
    #else
        #define RAZIX_BUSY_WAIT() ((void) 0)
    #endif
#else
    #error "Undefined platform"
#endif

/* OS-level yield: give timeslice to another runnable thread */
#if defined(RAZIX_PLATFORM_WINDOWS)
    #define RAZIX_YIELD() SwitchToThread()
#elif defined(RAZIX_PLATFORM_UNIX)
    #define RAZIX_YIELD() sched_yield()
#else
    #error Undefined platform
#endif

#define RAZIX_THREAD_DEFAULT_STACK_SIZE (2 * 1024 * 1024)

// For C99 compatibility, we cannot use thread_local since its C++11 keyword
#ifdef RAZIX_COMPILER_MSVC
    #define RAZIX_THREAD_LOCAL_STORAGE __declspec(thread)
#elif defined RAZIX_COMPILER_GCC || defined RAZIX_COMPILER_CLANG
    #define RAZIX_THREAD_LOCAL_STORAGE __thread
#else
    #error "No thread-local storage keyword available for this compiler"
#endif

#define RAZIX_TLS RAZIX_THREAD_LOCAL_STORAGE
