#pragma once

#include "Razix/Core/RZCore.h"
#include "Razix/Gfx/Cameras/Camera3D.h"

#include "Razix/Math/Frustum.h"

#include <glm/glm.hpp>

namespace Razix {

    class RAZIX_API RZSceneCamera : public Gfx::Camera3D
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

        inline float4x4 getViewProjection() { return getProjection() * getViewMatrix(); }

        inline float4x4 getProjection();
        inline float4x4 getProjectionRaw();

        void setPerspective(f32 verticalFOV, f32 nearClip, f32 farClip);
        void setOrthographic(f32 size, f32 nearClip, f32 farClip);
        void setViewportSize(u32 width, u32 height);

        inline ProjectionType getProjectionType() const { return m_ProjectionType; }
        inline void           setProjectionType(ProjectionType type)
        {
            m_ProjectionType = type;
            recalculateProjection();
        }

        inline f32  getPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
        inline void setPerspectiveVerticalFOV(f32 verticalFov)
        {
            m_PerspectiveFOV = verticalFov;
            recalculateProjection();
        }

        inline f32  getPerspectiveNearClip() const { return m_PerspectiveNear; }
        inline void setPerspectiveNearClip(f32 nearClip)
        {
            m_PerspectiveNear = nearClip;
            recalculateProjection();
        }

        inline f32  getPerspectiveFarClip() const { return m_PerspectiveFar; }
        inline void setPerspectiveFarClip(f32 farClip)
        {
            m_PerspectiveFar = farClip;
            recalculateProjection();
        }

        inline f32  getOrthographicSize() const { return m_OrthographicSize; }
        inline void setOrthographicSize(f32 size)
        {
            m_OrthographicSize = size;
            recalculateProjection();
        }

        inline f32  getOrthographicNearClip() const { return m_OrthographicNear; }
        inline void setOrthographicNearClip(f32 nearClip)
        {
            m_OrthographicNear = nearClip;
            recalculateProjection();
        }

        inline f32  getOrthographicFarClip() const { return m_OrthographicFar; }
        inline void setOrthographicFarClip(f32 farClip)
        {
            m_OrthographicFar = farClip;
            recalculateProjection();
        }

        inline f32  getAspectRatio() const { return m_AspectRatio; }
        inline void setAspectRatio(f32 ratio)
        {
            m_AspectRatio = ratio;
            recalculateProjection();
        }

        inline const float4& getBgColor() const { return m_BgColor; }
        inline void          setBgColor(const float4& color) { m_BgColor = color; }

        inline const Maths::RZFrustum& getFrustum() const { return m_CameraFrustum; }

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

            archive(cereal::make_nvp("BgColor", getBgColor()));
        }

        template<class Archive>
        void load(Archive& archive)
        {
            float3 value;
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

            f32 fvalue;
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

            float4 bg_color;
            archive(cereal::make_nvp("BgColor", bg_color));
            setBgColor(bg_color);
        }

    private:
        void recalculateProjection();

    private:
        ProjectionType   m_ProjectionType   = ProjectionType::Perspective;
        float4x4         m_Projection       = float4x4(1.0f);
        f32              m_PerspectiveFOV   = radians(45.0f);
        f32              m_PerspectiveNear  = 0.1f;
        f32              m_PerspectiveFar   = 1000.0f;
        f32              m_OrthographicSize = 10.0f;
        f32              m_OrthographicNear = -1.0f;
        f32              m_OrthographicFar  = 1.0f;
        f32              m_AspectRatio      = 0.0f;
        float4           m_BgColor          = float4(0.0f);
        Maths::RZFrustum m_CameraFrustum;
    };
}    // namespace Razix
