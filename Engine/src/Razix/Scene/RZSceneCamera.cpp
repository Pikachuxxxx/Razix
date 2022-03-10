#include "rzxpch.h"
#include "RZSceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Razix {

    RZSceneCamera::RZSceneCamera()
    {
        recalculateProjection();
    }

    void RZSceneCamera::setPerspective(float verticalFOV, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Perspective;
        m_PerspectiveFOV = verticalFOV;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;
        recalculateProjection();
    }

    void RZSceneCamera::setOrthographic(float size, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Orthographic;
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;
        recalculateProjection();
    }

    void RZSceneCamera::setViewportSize(uint32_t width, uint32_t height)
    {
        RAZIX_CORE_ASSERT((width > 0 && height > 0), "Invalid width and height set for the scene camera!");
        m_AspectRatio = (float) width / (float) height;
        recalculateProjection();
    }

    void RZSceneCamera::recalculateProjection()
    {
        if (m_ProjectionType == ProjectionType::Perspective) {
            m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
        }
        else {
            float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
            float orthoBottom = -m_OrthographicSize * 0.5f;
            float orthoTop = m_OrthographicSize * 0.5f;

            m_Projection = glm::ortho(orthoLeft, orthoRight,
                orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
        }
    }

}