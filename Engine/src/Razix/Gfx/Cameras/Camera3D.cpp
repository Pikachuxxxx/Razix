// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Camera3D.h"

#include "Razix/Core/OS/RZInput.h"

namespace Razix {
    namespace Gfx {

        Camera3D::Camera3D(float3 position /*= float3(0.0f, 0.0f, 0.0f)*/, float3 up /*= float3(0.0f, 1.0f, 0.0f)*/, f32 yaw /*= YAW*/, f32 pitch /*= PITCH*/)
        {
            this->Position = position;
            this->WorldUp  = up;
            this->Yaw      = yaw;
            this->Pitch    = pitch;
            this->updateCameraVectors();
        }

        Camera3D::Camera3D(f32 posX, f32 posY, f32 posZ, f32 upX, f32 upY, f32 upZ, f32 yaw, f32 pitch)
        {
            this->Position = float3(posX, posY, posZ);
            this->WorldUp  = float3(upX, upY, upZ);
            this->Yaw      = yaw;
            this->Pitch    = pitch;
            this->updateCameraVectors();
        }

        void Camera3D::update(d32 deltaTime)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Input management
            if (RZInput::IsKeyHeld(KeyCode::Key::Up) || RZInput::IsKeyHeld(KeyCode::Key::W))
                processKeyboard(FORWARD, deltaTime);
            if (RZInput::IsKeyHeld(KeyCode::Key::Down) || RZInput::IsKeyHeld(KeyCode::Key::S))
                processKeyboard(BACKWARD, deltaTime);
            if (RZInput::IsKeyHeld(KeyCode::Key::Right) || RZInput::IsKeyHeld(KeyCode::Key::D))
                processKeyboard(RIGHT, deltaTime);
            if (RZInput::IsKeyHeld(KeyCode::Key::Left) || RZInput::IsKeyHeld(KeyCode::Key::A))
                processKeyboard(LEFT, deltaTime);

            // Update position with lerping effect using the Velocity
            TargetMovement = this->Position + Velocity;
            this->Position = Math::lerp3(this->Position, TargetMovement, 0.6f, static_cast<f32>(deltaTime));

            // Gradually reduce the velocity over time to create the lingering effect
            Velocity *= DampingFactor;

            // Mouse movement handling
            auto mX = RZInput::GetMouseX();
            auto mY = RZInput::GetMouseY();

            f32 deltaX = mX - m_OldX;
            f32 deltaY = mY - m_OldY;

            if (RZInput::IsMouseButtonHeld(KeyCode::MouseKey::ButtonRight))
                processMouseMovement(deltaX, deltaY);

            m_OldX = mX;
            m_OldY = mY;
        }

        void Camera3D::processKeyboard(Camera_Movement_Direction direction, d32 deltaTime)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32    velocityMagnitude = this->MovementSpeed * static_cast<f32>(deltaTime);
            float3 inputMovement     = float3(0.0f);

            if (direction == FORWARD)
                inputMovement += this->Front;
            if (direction == BACKWARD)
                inputMovement -= this->Front;
            if (direction == LEFT)
                inputMovement -= this->Right;
            if (direction == RIGHT)
                inputMovement += this->Right;
            if (direction == UP)
                inputMovement += this->Up;
            if (direction == DOWN)
                inputMovement -= this->Up;

            // Scale input by velocity magnitude and add to current velocity
            Velocity += inputMovement * velocityMagnitude;
        }

        void Camera3D::processMouseMovement(f32 xoffset, f32 yoffset, bool constrainPitch /*= true*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            xoffset *= this->MouseSensitivity;
            yoffset *= this->MouseSensitivity;

            // TODO: Use lerp here to smoothly rotate the camera

            this->Yaw += xoffset;
            this->Pitch += yoffset;

            // Make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch) {
                if (this->Pitch > 89.0f)
                    this->Pitch = 89.0f;
                if (this->Pitch < -89.0f)
                    this->Pitch = -89.0f;
            }

            // Update Front, Right and Up Vectors using the updated Euler angles
            this->updateCameraVectors();
        }

        void Camera3D::processMouseScroll(f32 yoffset)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            RAZIX_UNIMPLEMENTED_METHOD
            if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
                this->Zoom -= yoffset;
            if (this->Zoom <= 1.0f)
                this->Zoom = 1.0f;
            if (this->Zoom >= 45.0f)
                this->Zoom = 45.0f;
        }

        float4x4 Camera3D::getViewMatrix() const
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return lookAt(this->Position, this->Position + this->Front, this->Up);
        }

        float4x4 Camera3D::getViewMatrixLH() const
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return lookAtLH(this->Position, this->Position + this->Front, this->Up);
        }

        float4x4 Camera3D::getViewMatrixRH() const
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return lookAtRH(this->Position, this->Position + this->Front, this->Up);
        }

        void Camera3D::updateCameraVectors()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Calculate the new Front vector
            float3 front = {};
            front.x      = cos(radians(this->Yaw)) * cos(radians(this->Pitch));
            front.y      = sin(radians(this->Pitch));
            front.z      = sin(radians(this->Yaw)) * cos(radians(this->Pitch));
            this->Front  = normalize(front);
            // Also re-calculate the Right and Up vector
            this->Right = normalize(cross(this->Front, this->WorldUp));    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            this->Up    = normalize(cross(this->Right, this->Front));
        }
    }    // namespace Gfx
}    // namespace Razix
