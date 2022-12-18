#pragma once

namespace Razix {

    static const unsigned NUM_FRUSTUM_PLANES   = 6;
    static const unsigned NUM_FRUSTUM_VERTICES = 8;

    class RZFrustum
    {
    public:
        RZFrustum()  = default;
        ~RZFrustum() = default;

        //void build(float fov, float aspectRatio, float zoom, float farz, glm::mat3x4);

        bool IsInside(const glm::vec3& point) const
        {
            /*for (const auto& plane: m_Planes) {
                if (plane.Distance(point) < 0.0f)
                    return false;
            }*/

            return true;
        }

    private:
        //Plane     m_Planes[NUM_FRUSTUM_PLANES];
        glm::vec3 vertices_[NUM_FRUSTUM_VERTICES];
    };
}    // namespace Razix
