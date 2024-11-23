#pragma once

#include <cstdint>

#include "RZMemoryTags.h"

/**
 * Allocations will be divided into clusters depending on the category of Allocations such as GRAPHICS, SCENE, PHYSICS, SHARED_THREAD pools etc
 * Each cluster will have cells that contain same types for fixed aligned Allocations ex. a GRAPHICS cluster will have cells for VertexBuffer, IndexBuffer etc.
 */

struct RZMemAllocInfo
{
    const char* name;
    uint32_t    line;
    const char* file;
    MemoryTag   tag;
    void*       address;
    size_t      size;
};

class RZMemAllocInfoIterator
{
public:
};

struct RZMemClusterAllocInfo : public RZMemAllocInfo
{
    uint32_t clusterID;
    uint32_t clusterSize;
};

struct RZMemCellAllocInfo : public RZMemClusterAllocInfo
{
    uint32_t elementSize;
    uint32_t elementsCount;
    uint32_t cellID;
    uint8_t  cellAlignment;
};

struct RZMemView
{

};

struct RZMemAllocSnapshot
{
    void AddAlloc(const RZMemAllocInfo& AllocInfo);
    void AddAllocCluster(const RZMemClusterAllocInfo& AllocClusterInfo);

    bool CaptureAllocs(RZMemView& view);
    // This is a debug method to print total Allocations
    void DisplayView(const RZMemView& view);
};

struct RZMemInfoElement
{
};

struct RZMemAllocatorStats
{
    size_t   allocatedBytes;
    size_t   totalBytes;
    uint32_t allocationsCount;

    void Add(size_t alloc)
    {
        if (alloc) {
            allocatedBytes += alloc;
            ++allocationsCount;
        }
    }
};