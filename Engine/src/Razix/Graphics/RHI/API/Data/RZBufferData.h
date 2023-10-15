#pragma once

namespace Razix {
    namespace Graphics {

        /* Defines how the buffer is used */
        enum class BufferUsage
        {
            Static,
            Dynamic,
            Stream,
            COUNT
        };

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
            "Dynamic",
            "Stream"};

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

    }    // namespace Graphics
}    // namespace Razix