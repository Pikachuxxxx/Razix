#pragma once
#include <stdbool.h>
#include <stdint.h>

// https://twitter.com/SebAaltonen/status/1534416275828514817
// https://twitter.com/SebAaltonen/status/1535175559067713536
/**
     * Handle is a weak pointer like reference to real objects inside a Pool, this forms the basis for various handles
     * 
     * Handles are like weak pointers. The container has an array of generation counters, and the data getter checks whether the generation counters match. 
     * If not, then the slot was deleted (and possibly reused). The getter returns null instead of the data pointer in this case
     */

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

    typedef struct rz_handle
    {
        uint32_t index;
        uint32_t generation;
    } rz_handle;

    static inline rz_handle rz_handle_create(uint32_t index, uint32_t generation)
    {
        rz_handle h = {index, generation};
        return h;
    }

    static inline rz_handle rz_handle_make_invalid()
    {
        rz_handle h = {0, 0};
        return h;
    }

    static inline uint32_t rz_handle_get_index(const rz_handle* handle)
    {
        return handle->index;
    }

    static inline void rz_handle_set_index(rz_handle* handle, uint32_t index)
    {
        handle->index = index;
    }

    static inline uint32_t rz_handle_get_generation(const rz_handle* handle)
    {
        return handle->generation;
    }

    static inline void rz_handle_set_generation(rz_handle* handle, uint32_t gen)
    {
        handle->generation = gen;
    }

    static inline bool rz_handle_is_valid(const rz_handle* handle)
    {
        return handle->generation != 0;
    }

    static inline void rz_handle_destroy(rz_handle* handle)
    {
        handle->index      = 0;
        handle->generation = 0;
    }

    static inline bool rz_handle_equals(const rz_handle* a, const rz_handle* b)
    {
        return a->index == b->index;
    }

    static inline bool rz_handle_not_equals(const rz_handle* a, const rz_handle* b)
    {
        return a->index != b->index;
    }

#ifdef __cplusplus
}
#endif    // __cplusplus
