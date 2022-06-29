#pragma once

/****************************************************************************************************
 *                                      Settings based on OS                                        *
 ****************************************************************************************************/
// TODO: Use inline macros such as RAZIX_INLINE to enable special optimizations of inline function based on OS
// Settings for Windows OS
#ifdef RAZIX_PLATFORM_WINDOWS

    // Disable any MSVC specific warnings
    #pragma warning(disable : 4251)

    // 16-bit Memory alignment for the Windows OS
    #define MEM_ALIGNMENT 16
    #define RAZIX_MEM_ALIGN __declspec(align(MEM_ALIGNMENT)))

    // Symbols Export settings for Engine
    #ifdef RAZIX_BUILD_DLL
        #define RAZIX_API __declspec(dllexport)
    #else
        #define RAZIX_API __declspec(dllimport)
    #endif

    // Debug functions for breaking the debugger or interrupting the code
    #define RAZIX_DEBUG_BREAK() __debugbreak()

    // IDK what the fuck this is!
    #define RAZIX_HIDDEN

#else
    #define RAZXI_API           __attribute__((visibility("default")))
    #define RAZIX_HIDDEN        __attribute__((visibility("hidden")))
    #define RAZIX_DEBUG_BREAK() raise(SIGTRAP);

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
#ifdef RAZIX_ENABLE_ASSERTS    // Only available in Debug Builds (predefined by the build system only in Debug mode)
    // Predefined conditioned Assert
    // TODO: Use the generic condition assert macro and pass this specific condition
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
    #define RAZIX_CORE_ASSERT(x, ...)
    #define RAZIX_ASSERT(x, ...)
    #define RAZIX_ASSERT_NO_MESSAGE(condition)
    #define RAZIX_ASSERT_MESSAGE(condition, ...)
#endif

// Max number of objects in a scene
#define MAX_OBJECTS 2048

// Stringize
#define STRINGIZE2(s)         #s
#define RAZIX_STRINGIZE(s)    STRINGIZE2(s)
#define RAZIX_ENGINE_ROOT_DIR STRINGIZE(RAZIX_ROOT_DIR)

// Function Bind macro
#define RAZIX_BIND_CB_EVENT_FN(x) std::bind(&RZApplication::x, this, std::placeholders::_1)

// right bit shift (useful for converting integer based color to hex)
#define RZ_BIT_SHIFT(x) (1 << x)

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

// Make the Class/Struct Object Non-Copyable/Assignable
#define RAZIX_NONCOPYABLE_CLASS(type_identifier)      \
    type_identifier(const type_identifier&) = delete; \
    type_identifier& operator=(const type_identifier&) = delete;

// Deprecation error macros
#ifdef _MSC_VER
    #define RAZIX_DEPRECATED(msg_str) __declspec(deprecated("This symbol is deprecated by Razix Engine. Details: " msg_str))
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

#ifdef RAZIX_DISTRIBUTION
    #define RAZIX_INLINE RAZIX_FORCE_INLINE
#endif

// Release for API convention consistency
//#define RAZIX_RELEASE(x)            \
//    {                               \
//        if (x) { (x)->Release(); }; \
//    }
//#define RAZIX_SAFE_RELEASE(x)       \
//    {                               \
//        if (x) { (x)->Release(); }; \
//        x = nullptr;                \
//    }

// Warning push/pop as per compiler convention
#define RAZIX_WARNING_PUSH() __pragma(warning(push))
#define RAZIX_WARNING_POP()  __pragma(warning(pop))
//#define RAZIX_WARNING_DISABLE( x )  #pragma warning( disable : x)

// TODO: Add Safe memory delete and unloading macros
/****************************************************************************************************
 *                                         Platforms Settings                                       *
 ****************************************************************************************************/
