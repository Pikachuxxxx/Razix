#pragma once

// TODO: Separate common tags and Thread tags to enforce more strict rules
/**
 * Used to tag memory to create cluster pools
 * 
 * Note:- Multiple types can be OR-ed together for shared allocation however it's recommended
 * only to OR common tags and Threads for optimal usage
 */
enum class MemoryTag
{
    // Common Tags
    GRAPHICS, 
    CORE, 
    APPLICATION, 
    SCENE,
    PHYSICS, 
    SCRIPTING, 
    COMMON,
    UNKNOWN, 
    // VERY IMPORTANT TAGS !!! 
    THREAD_GLOBAL,
    THREAD_SHARED
};

enum class MemoryLife
{
    Transient,  // Per-frame allocations will use a Bump/Ring based frame allocator
    Persistent  // These kind of allocations use a Pool based on memory Tag and alloc properties
};