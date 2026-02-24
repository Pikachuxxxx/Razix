#pragma once

#include "Razix/Core/RZCore.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- Logging Wrappers ---
RAZIX_API void rz_asm_log_core_trace(const char* msg);
RAZIX_API void rz_asm_log_core_info(const char* msg);
RAZIX_API void rz_asm_log_core_warn(const char* msg);
RAZIX_API void rz_asm_log_core_error(const char* msg);

RAZIX_API void rz_asm_log_app_trace(const char* msg);
RAZIX_API void rz_asm_log_app_info(const char* msg);
RAZIX_API void rz_asm_log_app_warn(const char* msg);
RAZIX_API void rz_asm_log_app_error(const char* msg);

RAZIX_API void rz_asm_log_core_trace_fmt(const char* fmt, ...);
RAZIX_API void rz_asm_log_core_info_fmt(const char* fmt, ...);
RAZIX_API void rz_asm_log_core_warn_fmt(const char* fmt, ...);
RAZIX_API void rz_asm_log_core_error_fmt(const char* fmt, ...);

// --- Profiling Wrappers ---
RAZIX_API void rz_asm_profile_scope_begin(const char* name);
RAZIX_API void rz_asm_profile_scope_begin_c(const char* name, uint32_t color);
RAZIX_API void rz_asm_profile_scope_end(void);

#ifdef __cplusplus
}
#endif
