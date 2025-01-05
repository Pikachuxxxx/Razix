#ifndef IMPORTANCE_SAMPLING_H
#define IMPORTANCE_SAMPLING_H

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Math/MathConstants.h"

// Tasks
// [x] Finish the notes theory
// [] C++ code & publish notes
// [x] Re-use engine header files in shader --> Setup common file to use proper types etc. using a RZDataTypes.h file to help with type conversion
// [] Verify IBL gen on windows using renderdoc
// [] Fix resource cleanup bug with SkyboxPass

// This file will be shared in shaders so no cpp file...unless we move these to a seperate project called Shared that build as HLSL and Engine linkable libraries we will use header only implementation files that are shader b/w engine and shaders
namespace Razix {
    namespace Math {
        namespace ImportanceSampling {
            // Importance Sampling Functions
            // My Notes: https://github.com/Pikachuxxxx/Notes/blob/master/MonteCarlo/MonteCarlo.pdf
            // References:
            // http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html // Fast Hammersely2D optimization trick
            // https://www.youtube.com/watch?v=N6xZvrLusPI // Game Engine Series
            // https://www.youtube.com/watch?v=N6xZvrLusPI // Shaders Monthly
            // https://ttwong12.github.io/papers/udpoint/udpoint.pdf // Radical Inverse derivations

            //---------------------------------------------------------------
            // Quasi-Random Sequences for samples generation
            //---------------------------------------------------------------

            /* A single dimensional low-discrepancy radical inverse sequence for base p */
            f32 RadicalInverseVanDerCorputSample(u32 index, u32 base)
            {
                f32 inverseBase = 1.0f / base;
                f32 result = 0.0f;
                f32 fraction = inverseBase;

                while (index > 0) {
                    // Reflect the digits across the decimal point in the given base
                    result += (index % base) * fraction;
                    index /= base;
                    fraction *= inverseBase;
                }
                return result;
            }
        
            float RadicalInverseVdCBase2(uint bits) {
                 bits = (bits << 16u) | (bits >> 16u);
                 bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
                 bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
                 bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
                 bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
                 return float(bits) * 2.3283064365386963e-10; // / 0x100000000
             }

            // All these functions internally reuse the RadicalInverse_VDC for higher dimensions
            // Prefer Hammersley to Halton when total no. of samples are known in advance
            /**
             * Computes a single value of the Halton sequence for a given sample index and base
             * Increase the base to sample for higher dimensions, ex 3D/4D etc.
             * This is a quasi-random aka low-descrepancy random number generator, these can be
             * used with importance sampling where we need faster convergence and and lower variance
             */
            f32 HaltonSequenceSample(u32 index, u32 base)
            {
                return RadicalInverseVanDerCorputSample(index, base);
            }
            f32 HammersleySequenceSample(u32 index, u32 base, u32 totalSamples)
            {
                if (base == 1)
                     return (f32)index / (f32)totalSamples;
                 return RadicalInverseVanDerCorputSample(index, base);
            }
            /* [x_i/N, RadicalInverseVanDerCorput(index, base = 2) or Halton @ base = 2] */
            vec2 HammersleySequence2DSample(u32 index, u32 totalSamples)
            {
                return vec2((f32)index / (f32)totalSamples, RadicalInverseVanDerCorputSample(index, 2));
            }
            /**
             * Faster HammersleySequence2D(index, DIM = 2) bitwise trick
             * based on http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
             */
            vec2 HammersleySequence2DFastSample(u32 index, u32 totalSamples)
            {
                return vec2(f32(index) / f32(totalSamples), RadicalInverseVdCBase2(index));
            }

            //----------------------------
            // Hemisphere sampling utilities (uniform and cosine)
            // pass the Hammersley2D/Halton2D sequence points for sampling over the hemisphere
            // Uniformly sample point on a hemisphere.
            // See: "Physically Based Rendering" 2nd ed., section 13.6.1.
            vec3 HemisphereUniformSample(float u, float v)
            {
                float phi = v * 2.0 * PI;
                float cosTheta = 1.0 - u;
                float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
                return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
            }
        
            vec3 HemisphereCosSample(float u, float v)
            {
                float phi = v * 2.0 * PI;
                float cosTheta = sqrt(1.0 - u);
                float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
                return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
            }

        }
    }
}
#endif
