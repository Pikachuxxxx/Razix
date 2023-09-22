#pragma once

namespace Razix {
    namespace Graphics {

        /* Defines how the buffer is used */
        enum class BufferUsage
        {
            Static,
            Dynamic,
            Stream
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
            AccelerationStructure
        };

    }    // namespace Graphics
}    // namespace Razix