//#pragma once
//
//// [Source]: https://github.com/ConfettiFX/The-Forge/blob/fabba8c90e6f53fbaeed1b74d2d81749b611a0a9/Common_3/Utilities/Interfaces/IMemory.h
//
//#ifndef malloc
//#define malloc(size) static_assert(false, "Please use razix_malloc");
//#endif
//#ifndef calloc
//#define calloc(count, size) static_assert(false, "Please use razix_calloc");
//#endif
//#ifndef memalign
//#define memalign(align, size) static_assert(false, "Please use razix_memalign");
//#endif
//#ifndef realloc
//
//#define realloc(ptr, size) static_assert(false, "Please use razix_realloc");
//#endif
//#ifndef free
//#define free(ptr) static_assert(false, "Please use razix_free");
//#endif
//
//#ifdef __cplusplus
//#ifndef new
//#define new static_assert(false, "Please use razix_new or razix_placement_new");
//#endif
//#ifndef delete
//#define delete static_assert(false, "Please use razix_free with explicit destructor call");
//#endif
//#endif
//
//// CUSTOM RAZIX MEM ALLOC FUNCTIONS
//#ifdef __cplusplus
//#ifndef razix_new
//#define razix_new new// tf_new_internal<ObjectType>(__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
//#endif
//#ifndef razix_delete
//#define razix_delete(ptr) delete(ptr)// tf_delete_internal(ptr, __FILE__, __LINE__, __FUNCTION__)
//#endif
//#endif
