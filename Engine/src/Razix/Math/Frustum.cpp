// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Frustum.h"

namespace Razix {
    namespace Maths {

        void RZFrustum::build(float4x4 VP)
        {
            static const bool zerotoOne = false;

            float4x4 transformInv = inverse(VP);

            m_Vertices[0] = float4(-1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
            m_Vertices[1] = float4(1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
            m_Vertices[2] = float4(1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
            m_Vertices[3] = float4(-1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);

            m_Vertices[4] = float4(-1.0f, -1.0f, 1.0f, 1.0f);
            m_Vertices[5] = float4(1.0f, -1.0f, 1.0f, 1.0f);
            m_Vertices[6] = float4(1.0f, 1.0f, 1.0f, 1.0f);
            m_Vertices[7] = float4(-1.0f, 1.0f, 1.0f, 1.0f);

            float4 temp;
            for (int i = 0; i < 8; i++) {
                temp          = transformInv * float4(m_Vertices[i], 1.0f);
                m_Vertices[i] = temp / temp.w;
            }
        }
    }    // namespace Maths
}    // namespace Razix
