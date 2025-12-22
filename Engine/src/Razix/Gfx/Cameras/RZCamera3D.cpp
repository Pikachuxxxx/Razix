// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZCamera3D.h"

#include "Razix/Core/OS/RZInput.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Razix {

    RZCamera3D::RZCamera3D(float3 position, float3 up, f32 yaw, f32 pitch)
    {
        Position = position;
        WorldUp  = up;
        Yaw      = yaw;
        Pitch    = pitch;
        m_DirtyFlags |= RZ_CAMERA_FLAG_VIEW;
        ensureViewUpdated();
        ensureProjectionUpdated();
    }

    RZCamera3D::RZCamera3D(f32 posX, f32 posY, f32 posZ, f32 upX, f32 upY, f32 upZ, f32 yaw, f32 pitch)
    {
        Position = float3(posX, posY, posZ);
        WorldUp  = float3(upX, upY, upZ);
        Yaw      = yaw;
        Pitch    = pitch;
        m_DirtyFlags |= RZ_CAMERA_FLAG_VIEW;
        ensureViewUpdated();
        ensureProjectionUpdated();
    }

    void RZCamera3D::update(d32 deltaTime)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        // Prefer a Controller if connected or else fallback to mouse and keyboard
        if (RZInput::IsGamepadConnected()) {
            f32 leftX = RZInput::GetJoyLeftStickHorizontal();
            f32 leftY = RZInput::GetJoyLeftStickVertical();

            MovementSpeed = 0.01f;

            const f32               deadZone      = 0.15f;
            CameraMovementDirection direction     = CameraMovementDirection::kForward;
            glm::vec3               abs_direction = {};

            if (fabs(leftY) > deadZone) {
                direction = (leftY < 0.0f ? CameraMovementDirection::kForward : CameraMovementDirection::kBackward);
                abs_direction.y += leftY;
            }
            if (fabs(leftX) > deadZone) {
                direction = (leftX < 0.0f ? CameraMovementDirection::kLeft : CameraMovementDirection::kRight);
                abs_direction.x += leftX;
            }

            if (glm::length(abs_direction) > 0.0f)
                processKeyboard(direction, deltaTime);

            f32 rightX = RZInput::GetJoyRightStickHorizontal();
            f32 rightY = RZInput::GetJoyRightStickVertical();

            if (fabs(rightX) > deadZone || fabs(rightY) > deadZone)
                processMouseMovement(rightX * 7.5f, rightY * 7.5f);

            TargetMovement = Position + Velocity;
            Position       = Math::lerp3(Position, TargetMovement, 0.6f, static_cast<f32>(deltaTime));
            Velocity *= DampingFactor;

        } else {
            // Input management
            if (RZInput::IsKeyHeld(KeyCode::Key::Up) || RZInput::IsKeyHeld(KeyCode::Key::W))
                processKeyboard(CameraMovementDirection::kForward, deltaTime);
            if (RZInput::IsKeyHeld(KeyCode::Key::Down) || RZInput::IsKeyHeld(KeyCode::Key::S))
                processKeyboard(CameraMovementDirection::kBackward, deltaTime);
            if (RZInput::IsKeyHeld(KeyCode::Key::Right) || RZInput::IsKeyHeld(KeyCode::Key::D))
                processKeyboard(CameraMovementDirection::kRight, deltaTime);
            if (RZInput::IsKeyHeld(KeyCode::Key::Left) || RZInput::IsKeyHeld(KeyCode::Key::A))
                processKeyboard(CameraMovementDirection::kLeft, deltaTime);

            // Update position with lerping effect using the Velocity
            TargetMovement = Position + Velocity;
            Position       = Math::lerp3(Position, TargetMovement, 0.6f, static_cast<f32>(deltaTime));

            // Gradually reduce the velocity over time to create the lingering effect
            Velocity *= DampingFactor;

            // Mouse movement handling
            auto mX = RZInput::GetMouseX();
            auto mY = RZInput::GetMouseY();

            f32 deltaX = mX - m_OldX;
            f32 deltaY = mY - m_OldY;

            if (RZInput::IsMouseButtonHeld(KeyCode::MouseKey::ButtonRight))
                processMouseMovement(deltaX, -deltaY);

            m_OldX = mX;
            m_OldY = mY;
        }
        ensureViewUpdated();
        ensureProjectionUpdated();
    }

    void RZCamera3D::processKeyboard(CameraMovementDirection direction, d32 deltaTime)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        f32    velocityMagnitude = MovementSpeed * static_cast<f32>(deltaTime);
        float3 inputMovement     = float3(0.0f);

        if (direction == CameraMovementDirection::kForward)
            inputMovement += Front;
        if (direction == CameraMovementDirection::kBackward)
            inputMovement -= Front;
        if (direction == CameraMovementDirection::kLeft)
            inputMovement -= Right;
        if (direction == CameraMovementDirection::kRight)
            inputMovement += Right;
        if (direction == CameraMovementDirection::kUp)
            inputMovement += Up;
        if (direction == CameraMovementDirection::kDown)
            inputMovement -= Up;

        // Scale input by velocity magnitude and add to current velocity
        Velocity += inputMovement * velocityMagnitude;
    }

    void RZCamera3D::processMouseMovement(f32 xoffset, f32 yoffset, bool constrainPitch)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        // TODO: Use lerp here to smoothly rotate the camera

        Yaw += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        m_DirtyFlags |= RZ_CAMERA_FLAG_VIEW;
    }

    void RZCamera3D::processMouseScroll(f32 yoffset)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        RAZIX_UNIMPLEMENTED_METHOD
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

    float4x4 RZCamera3D::getViewMatrix()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        ensureViewUpdated();
        return glm::lookAt(Position, Position + Front, Up);
    }

    float4x4 RZCamera3D::getViewMatrixLH()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        ensureViewUpdated();
        return glm::lookAtLH(Position, Position + Front, Up);
    }

    float4x4 RZCamera3D::getViewMatrixRH()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);
        ensureViewUpdated();
        return glm::lookAtRH(Position, Position + Front, Up);
    }

    void RZCamera3D::setPerspective(f32 verticalFOV, f32 nearClip, f32 farClip)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        m_ProjectionType  = ProjectionType::kPerspective;
        m_PerspectiveFOV  = verticalFOV;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar  = farClip;
        m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
    }

    void RZCamera3D::setOrthographic(f32 size, f32 nearClip, f32 farClip)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        m_ProjectionType   = ProjectionType::kOrthographic;
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar  = farClip;
        m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
    }

    void RZCamera3D::setViewportSize(u32 width, u32 height)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        RAZIX_CORE_ASSERT((width > 0 && height > 0), "Invalid width and height set for the scene camera!");
        m_AspectRatio = (f32) width / (f32) height;
        m_DirtyFlags |= RZ_CAMERA_FLAG_PROJECTION;
    }

    void RZCamera3D::recalculateProjection()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_SCENE);

        if (m_ProjectionType == ProjectionType::kPerspective) {
            m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
        } else {
            f32 orthoLeft   = -m_OrthographicSize * m_AspectRatio * 0.5f;
            f32 orthoRight  = m_OrthographicSize * m_AspectRatio * 0.5f;
            f32 orthoBottom = -m_OrthographicSize * 0.5f;
            f32 orthoTop    = m_OrthographicSize * 0.5f;

            m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
        }

        m_CameraFrustum.build(m_Projection * getViewMatrix());
        m_DirtyFlags &= static_cast<u8>(~RZ_CAMERA_FLAG_PROJECTION);
    }

    void RZCamera3D::ensureProjectionUpdated()
    {
        if (m_DirtyFlags & RZ_CAMERA_FLAG_PROJECTION)
            recalculateProjection();
    }

    void RZCamera3D::updateCameraVectors() 
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

        float3 front = {};
        front.x      = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y      = sin(glm::radians(Pitch));
        front.z      = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front        = normalize(front);
        Right        = normalize(glm::cross(Front, WorldUp));
        Up           = normalize(glm::cross(Right, Front));
        m_DirtyFlags &= static_cast<u8>(~RZ_CAMERA_FLAG_VIEW);
    }

    void RZCamera3D::ensureViewUpdated()
    {
        if (m_DirtyFlags & RZ_CAMERA_FLAG_VIEW)
            updateCameraVectors();
    }
}    // namespace Razix
