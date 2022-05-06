#pragma once

// Std. Includes
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

namespace Razix {
    namespace Graphics {
        // Default camera values
        const float YAW        = -90.0f;
        const float PITCH      = 0.0f;
        const float SPEED      = 0.025f;
        const float SENSITIVTY = 0.25f;
        const float ZOOM       = 45.0f;

        // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
        enum Camera_Movement_Direction
        {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

        class RAZIX_API Camera3D
        {
        public:
            // Constructor with vectors
            Camera3D (glm::vec3 position = glm::vec3 (0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3 (0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
            // Constructor with scalar values
            Camera3D (float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
            // Update the camera movement in the world space
            void update (double deltaTime);
            // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
            void processKeyboard (Camera_Movement_Direction direction, double deltaTime);
            // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
            void processMouseMovement (float xoffset, float yoffset, bool constrainPitch = true);
            // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
            void processMouseScroll (float yoffset);

            RAZIX_INLINE const glm::vec3& getPosition () const { return Position; }
            void                          setPosition (glm::vec3 vector)
            {
                Position = vector;
                updateCameraVectors ();
            }

            RAZIX_INLINE const glm::vec3& getForward () const { return Front; }
            void                          setForward (glm::vec3 vector)
            {
                Front = vector;
                updateCameraVectors ();
            }

            RAZIX_INLINE const glm::vec3& getUp () const { return Up; }
            void                          setUp (glm::vec3 vector)
            {
                Up = vector;
                updateCameraVectors ();
            }

            RAZIX_INLINE const glm::vec3& getRight () const { return Right; }
            void                          setRight (glm::vec3 vector)
            {
                Right = vector;
                updateCameraVectors ();
            }

            RAZIX_INLINE const glm::vec3& getWorldUp () const { return WorldUp; }
            void                          setWorldUp (glm::vec3 vector)
            {
                WorldUp = vector;
                updateCameraVectors ();
            }

            RAZIX_INLINE const float& getYaw () const { return Yaw; }
            void                      setYaw (float value)
            {
                Yaw = value;
                updateCameraVectors ();
            }

            RAZIX_INLINE const float& getPitch () const { return Pitch; }
            void                      setPitch (float value)
            {
                Pitch = value;
                updateCameraVectors ();
            }

            RAZIX_INLINE const float& getZoom () const { return Zoom; }
            void                      setZoom (float value)
            {
                Zoom = value;
                updateCameraVectors ();
            }

            RAZIX_INLINE const float& getSpeed () const { return MovementSpeed; }
            void                      setSpeed (float speed)
            {
                MovementSpeed = speed;
                updateCameraVectors ();
            }

            RAZIX_INLINE const float& getSensitivity () const { return MouseSensitivity; }
            void                      setSensitivity (float sensitivity)
            {
                MouseSensitivity = sensitivity;
                updateCameraVectors ();
            }

            // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
            RAZIX_FORCE_INLINE glm::mat4 getViewMatrix ();

            glm::mat4 getViewMatrixLH ();
            glm::mat4 getViewMatrixRH ();

        private:
            // Camera Attributes
            glm::vec3 Position;
            glm::vec3 Front;
            glm::vec3 Up;
            glm::vec3 Right;
            glm::vec3 WorldUp;
            // Euler Angles
            float Yaw   = YAW;
            float Pitch = PITCH;
            // Camera options
            float MovementSpeed    = SPEED;
            float MouseSensitivity = SENSITIVTY;
            float Zoom             = ZOOM;

            float m_OldX = 0, m_OldY = 0;

        private:
            // Calculates the front vector from the Camera's (updated) Euler Angles
            void updateCameraVectors ();
        };

    }    // namespace Graphics
}    // namespace Razix
