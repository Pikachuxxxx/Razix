// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrustum.h"

namespace Razix {
    namespace Maths {

        void RZFrustum::build(glm::mat4 VP)
        {
            static const bool zerotoOne = false;
            static const bool leftHand  = true;

            glm::mat4 transformInv = glm::inverse(VP);

            m_Vertices[0] = glm::vec4(-1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
            m_Vertices[1] = glm::vec4(1.0f, -1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
            m_Vertices[2] = glm::vec4(1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);
            m_Vertices[3] = glm::vec4(-1.0f, 1.0f, zerotoOne ? 0.0f : -1.0f, 1.0f);

            m_Vertices[4] = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
            m_Vertices[5] = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
            m_Vertices[6] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            m_Vertices[7] = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);

            glm::vec4 temp;
            for (int i = 0; i < 8; i++) {
                temp          = transformInv * glm::vec4(m_Vertices[i], 1.0f);
                m_Vertices[i] = temp / temp.w;
            }
        }
    }    // namespace Maths
}    // namespace Razix