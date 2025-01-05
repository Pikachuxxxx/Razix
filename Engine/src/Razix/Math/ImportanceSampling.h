#ifndef IMPORTANCE_SAMPLING_H
#define IMPORTANCE_SAMPLING_H

#include "Razix/Core/RZDataTypes.h"
#include "Razix/Math/MathConstants.h"

// Tasks
// [] Finish the notes
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
                return 0;
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
                return 0;
            }
            f32 HammersleySequenceSample(u32 index, u32 base, u32 totalSamples)
            {
                return 0;
            }
            /* [x_i/N, RadicalInverseVanDerCorput(index, base = 2) or Halton @ base = 2] */
            vec2 HammersleySequence2DSample(u32 index, u32 totalSamples)
            {
                return vec2(0, 0);
            }
            /**
             * Faster HammersleySequence2D(index, DIM = 2) bitwise trick
             * based on http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
             */
            vec2 HammersleySequence2DFastSample(u32 index, u32 totalSamples)
            {
                return vec2(0, 0);
            }

            //----------------------------
            // Hemisphere sampling utilities (uniform and cosine)
            // pass the Hammersley2D/Halton2D sequence points for sampling over the hemisphere
            // Uniformly sample point on a hemisphere.
            // See: "Physically Based Rendering" 2nd ed., section 13.6.1.
            vec3 HemisphereUniformSample(float u, float v)
            {
                const float u1p = sqrt(fmax(0.0f, 1.0f - u * v));
                return vec3(cos(Razix::Math::TwoPI * v) * u1p, sin(Math::TwoPI * v) * u1p, u);
            }
        
            vec3 HemisphereCosSample(float u, float v)
            {
                return vec3(0, 0, 0);
            }

        }
    }
}
#endif
