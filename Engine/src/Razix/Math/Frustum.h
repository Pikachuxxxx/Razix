#pragma once

namespace Razix {
    namespace Maths {

        static const unsigned NUM_FRUSTUM_PLANES   = 6;
        static const unsigned NUM_FRUSTUM_VERTICES = 8;

        class RZFrustum
        {
        public:
            RZFrustum()  = default;
            ~RZFrustum() = default;

            void build(float4x4 VP);

            bool IsInside(const float3& point) const
            {
                /*for (const auto& plane: m_Planes) {
                if (plane.Distance(point) < 0.0f)
                    return false;
            }*/

                return true;
            }

            const float3* getVertices() const { return m_Vertices; }

        private:
            //Plane     m_Planes[NUM_FRUSTUM_PLANES];
            float3 m_Vertices[NUM_FRUSTUM_VERTICES];
        };
    }    // namespace Maths
}    // namespace Razix
