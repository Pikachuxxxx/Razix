#pragma once

/****************************************************************************************************
 *                                      Settings based on OS                                        *
 ****************************************************************************************************/
// Settings for Windows OS
#ifdef RAZIX_PLATFORM_WINDOWS
    
    // Disable any MSVC specific warnings
    #pragma warning(disable : 4251)
       
    // 16-bit Memory alignment for the Windows OS
    #define MEM_ALIGNMENT 16
    #define MEM_ALIGN __declspec(align(MEM_ALIGNMENT)))

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
    #define RAZXI_API __attribute__((visibility("default")))
    #define RAZIX_HIDDEN __attribute__((visibility("hidden")))
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
#ifdef RAZIX_ENABLE_ASSERTS // Only available in Debug Builds (predefined by the build system only in Debug mode)
    // Predefined conditioned Assert
    // TODO: Use the generic condition assert macro and pass this specific condition
    #define RAZIX_CORE_ASSERT(x, ...) {if(!(x)) { RAZIX_CORE_ERROR("Assertions Failed: {0} at Line {1} in File {2}", __VA_ARGS__, __LINE__, __FILE__); RAZIX_DEBUG_BREAK(); }}
    #define RAZIX_ASSERT(x, ...) {if(!(x)) { RAZIX_ERROR("Assertions Failed: {0} at Line {1} in File {2}", __VA_ARGS__, __LINE__, __FILE__); RAZIX_DEBUG_BREAK(); }}
    // Generic conditioned Assertions
    #define RAZIX_ASSERT_NO_MESSAGE(condition)                                                      \
    {                                                                                               \
        if(!(condition))                                                                            \
        {                                                                                           \
            RAZIX_CORE_ERROR("Assertion Failed: at Line {0} in File {1}", __LINE__, __FILE__);      \
            RAZIX_DEBUG_BREAK();                                                                      \
        }                                                                                           \
    }

    #define RAZIX_ASSERT_MESSAGE(condition, ...)                                                                    \
    {                                                                                                               \
        if(!(condition))                                                                                            \
        {                                                                                                           \
            RAZIX_CORE_ERROR("Assertions Failed: {0} at Line {1} in File {2}", __VA_ARGS__, __LINE__, __FILE__);    \
            RAZIX_DEBUG_BREAK();                                                                                      \
        }                                                                                                           \
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
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define ROOT_DIR STRINGIZE(RAZIX_ROOT_DIR)

// Function Bind macro
#define RAZIX_BIND_CB_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

// right bit shift (useful for converting integer based color to hex)
#define BIT(x) (1 << x) 

// Convert hex to character
#define HEX2CHR(m_hex) \
        ((m_hex >= '0' && m_hex <= '9') ? (m_hex - '0') : ((m_hex >= 'A' && m_hex <= 'F') ? (10 + m_hex - 'A') : ((m_hex >= 'a' && m_hex <= 'f') ? (10 + m_hex - 'a') : 0)))


// Unimplemented Function/Class indicator
#define UNIMPLEMENTED                                                               \
{                                                                                   \
    RAZIX_CORE_ERROR("Unimplemented : {0} : {1}", __FILE__, __LINE__);              \
    RAZIX_DEBUG_BREAK();                                                            \
}

// Make the Class/Struct Object Non-Copyable
#define NONCOPYABLE(type_identifier)                                \
    type_identifier(const type_identifier&) = delete;               \
    type_identifier& operator=(const type_identifier&) = delete;


// TODO: Add Safe memory delete and unloading macros

/****************************************************************************************************/