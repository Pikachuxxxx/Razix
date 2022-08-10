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

struct RZMemAllocationSnapshot
{

};

struct RZMemInfoElement
{

};

class RZMemoAllocationInfoIterator
{
public:

};