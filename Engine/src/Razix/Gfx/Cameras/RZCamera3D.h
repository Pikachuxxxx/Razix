#pragma once

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZDataTypes.h"

#include "Razix/Math/Frustum.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Default camera values
#define RZ_CAMERA3D_YAW           -90.0f
#define RZ_CAMERA3D_PITCH         0.0f
#define RZ_CAMERA3D_SPEED         0.025f
#define RZ_CAMERA3D_SENSITIVTY    0.25f
#define RZ_CAMERA3D_ZOOM          45.0f
#define RZ_CAMERA_FLAG_VIEW       (1u << 0)
#define RZ_CAMERA_FLAG_PROJECTION (1u << 1)

namespace Razix {
    enum class CameraMovementDirection
    {
        kForward,
        kBackward,
        kLeft,
        kRight,
        kUp,
        kDown,
        COUNT
    };

    class RAZIX_API RZCamera3D
    {
    public:
        enum class ProjectionType
        {
            kPerspective  = 0,
            kOrthographic = 1,
            COUNT
        };

    public:
        RZCamera3D() = default;

        void update(d32 deltaTime);
        void processKeyboard(CameraMovementDirection direction, d32 deltaTime);
        void processMouseMovement(f32 xoffset, f32 yoffset, bool constrainPitch = true);
        void processMouseScroll(f32 yoffset);

        inline float4x4 getViewProjection() { return getProjection() * getViewMatrix(); }
        inline float4x4 getProjection()
        {
            ensureProjectionUpdated();
            return m_Projection;
        }
        inline float4x4 getProjectionRaw() const { return m_Projection; }
        float4x4        getViewMatrix();
        float4x4        getViewMatrixLH();
        float4x4        getViewMatrixRH();

        void setPerspective(f32 verticalFOV, f32 nearClip, f32 farClip);
        void setOrthographic(f32 size, f32 nearClip, f32 farClip);
        void setViewportSize(u32 width, u32 height);

        inline ProjectionType getProjectionType() const { return m_ProjectionType; }
        inline void           setProjectionType(ProjectionType type)
        {
            m_ProjectionType = type;
            m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
        }

        inline f32  getPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
        inline void setPerspectiveVerticalFOV(f32 verticalFov)
        {
            m_PerspectiveFOV = verticalFov;
            m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
        }

        inline f32  getPerspectiveNearClip() const { return m_PerspectiveNear; }
        inline void setPerspectiveNearClip(f32 nearClip)
        {
            m_PerspectiveNear = nearClip;
            m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
        }

        inline f32  getPerspectiveFarClip() const { return m_PerspectiveFar; }
        inline void setPerspectiveFarClip(f32 farClip)
        {
            m_PerspectiveFar = farClip;
            m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
        }

        inline f32  getOrthographicSize() const { return m_OrthographicSize; }
        inline void setOrthographicSize(f32 size)
        {
            m_OrthographicSize = size;
            m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
        }

        inline f32  getOrthographicNearClip() const { return m_OrthographicNear; }
        inline void setOrthographicNearClip(f32 nearClip)
        {
            m_OrthographicNear = nearClip;
            m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
        }

        inline f32  getOrthographicFarClip() const { return m_OrthographicFar; }
        inline void setOrthographicFarClip(f32 farClip)
        {
            m_OrthographicFar = farClip;
            m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
        }

        inline f32  getAspectRatio() const { return m_AspectRatio; }
        inline void setAspectRatio(f32 ratio)
        {
            m_AspectRatio = ratio;
            m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
        }

        inline const float4& getBgColor() const { return m_BgColor; }
        inline void          setBgColor(const float4& color) { m_BgColor = color; }

        inline const Maths::RZFrustum& getFrustum() const { return m_CameraFrustum; }

        inline const float3& getPosition() const { return Position; }
        inline void          setPosition(float3 vector)
        {
            Position       = vector;
            TargetMovement = Position;
            m_DirtyFlags |= RZ_CAMERA_FLAG_VIEW;
        }

        inline const float3& getForward() const { return Front; }
        inline void          setForward(float3 vector)
        {
            Front = vector;
            m_DirtyFlags |= RZ_CAMERA_FLAG_VIEW;
        }
        inline const float3& getUp() const { return Up; }
        inline void          setUp(float3 vector)
        {
            Up = vector;
            m_DirtyFlags |= RZ_CAMERA_FLAG_VIEW;
        }
        inline const float3& getRight() const { return Right; }
        inline void          setRight(float3 vector)
        {
            Right = vector;
            m_DirtyFlags |= RZ_CAMERA_FLAG_VIEW;
        }
        inline const float3& getWorldUp() const { return WorldUp; }
        inline void          setWorldUp(float3 vector)
        {
            WorldUp = vector;
            m_DirtyFlags |= RZ_CAMERA_FLAG_VIEW;
        }

        inline const f32& getYaw() const { return Yaw; }
        inline void       setYaw(f32 value)
        {
            Yaw = value;
            m_DirtyFlags |= RZ_CAMERA_FLAG_VIEW;
        }

        inline const f32& getPitch() const { return Pitch; }
        inline void       setPitch(f32 value)
        {
            Pitch = value;
            m_DirtyFlags |= RZ_CAMERA_FLAG_VIEW;
        }

        inline const f32& getZoom() const { return Zoom; }
        void              setZoom(f32 value)
        {
            Zoom = value;
        }

        inline const f32& getSpeed() const { return MovementSpeed; }
        void              setSpeed(f32 speed)
        {
            MovementSpeed = speed;
        }

        inline const f32& getSensitivity() const { return MouseSensitivity; }
        void              setSensitivity(f32 sensitivity)
        {
            MouseSensitivity = sensitivity;
        }

    private:
        void ensureViewUpdated();
        void ensureProjectionUpdated();
        void updateCameraVectors();
        void recalculateProjection();

    private:
        float4x4         m_Projection = float4x4(1.0f);
        Maths::RZFrustum m_CameraFrustum = {};   
        float4           m_BgColor          = float4(0.0f);
        float3           Position           = float3(0.0f);
        f32              Yaw                = RZ_CAMERA3D_YAW;
        float3           Front              = float3(-0.1f, -0.18f, -0.88f);
        f32              Pitch              = RZ_CAMERA3D_PITCH;
        float3           Up                 = float3(0.0f);
        f32              MovementSpeed      = RZ_CAMERA3D_SPEED;
        float3           Right              = float3(0.0f);
        f32              MouseSensitivity   = RZ_CAMERA3D_SENSITIVTY;
        float3           WorldUp            = float3(0.0f, 1.0f, 0.0f);
        f32              Zoom               = RZ_CAMERA3D_ZOOM;
        float3           TargetMovement     = float3(0.0f);
        f32              DampingFactor      = 0.90f;
        float3           Velocity           = float3(0.0f);
        f32              m_AspectRatio      = 0.0f;
        f32              m_PerspectiveFOV   = glm::radians(45.0f);
        f32              m_PerspectiveNear  = 0.1f;
        f32              m_PerspectiveFar   = 1000.0f;
        f32              m_OrthographicSize = 10.0f;
        f32              m_OrthographicNear = -1.0f;
        f32              m_OrthographicFar  = 1.0f;
        f32              m_OldX             = 0.0f;
        f32              m_OldY             = 0.0f;
        ProjectionType   m_ProjectionType   = ProjectionType::kPerspective;
        u8               m_DirtyFlags       = static_cast<u8>(RZ_CAMERA_FLAG_VIEW | RZ_CAMERA_FLAG_PROJECTION);
    };

    namespace Gfx {
        using RZCamera3D = Razix::RZCamera3D;
    }    // namespace Gfx
}    // namespace Razix
