#include "Razix/Core/Memory/RZMemoryFunctions.h"

#ifdef RAZIX_PLATFORM_WINDOWS
    #include <corecrt_malloc.h>
    #include <windows.h>
#elif RAZIX_PLATFORM_UNIX
    #include <memory.h>
    #include <pthread.h>
    #include <stdlib.h>
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>    // for memset, memcpy
#include <time.h>

#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING
    #include "Razix/Core/std/sync.h"
#endif

#define RAZIX_MEMORY_POISON 0xCD

#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING

//--------------------------------------
// Tracking data structures
//--------------------------------------

#define RZ_MEMTRACK_NAME_MAX 64
#define RZ_MEMTRACK_FILE_MAX 96

typedef struct RZMemAllocationRecord
{
    u32      id;
    bool     active;
    void*    raw_ptr;
    void*    user_ptr;
    size_t   size;
    size_t   alignment;
    u32      magic_number;
    u32      thread_id;
    u64      timestamp;
    u32      line;
    char     name[RZ_MEMTRACK_NAME_MAX];
    char     file[RZ_MEMTRACK_FILE_MAX];
} RZMemAllocationRecord;

typedef struct RZMemTracker
{
    bool                   initialized;
    bool                   enabled;
    rz_critical_section    lock;
    u32                    next_allocation_idx;
    RZMemAllocationRecord* allocations;
    size_t                 allocation_capacity;
} RZMemTracker;

static RZMemTracker s_memtracker = {0};

//--------------------------------------
// Tracking helpers
//--------------------------------------

static inline void rz_memtracker_copy_text(char* dst, size_t dst_len, const char* src)
{
    if (!dst || dst_len == 0) return;
    if (!src) {
        dst[0] = '\0';
        return;
    }
    strncpy(dst, src, dst_len - 1);
    dst[dst_len - 1] = '\0';
}

// Opt-in diagnostics to help find mismatched frees/reallocs while tracking is on
#ifndef RZ_MEMTRACK_DIAGNOSTICS
    #define RZ_MEMTRACK_DIAGNOSTICS 1
#endif

static inline u64 rz_memtracker_timestamp(void)
{
    return (u64) time(NULL);
}

static inline u32 rz_memtracker_thread_id(void)
{
#ifdef RAZIX_PLATFORM_WINDOWS
    return (u32) GetCurrentThreadId();
#elif defined(RAZIX_PLATFORM_MACOS)
    return (u32) pthread_mach_thread_np(pthread_self());
#else
    return (u32) (uintptr_t) pthread_self();
#endif
}

static inline u32 rz_memtracker_hash_magic(const char* text, u32 seed)
{
    if (!text) return seed;
    u32 hash = seed ? seed : 2166136261u;
    while (*text) {
        hash ^= (u8) (*text++);
        hash *= 16777619u;
    }
    return hash;
}

static inline u32 rz_memtracker_compose_magic(const char* name, const char* file, u32 line, u32 seed)
{
    // Base disguise uses "Razix" letters to keep it recognizable while non-zero.
    u32 magic = seed ? seed : 0x525a4958u;    // 'R''Z''I''X'
    magic     = rz_memtracker_hash_magic(name, magic);
    magic     = rz_memtracker_hash_magic(file, magic);
    magic ^= (line + 1u) * 16777619u;    // ensure line contributes and avoid zero for line 0
    if (magic == 0) magic = 0x525a4958u ^ 0xA5A5A5A5u;
    return magic;
}

static void rz_memtracker_ensure_allocation_capacity(u32 idx)
{
    if (idx < s_memtracker.allocation_capacity) return;
    size_t new_cap = s_memtracker.allocation_capacity == 0 ? 128 : s_memtracker.allocation_capacity;
    while (new_cap <= idx) new_cap *= 2;
    RZMemAllocationRecord* new_buf = (RZMemAllocationRecord*) realloc(s_memtracker.allocations, new_cap * sizeof(RZMemAllocationRecord));
    if (!new_buf) return;
    memset(new_buf + s_memtracker.allocation_capacity, 0, (new_cap - s_memtracker.allocation_capacity) * sizeof(RZMemAllocationRecord));
    s_memtracker.allocations         = new_buf;
    s_memtracker.allocation_capacity = new_cap;
}

// Helper to grab the header word (if present) for diagnostics without failing hard
static inline u64 rz_memtracker_peek_header(void* user_ptr)
{
    if (!user_ptr) return 0;
    return *((const u64*) (((const u8*) user_ptr) - sizeof(u64)));
}

static void rz_memtracker_init(void)
{
    if (s_memtracker.initialized) return;
    s_memtracker.lock                 = rz_critical_section_create();
    s_memtracker.initialized          = true;
    s_memtracker.enabled              = false;
    s_memtracker.next_allocation_idx  = 1;
    s_memtracker.allocations          = NULL;
    s_memtracker.allocation_capacity  = 0;
}

static u64 rz_memtracker_record_allocation(void* raw_ptr, void* user_ptr, size_t size, size_t alignment, const char* name, const char* file, u32 line, u32 magic)
{
    if (!s_memtracker.enabled) return 0;
    rz_memtracker_init();

    rz_critical_section_lock(&s_memtracker.lock);

    const u32 alloc_idx = s_memtracker.next_allocation_idx++;
    rz_memtracker_ensure_allocation_capacity(alloc_idx);

    if (alloc_idx < s_memtracker.allocation_capacity) {
        RZMemAllocationRecord* rec = &s_memtracker.allocations[alloc_idx];
        memset(rec, 0, sizeof(RZMemAllocationRecord));
        rec->id            = alloc_idx;
        rec->active        = true;
        rec->raw_ptr       = raw_ptr;
        rec->user_ptr      = user_ptr;
        rec->size          = size;
        rec->alignment     = alignment;
        rec->magic_number  = rz_memtracker_compose_magic(name, file, line, magic);
        rec->thread_id     = rz_memtracker_thread_id();
        rec->timestamp     = rz_memtracker_timestamp();
        rec->line          = line;
        rz_memtracker_copy_text(rec->name, sizeof(rec->name), name);
        rz_memtracker_copy_text(rec->file, sizeof(rec->file), file);
        rz_critical_section_unlock(&s_memtracker.lock);
        return ((u64) alloc_idx) << 32;
    }

    rz_critical_section_unlock(&s_memtracker.lock);
    return 0;
}

static RZMemAllocationRecord* rz_memtracker_get_record(void* user_ptr)
{
    if (!s_memtracker.enabled || !user_ptr) return NULL;

    const u8* header_loc = ((u8*) user_ptr) - sizeof(u64);
    u64       header     = *((const u64*) header_loc);
    u32       alloc_idx  = (u32) (header >> 32);

    if (alloc_idx >= s_memtracker.allocation_capacity) return NULL;
    RZMemAllocationRecord* rec = &s_memtracker.allocations[alloc_idx];
    if (!rec->active || rec->user_ptr != user_ptr) return NULL;
    return rec;
}

static void* rz_memtracker_unwrap_pointer(void* user_ptr, const char* file, u32 line)
{
    if (!s_memtracker.enabled || !user_ptr) return user_ptr;

    void* raw_ptr = user_ptr;
    rz_critical_section_lock(&s_memtracker.lock);
    RZMemAllocationRecord* rec = rz_memtracker_get_record(user_ptr);
    if (rec && rec->active) {
        raw_ptr     = rec->raw_ptr ? rec->raw_ptr : user_ptr;
        rec->active = false;
    } else if (RZ_MEMTRACK_DIAGNOSTICS) {
        u64 header       = rz_memtracker_peek_header(user_ptr);
        const char* src  = file ? file : "<unknown>";
        const char* kind = rec ? "inactive" : "untracked";
        fprintf(stderr,
                "[Razix][Memory][diag] free on %s pointer %p header=0x%016llx (free site %s:%u)%s\n",
                kind,
                user_ptr,
                (unsigned long long) header,
                src,
                line,
                rec ? "" : "");
    }
    rz_critical_section_unlock(&s_memtracker.lock);
    return raw_ptr;
}

#endif    // RAZIX_ENABLE_MEM_ALLOC_TRACKING

static inline void* rz_platform_aligned_alloc(size_t size, size_t alignment)
{
#ifdef RAZIX_PLATFORM_WINDOWS
    return _aligned_malloc(size, alignment);
#elif RAZIX_PLATFORM_UNIX
    void* ptr = NULL;
    if (posix_memalign(&ptr, alignment, size) != 0) ptr = NULL;
    return ptr;
#else
    return malloc(size);
#endif
}

static inline void rz_platform_aligned_free(void* address)
{
#ifdef RAZIX_PLATFORM_WINDOWS
    _aligned_free(address);
#elif RAZIX_PLATFORM_UNIX
    free(address);
#else
    free(address);
#endif
}

//--------------------------------------
// Core allocation helpers
//--------------------------------------

static void* rz_mem_alloc_internal(size_t size, size_t alignment, const char* name, const char* file, u32 line, u32 magic, bool zeroMemory)
{
    if (alignment == 0) alignment = 16;

    bool tracking_active = false;
#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING
    tracking_active = s_memtracker.enabled;
#endif

    size_t total_size = size;
     if (tracking_active) {
        total_size += alignment + sizeof(u64);
    }

    void* raw_ptr = NULL;

#ifdef RAZIX_DEBUG
    raw_ptr = rz_debug_malloc(total_size, alignment, NULL, 0, NULL);
#else
    raw_ptr = rz_platform_aligned_alloc(total_size, alignment);
#endif

    if (!raw_ptr) return NULL;

    void* user_ptr = raw_ptr;

#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING
    if (tracking_active) {
        const char* rec_file = file ? file : "<unknown>";
        const char* rec_name = name ? name : "<unnamed>";
        u8* aligned_user     = (u8*) rz_align_ptr(((u8*) raw_ptr) + sizeof(u64), alignment);
        const u64 header     = rz_memtracker_record_allocation(raw_ptr, aligned_user, size, alignment, rec_name, rec_file, line, magic);
        *((u64*) (aligned_user - sizeof(u64))) = header;
        user_ptr = aligned_user;
    }
#endif

    if (zeroMemory && user_ptr)
        memset(user_ptr, 0, size);

    return user_ptr;
}

void* rz_malloc(size_t size, size_t alignment)
{
    return rz_mem_alloc_internal(size, alignment, "rz_malloc", "<unknown>", 0, 0, true);
}

void* rz_malloc_aligned(size_t size)
{
    // 16-byte makes it easy for AVX operations and SIMD so yeah we choose 16 for now
    return rz_malloc(size, 16);
}

void* rz_malloc_debug(size_t size, const char* filename, uint32_t lineNumber, const char* tag)
{
    u32 magic = 0;
#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING
    magic = rz_memtracker_hash_magic(tag ? tag : filename, (u32) lineNumber);
#endif
    return rz_mem_alloc_internal(size, 16, tag ? tag : "rz_malloc_debug", filename, lineNumber, magic, true);
}

void* rz_malloc_debug_aligned(size_t size, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag)
{
    u32 magic = 0;
#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING
    magic = rz_memtracker_hash_magic(tag ? tag : filename, (u32) lineNumber);
#endif
    return rz_mem_alloc_internal(size, alignment, tag ? tag : "rz_malloc_debug", filename, lineNumber, magic, true);
}

void* rz_mem_copy_to_heap(void* data, size_t size)
{
    uint8_t* heapData = (uint8_t*) rz_malloc(size, RAZIX_16B_ALIGN);
    if (heapData && data)
        memcpy(heapData, data, size);
    return heapData;
}

static void* rz_realloc_internal_with_meta(void* oldPtr, size_t oldSize, size_t newSize, size_t alignment, const char* file, uint32_t line, const char* tag)
{
    if (newSize == 0) {
        rz_free(oldPtr);
        return NULL;
    }

    if (!oldPtr) {
        return rz_mem_alloc_internal(newSize, alignment, tag ? tag : "rz_realloc", file, line, 0, false);
    }

#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING
    RZMemAllocationRecord* rec     = rz_memtracker_get_record(oldPtr);
    void*                  raw_ptr = rec ? rec->raw_ptr : oldPtr;
#else
    void* raw_ptr = oldPtr;
#endif

    size_t totalSize = newSize;
#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING
    bool tracking_active = s_memtracker.enabled;
    if (tracking_active) {
        if (alignment == 0) alignment = 16;
        totalSize += alignment + sizeof(u64);
    }
#endif

#ifdef RAZIX_PLATFORM_WINDOWS
    void* new_raw_ptr = _aligned_realloc(raw_ptr, totalSize, alignment);
    bool  need_copy   = false;
    if (!new_raw_ptr) return NULL;
#elif RAZIX_PLATFORM_UNIX
    void* new_raw_ptr = NULL;
    bool  need_copy   = true;
    if (posix_memalign(&new_raw_ptr, alignment, totalSize) != 0) {
        new_raw_ptr = NULL;
    }
    if (!new_raw_ptr) return NULL;
#else
    void* new_raw_ptr = realloc(raw_ptr, totalSize);
    bool  need_copy   = true;
    if (!new_raw_ptr) return NULL;
#endif

#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING
    if (tracking_active && rec) {
        u8* aligned_user = (u8*) rz_align_ptr(((u8*) new_raw_ptr) + sizeof(u64), alignment);
        u64 header       = ((u64) rec->id) << 32;
        *((u64*) (aligned_user - sizeof(u64))) = header;

        if (need_copy) {
            size_t copySize = oldSize < newSize ? oldSize : newSize;
            memcpy(aligned_user, oldPtr, copySize);
        }

        rz_critical_section_lock(&s_memtracker.lock);
        rec->raw_ptr      = new_raw_ptr;
        rec->user_ptr     = aligned_user;
        rec->size         = newSize;
        rec->alignment    = alignment;
        rec->timestamp    = rz_memtracker_timestamp();
        rec->thread_id    = rz_memtracker_thread_id();
        rec->active       = true;
        rz_critical_section_unlock(&s_memtracker.lock);

        return aligned_user;
    } else if (tracking_active && !rec) {
        // Fallback: treat as a new tracked allocation and copy, then free the old block raw
        void* new_user = rz_mem_alloc_internal(newSize, alignment, tag ? tag : "rz_realloc", file, line, 0, false);
        if (new_user) {
            size_t copySize = oldSize < newSize ? oldSize : newSize;
            memcpy(new_user, oldPtr, copySize);
        }
        // Free the original block without touching tracker (it wasn't tracked)
        rz_platform_aligned_free(raw_ptr);
        if (RZ_MEMTRACK_DIAGNOSTICS) {
            u64 header = rz_memtracker_peek_header(oldPtr);
            fprintf(stderr, "[Razix][Memory][diag] realloc on untracked pointer %p header=0x%016llx (file=%s line=%u)\n", oldPtr, (unsigned long long) header, file ? file : "<unknown>", line);
            if (!new_user) {
                fprintf(stderr, "[Razix][Memory][diag] realloc fallback alloc failed for pointer %p\n", oldPtr);
            }
        }
        return new_user;
    }
#endif

    if (need_copy) {
        size_t copySize = oldSize < newSize ? oldSize : newSize;
        memcpy(new_raw_ptr, oldPtr, copySize);
    }

    return new_raw_ptr;
}

void* rz_realloc(void* oldPtr, size_t oldSize, size_t newSize, size_t alignment)
{
    return rz_realloc_internal_with_meta(oldPtr, oldSize, newSize, alignment, "<unknown>", 0, "rz_realloc");
}

void* rz_realloc_debug(void* oldPtr, size_t oldSize, size_t newSize, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag)
{
    return rz_realloc_internal_with_meta(oldPtr, oldSize, newSize, alignment, filename, lineNumber, tag);
}

void* rz_realloc_aligned(void* oldPtr, size_t oldSize, size_t newSize)
{
    return rz_realloc(oldPtr, oldSize, newSize, 16);
}

void* rz_calloc(size_t count, size_t size, size_t alignment)
{
    size_t totalSize = count * size;
    return rz_mem_alloc_internal(totalSize, alignment, "rz_calloc", "<unknown>", 0, 0, true);
}

void* rz_calloc_aligned(size_t count, size_t size)
{
    return rz_calloc(count, size, 16);
}

void* rz_calloc_debug(size_t count, size_t size, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag)
{
    size_t totalSize = count * size;
    return rz_mem_alloc_internal(totalSize, alignment, tag ? tag : "rz_calloc", filename, lineNumber, 0, true);
}

void rz_free(void* address)
{
#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING
    void* raw_ptr = rz_memtracker_unwrap_pointer(address, NULL, 0);
#else
    void* raw_ptr = address;
#endif

#ifdef RAZIX_DEBUG
    rz_debug_free(raw_ptr);
#else
    if (raw_ptr) {
        rz_platform_aligned_free(raw_ptr);
    }
#endif
}

void rz_free_debug(void* address, const char* filename, uint32_t lineNumber)
{
    void* raw_ptr = address;
#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING
    raw_ptr = rz_memtracker_unwrap_pointer(address, filename, lineNumber);
#endif

#ifdef RAZIX_DEBUG
    rz_debug_free(raw_ptr);
#else
    if (raw_ptr) {
        rz_platform_aligned_free(raw_ptr);
    }
#endif
}

//--------------------------------------
// Utility helpers
//--------------------------------------

size_t rz_mem_align(size_t size, size_t alignment)
{
    const size_t alignment_mask = alignment - 1;
    return (size + alignment_mask) & ~alignment_mask;
}

void* rz_align_ptr(void* ptr, size_t alignment)
{
    uintptr_t addr    = (uintptr_t) (ptr);
    uintptr_t aligned = (addr + alignment - 1) & ~(alignment - 1);
    return (void*) (aligned);
}

#ifdef RAZIX_DEBUG

void* rz_debug_malloc(size_t size, size_t alignment, const char* filename, uint32_t lineNumber, const char* tag)
{
    void* addr = NULL;

    #ifdef RAZIX_PLATFORM_WINDOWS
    addr = _aligned_malloc(size, alignment);
    #elif RAZIX_PLATFORM_UNIX
    posix_memalign(&addr, alignment, size);
    #endif

    return addr;
}

void rz_debug_free(void* address)
{
    if (address) {
    #ifdef RAZIX_PLATFORM_WINDOWS
        _aligned_free(address);
    #elif RAZIX_PLATFORM_UNIX
        free(address);
    #endif
    }
}

#endif

RAZIX_API void rz_poison_memory(void* ptr, size_t size)
{
    if (ptr) {
        memset(ptr, RAZIX_MEMORY_POISON, size);
    }
}

#ifdef RAZIX_ENABLE_MEM_ALLOC_TRACKING

void rz_memory_tracking_set_enabled(bool enabled)
{
    rz_memtracker_init();
    s_memtracker.enabled = enabled;
}

bool rz_memory_tracking_is_enabled(void)
{
    return s_memtracker.enabled;
}

void rz_memory_tracker_report_leaks(void)
{
    if (!s_memtracker.initialized || !s_memtracker.enabled) return;

    rz_critical_section_lock(&s_memtracker.lock);

    u32  leak_count  = 0;
    u64  leak_bytes  = 0;

    for (size_t i = 1; i < s_memtracker.allocation_capacity; ++i) {
        const RZMemAllocationRecord* rec = &s_memtracker.allocations[i];
        if (rec->active) {
            ++leak_count;
            leak_bytes += rec->size;
        }
    }

    if (leak_count > 0) {
        fprintf(stderr, "[Razix][Memory] Detected %u leaked allocations totaling %llu bytes\n", leak_count, (unsigned long long) leak_bytes);
        for (size_t i = 1; i < s_memtracker.allocation_capacity; ++i) {
            const RZMemAllocationRecord* rec = &s_memtracker.allocations[i];
            if (!rec->active) continue;
                fprintf(stderr,
                    "  [Leak] alloc_id=%u size=%zu thread=%u name=%s magic=0x%08x user_ptr=%p raw_ptr=%p %s:%u\n",
                    rec->id,
                    rec->size,
                    rec->thread_id,
                    rec->name[0] ? rec->name : "<unnamed>",
                    rec->magic_number,
                    rec->user_ptr,
                    rec->raw_ptr,
                    rec->file[0] ? rec->file : "<unknown>",
                    rec->line);
        }
    }

    rz_critical_section_unlock(&s_memtracker.lock);
}

#endif    // RAZIX_ENABLE_MEM_ALLOC_TRACKING
