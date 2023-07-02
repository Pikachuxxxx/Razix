#pragma once

namespace Razix {
    namespace Maths {

        static const unsigned NUM_FRUSTUM_PLANES   = 6;
        static const unsigned NUM_FRUSTUM_VERTICES = 8;

        class Frustum
        {
        public:
            Frustum()  = default;
            ~Frustum() = default;

            //void build(f32 fov, f32 aspectRatio, f32 zoom, f32 farz, glm::mat3x4);

            bool IsInside(const glm::vec3& point) const
            {
                /*for (const auto& plane: m_Planes) {
                if (plane.Distance(point) < 0.0f)
                    return false;
            }*/

                return true;
            }

            glm::vec3* getVertices() { return vertices_; }

        private:
            //Plane     m_Planes[NUM_FRUSTUM_PLANES];
            glm::vec3 vertices_[NUM_FRUSTUM_VERTICES];
        };
    }    // namespace Maths
}    // namespace Razix
