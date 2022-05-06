// clang-format off
#include "rzxpch.h"
// clang-format on
#include "Camera3D.h"

#include "Razix/Core/OS/RZInput.h"

namespace Razix {
    namespace Graphics {

        Camera3D::Camera3D(glm::vec3 position /*= glm::vec3(0.0f, 0.0f, 0.0f)*/, glm::vec3 up /*= glm::vec3(0.0f, 1.0f, 0.0f)*/, float yaw /*= YAW*/, float pitch /*= PITCH*/)
        {
            this->Position = position;
            this->WorldUp  = up;
            this->Yaw      = yaw;
            this->Pitch    = pitch;
            this->updateCameraVectors();
        }

        Camera3D::Camera3D(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        {
            this->Position = glm::vec3(posX, posY, posZ);
            this->WorldUp  = glm::vec3(upX, upY, upZ);
            this->Yaw      = yaw;
            this->Pitch    = pitch;
            this->updateCameraVectors();
        }

        void Camera3D::update(double deltaTime)
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

            float deltaX = mX - m_OldX;
            float deltaY = mY - m_OldY;

            //RAZIX_CORE_TRACE("Mouse delta [ X : {0} | Y : {1} ]", deltaX, deltaY);

            if (RZInput::IsMouseButtonHeld(KeyCode::MouseKey::ButtonRight))
                processMouseMovement(deltaX, -deltaY);

            m_OldX = mX;
            m_OldY = mY;
        }

        void Camera3D::processKeyboard(Camera_Movement_Direction direction, double deltaTime)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            float velocity = this->MovementSpeed * (float) deltaTime;

            if (direction == FORWARD)
                this->Position += this->Front * velocity;
            if (direction == BACKWARD)
                this->Position -= this->Front * velocity;
            if (direction == LEFT)
                this->Position -= this->Right * velocity;
            if (direction == RIGHT)
                this->Position += this->Right * velocity;
            if (direction == UP)
                this->Position += this->Up * velocity;
            if (direction == DOWN)
                this->Position -= this->Up * velocity;
        }

        void Camera3D::processMouseMovement(float xoffset, float yoffset, bool constrainPitch /*= true*/)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            xoffset *= this->MouseSensitivity;
            yoffset *= this->MouseSensitivity;

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

        void Camera3D::processMouseScroll(float yoffset)
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