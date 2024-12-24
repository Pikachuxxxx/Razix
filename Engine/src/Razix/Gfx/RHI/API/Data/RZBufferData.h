#pragma once

namespace Razix {
    namespace Gfx {

        /* Defines how the buffer is used, enum cause of multiple flags for customization */
        enum class BufferUsage : u32
        {
            Static,           /* GPU only device memory, not CPU accessible, we can copy from GPU<->GPU though ig?, uses a staging buffer to copy any initial data */
            PersistentStream, /* GPU<->CPU two way mappable, but for for continuous updates, sequential updates                                                    */
            Staging,          /* Intermediate buffer for copying data from one Host to Device memory, Host visible                                                 */
            IndirectDrawArgs, /* GPU buffer for issuing indirect draw arguments                                                                                    */
            ReadBack,         /* Buffer to read back from GPU to CPU side                                                                                          */
            COUNT
        };

        // Overriding | operator to support bitwise OR/AND operations
        //RAZIX_ENUM_CLASS_BITWISE_COMPATIBLE(BufferUsage)

        /* Buffer Usage Additional flags for memory types */
        enum class BufferUsageAFlags
        {
            // These are more explicit types
            GPUOnly = 0,
            CPUOnly,
            CPUToGPU,
            GPUToCPU,
            GPUCopy,
            GPULazyAlloc,
            GenericAndPersistentlyMappableSlow,
            // These are more detailed usage types
            GPUCached,
            CPUCached,
            CPUCoherent,
            GPUCoherent,
            CPUSequentialWrite,
            CPUSequentialRead,
            CPURandomWrite,
            CPURandomRead,
            TransferHelp,
            COUNT
        };

        /* TODO: Useful for a unified buffer class similar to how RZTexture is */
        enum class BufferType
        {
            Constant,
            RWConstant,
            Storage,
            RWStructured,
            RWData,
            RWRegular,
            Structured,
            Data,
            Regular,
            AccelerationStructure,
            COUNT
        };

        static const char* BufferUsageNames[] = {
            "Static",
            "PersistentStream",
            "Staging",
            "IndirectDrawArgs",
            "ReadBack"};

        // Can't do this because it's enum and we have multiple options
        RAZIX_ENUM_NAMES_ASSERT(BufferUsageNames, BufferUsage);

        static const char* BufferTypeNames[] =
            {
                "Constant",
                "RWConstant",
                "Storage",
                "RWStructured",
                "RWData",
                "RWRegular",
                "Structured",
                "Data",
                "Regular",
                "AccelerationStructure"};

        RAZIX_ENUM_NAMES_ASSERT(BufferTypeNames, BufferType);

    }    // namespace Gfx
}    // namespace Razix