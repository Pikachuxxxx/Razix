#pragma once

#include "Razix/Core/RZCore.h"
#include "Razix/Graphics/Cameras/Camera3D.h"

#include <glm/glm.hpp>

namespace Razix {

    class RAZIX_API RZSceneCamera : public Graphics::Camera3D
    {
    public:
        enum class ProjectionType
        {
            Perspective  = 0,
            Orthographic = 1
        };

    public:
        RZSceneCamera();
        virtual ~RZSceneCamera() = default;

        RAZIX_FORCE_INLINE glm::mat4 getViewProjection() { return getProjection() * getViewMatrix(); }

        RAZIX_FORCE_INLINE const glm::mat4& getProjection();

        void setPerspective(float verticalFOV, float nearClip, float farClip);
        void setOrthographic(float size, float nearClip, float farClip);
        void setViewportSize(uint32_t width, uint32_t height);

        RAZIX_INLINE ProjectionType getProjectionType() const { return m_ProjectionType; }
        RAZIX_INLINE void           setProjectionType(ProjectionType type)
        {
            m_ProjectionType = type;
            recalculateProjection();
        }

        RAZIX_INLINE float getPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
        RAZIX_INLINE void  setPerspectiveVerticalFOV(float verticalFov)
        {
            m_PerspectiveFOV = verticalFov;
            recalculateProjection();
        }

        RAZIX_INLINE float getPerspectiveNearClip() const { return m_PerspectiveNear; }
        RAZIX_INLINE void  setPerspectiveNearClip(float nearClip)
        {
            m_PerspectiveNear = nearClip;
            recalculateProjection();
        }

        RAZIX_INLINE float getPerspectiveFarClip() const { return m_PerspectiveFar; }
        RAZIX_INLINE void  setPerspectiveFarClip(float farClip)
        {
            m_PerspectiveFar = farClip;
            recalculateProjection();
        }

        RAZIX_INLINE float getOrthographicSize() const { return m_OrthographicSize; }
        RAZIX_INLINE void  setOrthographicSize(float size)
        {
            m_OrthographicSize = size;
            recalculateProjection();
        }

        RAZIX_INLINE float getOrthographicNearClip() const { return m_OrthographicNear; }
        RAZIX_INLINE void  setOrthographicNearClip(float nearClip)
        {
            m_OrthographicNear = nearClip;
            recalculateProjection();
        }

        RAZIX_INLINE float getOrthographicFarClip() const { return m_OrthographicFar; }
        RAZIX_INLINE void  setOrthographicFarClip(float farClip)
        {
            m_OrthographicFar = farClip;
            recalculateProjection();
        }

        RAZIX_INLINE float getAspectRatio() const { return m_AspectRatio; }
        RAZIX_INLINE void  setAspectRatio(float ratio)
        {
            m_AspectRatio = ratio;
            recalculateProjection();
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Position", getPosition()));
            archive(cereal::make_nvp("Front", getForward()));
            archive(cereal::make_nvp("Up", getUp()));
            archive(cereal::make_nvp("Right", getRight()));
            archive(cereal::make_nvp("WorldUp", getWorldUp()));

            archive(cereal::make_nvp("Yaw", getYaw()));
            archive(cereal::make_nvp("Pitch", getPitch()));
            archive(cereal::make_nvp("Zoom", getZoom()));

            archive(cereal::make_nvp("MovementSpeed", getSpeed()));
            archive(cereal::make_nvp("MouseSensitivity", getSensitivity()));

            //--------------------------------------------------------------------

            archive(cereal::make_nvp("ProjectionType", getProjectionType()));

            archive(cereal::make_nvp("PerspectiveFOV", getPerspectiveVerticalFOV()));
            archive(cereal::make_nvp("PerspectiveNearClip", getPerspectiveNearClip()));
            archive(cereal::make_nvp("PerspectiveFarClip", getPerspectiveFarClip()));

            archive(cereal::make_nvp("OrthographicSize", getOrthographicSize()));
            archive(cereal::make_nvp("OrthographicNearClip", getOrthographicNearClip()));
            archive(cereal::make_nvp("OrthographicFarClip", getOrthographicFarClip()));

            archive(cereal::make_nvp("AspectRatio", getAspectRatio()));
        }

        template<class Archive>
        void load(Archive& archive)
        {
            glm::vec3 value;
            archive(cereal::make_nvp("Position", value));
            setPosition(value);

            archive(cereal::make_nvp("Front", value));
            setForward(value);

            archive(cereal::make_nvp("Up", value));
            setUp(value);

            archive(cereal::make_nvp("Right", value));
            setRight(value);

            archive(cereal::make_nvp("WorldUp", value));
            setWorldUp(value);

            float fvalue;
            archive(cereal::make_nvp("Yaw", fvalue));
            setYaw(fvalue);

            archive(cereal::make_nvp("Pitch", fvalue));
            setPitch(fvalue);

            archive(cereal::make_nvp("Zoom", fvalue));
            setZoom(fvalue);

            archive(cereal::make_nvp("MovementSpeed", fvalue));
            setSpeed(fvalue);

            archive(cereal::make_nvp("MouseSensitivity", fvalue));
            setSensitivity(fvalue);

            //--------------------------------------------------------------------

            archive(cereal::make_nvp("ProjectionType", fvalue));
            if (!fvalue)
                setProjectionType(ProjectionType::Perspective);
            else
                setProjectionType(ProjectionType::Orthographic);

            archive(cereal::make_nvp("PerspectiveFOV", fvalue));
            setPerspectiveVerticalFOV(fvalue);

            archive(cereal::make_nvp("PerspectiveNearClip", fvalue));
            setPerspectiveNearClip(fvalue);

            archive(cereal::make_nvp("PerspectiveFarClip", fvalue));
            setPerspectiveFarClip(fvalue);

            archive(cereal::make_nvp("OrthographicSize", fvalue));
            setOrthographicSize(fvalue);

            archive(cereal::make_nvp("OrthographicNearClip", fvalue));
            setOrthographicNearClip(fvalue);

            archive(cereal::make_nvp("OrthographicFarClip", fvalue));
            setOrthographicFarClip(fvalue);

            archive(cereal::make_nvp("AspectRatio", fvalue));
            setAspectRatio(fvalue);
        }

    private:
        void recalculateProjection();

    private:
        ProjectionType m_ProjectionType = ProjectionType::Perspective;

        glm::mat4 m_Projection      = glm::mat4(1.0f);
        float     m_PerspectiveFOV  = glm::radians(45.0f);
        float     m_PerspectiveNear = 0.1f, m_PerspectiveFar = 100.0f;

        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

        float m_AspectRatio = 0.0f;
    };
}    // namespace Razix
