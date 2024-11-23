#ifndef VIS_BUFFER_COMMON_H
#define VIS_BUFFER_COMMON_H

// Define bit positions for each piece of data
#define ALPHA_MASK_LOW_BIT 31    // 1 bit for Alpha Mask
#define DRAW_ID_LOW_BIT    23    // 8 bits for Draw ID
#define PRIM_ID_LOW_BIT    0     // 23 bits for Primitive ID

// Define masks for each component
#define ALPHA_MASK_BIT 0x00000001    // 1 bit mask for Alpha
#define DRAW_ID_MASK   0x7F800000    // 8 bit mask for Draw ID
#define PRIM_ID_MASK   0x007FFFFF    // 23 bit mask for Primitive ID

// num_thread size on which the VB shading CS threads are run on
#define VB_SHADE_TILE_SZ 16

// Macro to pack visibility buffer data
#define PACK_VISIBILITY_BUFFER_DATA(primitiveID, drawID, alphaMask) \
    (((primitiveID << PRIM_ID_LOW_BIT) & PRIM_ID_MASK) |            \
        ((drawID << DRAW_ID_LOW_BIT) & DRAW_ID_MASK) |              \
        ((alphaMask & ALPHA_MASK_BIT) << ALPHA_MASK_LOW_BIT))

#endif