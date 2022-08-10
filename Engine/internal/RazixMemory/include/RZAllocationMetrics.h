#pragma once

#include <cstdint>

#include "RZMemoryTags.h"

struct RZMemAllocationInfo
{
    const char* name;
    uint32_t    line;
    MemoryTag   tag;
    void*       address;
    size_t      size;
};

struct RZMemClusterAllocationInfo : public RZMemAllocationInfo
{
    uint32_t clusterID;
    uint32_t clusterSize;
};

struct RZMemView
{

};

struct RZMemAllocationSnapshot
{
    void AddAllocation(const RZMemAllocationInfo& allocationInfo);
    void AddAllocationCluster(const RZMemClusterAllocationInfo& allocationClusterInfo);

    bool CaptureAllocations(RZMemView& view);
    // This is a debug method to print total allocations
    void DisplayView(const RZMemView& view);
};

struct RZMemInfoElement
{

};

class RZMemoAllocationInfoIterator
{
public:

};