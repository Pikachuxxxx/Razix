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

            void build(glm::mat4 VP);

            bool IsInside(const glm::vec3& point) const
            {
                /*for (const auto& plane: m_Planes) {
                if (plane.Distance(point) < 0.0f)
                    return false;
            }*/

                return true;
            }

            const glm::vec3* getVertices() const { return m_Vertices; }

        private:
            //Plane     m_Planes[NUM_FRUSTUM_PLANES];
            glm::vec3 m_Vertices[NUM_FRUSTUM_VERTICES];
        };
    }    // namespace Maths
}    // namespace Razix
