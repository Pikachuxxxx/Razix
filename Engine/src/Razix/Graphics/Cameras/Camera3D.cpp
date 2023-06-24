// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Camera3D.h"

#include "Razix/Core/OS/RZInput.h"

namespace Razix {
    namespace Graphics {

        // Taken from this article:
        // http://www.rorydriscoll.com/2016/03/07/frame-rate-independent-damping-using-lerp/
        //

        static float glm_lerp(float from, float to, float t)
        {
            return from + t * (to - from);
        }

        static float lerp(float a, float b, float t, float dt)
        {
            return glm_lerp(a, b, 1.f - powf(1 - t, dt));
        }

        static glm::vec3 lerp3(const glm::vec3& from, const glm::vec3& to, f32 t, f32 dt)
        {
            return glm::vec3{lerp(from.x, to.x, t, dt), lerp(from.y, to.y, t, dt), lerp(from.z, to.z, t, dt)};
        }

        // https://graphicscompendium.com/opengl/22-interpolation
        static glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t)
        {
            return x * (1.f - t) + y * t;
        }

        //----------------------------------------------------------------------------------------------------

        Camera3D::Camera3D(glm::vec3 position /*= glm::vec3(0.0f, 0.0f, 0.0f)*/, glm::vec3 up /*= glm::vec3(0.0f, 1.0f, 0.0f)*/, f32 yaw /*= YAW*/, f32 pitch /*= PITCH*/)
        {
            this->Position = position;
            this->WorldUp  = up;
            this->Yaw      = yaw;
            this->Pitch    = pitch;
            this->updateCameraVectors();
        }

        Camera3D::Camera3D(f32 posX, f32 posY, f32 posZ, f32 upX, f32 upY, f32 upZ, f32 yaw, f32 pitch)
        {
            this->Position = glm::vec3(posX, posY, posZ);
            this->WorldUp  = glm::vec3(upX, upY, upZ);
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
            else if (RZInput::IsKeyHeld(KeyCode::Key::Down) || RZInput::IsKeyHeld(KeyCode::Key::S))
                processKeyboard(BACKWARD, deltaTime);
            if (RZInput::IsKeyHeld(KeyCode::Key::Right) || RZInput::IsKeyHeld(KeyCode::Key::D))
                processKeyboard(RIGHT, deltaTime);
            else if (RZInput::IsKeyHeld(KeyCode::Key::Left) || RZInput::IsKeyHeld(KeyCode::Key::A))
                processKeyboard(LEFT, deltaTime);

            auto mX = RZInput::GetMouseX();
            auto mY = RZInput::GetMouseY();

            f32 deltaX = mX - m_OldX;
            f32 deltaY = mY - m_OldY;

            if (RZInput::IsMouseButtonHeld(KeyCode::MouseKey::ButtonRight))
                processMouseMovement(deltaX, -deltaY);

            m_OldX = mX;
            m_OldY = mY;
        }

        void Camera3D::processKeyboard(Camera_Movement_Direction direction, d32 deltaTime)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            f32 velocity = this->MovementSpeed * (f32) deltaTime;

            // TODO: Use lerp here to smoothly move the camera

            glm::vec3 CameraMovement = glm::vec3(0.0f);

            if (direction == FORWARD)
                CameraMovement += this->Front;
            if (direction == BACKWARD)
                CameraMovement -= this->Front;
            if (direction == LEFT)
                CameraMovement -= this->Right;
            if (direction == RIGHT)
                CameraMovement += this->Right;
            if (direction == UP)
                CameraMovement += this->Up;
            if (direction == DOWN)
                CameraMovement -= this->Up;

            TargetMovement = TargetMovement + (CameraMovement * velocity);

            this->Position = TargetMovement;
            //lerp3(this->Position, TargetMovement, 0.9f, deltaTime);
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

        glm::mat4 Camera3D::getViewMatrix()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
        }

        glm::mat4 Camera3D::getViewMatrixLH()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return glm::lookAtLH(this->Position, this->Position + this->Front, this->Up);
        }

        glm::mat4 Camera3D::getViewMatrixRH()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            return glm::lookAtRH(this->Position, this->Position + this->Front, this->Up);
        }

        void Camera3D::updateCameraVectors()
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Calculate the new Front vector
            glm::vec3 front;
            front.x     = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
            front.y     = sin(glm::radians(this->Pitch));
            front.z     = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
            this->Front = glm::normalize(front);
            // Also re-calculate the Right and Up vector
            this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
            this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
        }

    }    // namespace Graphics
}    // namespace Razix