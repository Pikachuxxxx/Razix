#pragma once

/**
 * Razix Memory is allocated from a set of well categorized pools that will be defined below
 * 
 * Their size, allocation strategy fragmentation will be defined and will also be influenced on how the objects are allocated
 * We will use macros to select the pools and other properties about alignment, speed, expansion etc (to be defined)
 */

#define RAZIX_CLASS_BIND_MEMORY_POOL(classType, poolType, allocationProperties)