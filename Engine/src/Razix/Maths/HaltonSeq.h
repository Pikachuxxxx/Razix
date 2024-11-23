#pragma once

#include "razix/Core/RZDataTypes.h"

/**
 * [Source]: https://stackoverflow.com/questions/42661304/implementing-4-dimensional-halton-sequence
 * Computes a single value of the Halton sequence for a given index and base.
 * 
 * @param index The position of the point in the Halton sequence. 
 *              Must be a positive integer.
 * @param base The base of the numeral system used for the calculation. 
 *             Typically a prime number, and different bases are used for 
 *             different dimensions in the Halton sequence.
 * @return A double value representing the coordinate of the Halton sequence 
 *         point at the given index and base.
 */
static d32 HaltonSequence(u32 index, u32 base)
{
    float f = 1.0f;
    float r = 0.0f;

    while (index > 0) {
        f /= static_cast<float>(base);
        r     = r + f * static_cast<float>(index % base);
        index = static_cast<uint32_t>(floorf(static_cast<float>(index) / static_cast<float>(base)));
    }
    return r;
}