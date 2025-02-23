#ifndef MATH_H
#define MATH_H

#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
using namespace glm;

namespace Razix {
    namespace Math {

        // Taken from this article:
        // http://www.rorydriscoll.com/2016/03/07/frame-rate-independent-damping-using-lerp/

        float  lerp(float a, float b, float t, float dt);
        float3 lerp3(const float3& from, const float3& to, f32 t, f32 dt);
        // https://graphicscompendium.com/opengl/22-interpolation
        float3 lerp(float3 x, float3 y, float t);

        template<typename T>
        constexpr T min(T a, T b)
        {
            static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>, "'min' only accepts floating-point or integer types.");
            return (a < b) ? a : b;
        }

        template<typename T>
        constexpr T max(T a, T b)
        {
            static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>, "'max' only accepts floating-point or integer types.");
            return (a > b) ? a : b;
        }

    }    // namespace Math
}    // namespace Razix

#endif    // MATH_H