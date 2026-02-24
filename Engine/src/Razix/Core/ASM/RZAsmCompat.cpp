#include "rzxpch.h"
#include "RZAsmCompat.h"
#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/Profiling/RZProfiling.h"
#include <stdarg.h>
#include <stdio.h>

extern "C" {
void rz_asm_log_core_trace(const char* msg) { RAZIX_CORE_TRACE("%s", msg); }
void rz_asm_log_core_info(const char* msg)  { RAZIX_CORE_INFO("%s", msg);  }
void rz_asm_log_core_warn(const char* msg)  { RAZIX_CORE_WARN("%s", msg);  }
void rz_asm_log_core_error(const char* msg) { RAZIX_CORE_ERROR("%s", msg); }

void rz_asm_log_app_trace(const char* msg) { RAZIX_TRACE("%s", msg); }
void rz_asm_log_app_info(const char* msg)  { RAZIX_INFO("%s", msg);  }
void rz_asm_log_app_warn(const char* msg)  { RAZIX_WARN("%s", msg);  }
void rz_asm_log_app_error(const char* msg) { RAZIX_ERROR("%s", msg); }

void rz_asm_log_core_trace_fmt(const char* fmt, ...) { char buffer[1024]; va_list args; va_start(args, fmt); vsnprintf(buffer, sizeof(buffer), fmt, args); va_end(args); RAZIX_CORE_TRACE("%s", buffer); }
void rz_asm_log_core_info_fmt(const char* fmt, ...) { char buffer[1024]; va_list args; va_start(args, fmt); vsnprintf(buffer, sizeof(buffer), fmt, args); va_end(args); RAZIX_CORE_INFO("%s", buffer); }
void rz_asm_log_core_warn_fmt(const char* fmt, ...) { char buffer[1024]; va_list args; va_start(args, fmt); vsnprintf(buffer, sizeof(buffer), fmt, args); va_end(args); RAZIX_CORE_WARN("%s", buffer); }
void rz_asm_log_core_error_fmt(const char* fmt, ...) { char buffer[1024]; va_list args; va_start(args, fmt); vsnprintf(buffer, sizeof(buffer), fmt, args); va_end(args); RAZIX_CORE_ERROR("%s", buffer); }

void rz_asm_profile_scope_begin(const char* name) { RAZIX_PROFILE_SCOPE_BEGIN(name); }
void rz_asm_profile_scope_begin_c(const char* name, uint32_t color) { RAZIX_PROFILE_SCOPEC_BEGIN(name, color); }
void rz_asm_profile_scope_end(void) { RAZIX_PROFILE_SCOPE_END(); }
}
