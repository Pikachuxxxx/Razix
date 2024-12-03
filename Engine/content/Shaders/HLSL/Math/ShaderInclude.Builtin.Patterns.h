#ifndef PATTERNS_H
#define PATTERNS_H

namespace Patterns {

//------------------------------------------------------------------------
// Grid like pattern
// [Source]: https://github.com/microsoft/DirectX-Graphics-Samples/blob/master/Samples/Desktop/D3D12SM6WaveIntrinsics/readme.md

float patternGrid(float2 ndcPos, float scale) {
    float t = sin(ndcPos.x * scale) + cos(ndcPos.y * scale);
    float c = smoothstep(0.0, 0.25, t*t);
    return c;
}

//------------------------------------------------------------------------

}
#endif