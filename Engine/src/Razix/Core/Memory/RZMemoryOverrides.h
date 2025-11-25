#ifndef _RZ_MEM_OVERRIDES_H_
#define _RZ_MEM_OVERRIDES_H_

#ifdef __cplusplus
extern "C"
{
#endif    // __cplusplus

    //#ifndef malloc
    //    #define malloc(size) PLEASE_USE_rz_malloc_or_rz_new
    //#endif
    //
    //#ifndef calloc
    //    #define calloc(count, size) PLEASE_USE_rz_calloc_or_rz_new_array_zero
    //#endif
    //
    //#ifndef memalign
    //    #define memalign(align, size) PLEASE_USE_rz_malloc_aligned_or_rz_new_aligned
    //#endif
    //
    //#ifndef realloc
    //    #define realloc(ptr, size) PLEASE_USE_rz_realloc
    //#endif
    //
    //#ifndef free
    //    #define free(ptr) PLEASE_USE_rz_delete
    //#endif

#define rz_new(type) \
    ((type*) rz_malloc(sizeof(type), _Alignof(type)))
#define rz_new_zero(type) \
    ((type*) rz_calloc(1, sizeof(type), _Alignof(type)))
#define rz_new_array(type, count) \
    ((type*) rz_malloc(sizeof(type) * (count), _Alignof(type)))
#define rz_new_array_zero(type, count) \
    ((type*) rz_calloc((count), sizeof(type), _Alignof(type)))
#define rz_new_aligned(type, alignment) \
    ((type*) rz_malloc(sizeof(type), (alignment)))
#define rz_new_array_aligned(type, count, alignment) \
    ((type*) rz_malloc(sizeof(type) * (count), (alignment)))

#define rz_delete(ptr)  \
    do {                \
        rz_free((ptr)); \
        (ptr) = NULL;   \
    } while (0)

#define rz_resize(ptr, type, new_count) \
    ((type*) rz_realloc_aligned((ptr), sizeof(type) * (new_count)))

#ifdef __cplusplus
}
#endif    // __cplusplus

#ifdef __cplusplus

    //#define new    PLEASE_USE_rz_new_INSTEAD_OF_cpp_new
    //#define delete PLEASE_USE_rz_delete_INSTEAD_OF_cpp_delete

#endif    // __cplusplus

#endif    // _RZ_MEM_OVERRIDES_H_