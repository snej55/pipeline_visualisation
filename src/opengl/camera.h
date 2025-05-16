//
// Created by Jens Kromdijk on 16/02/2025.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace CameraN
{
    enum class CameraMotion
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
    };

    // defaults
    constexpr float YAW{-90.0f};
    constexpr float PITCH{0.0f};
    constexpr float SPEED{25.f};
    constexpr float SENSITIVITY{0.05f};
    constexpr float ZOOM{45.0f};
}

class Camera
{
public:
    Camera()
    {
        _position = glm::vec3(0.0f, 0.0f, 3.0f);
        _front = glm::vec3(0.0f, 0.0f, -1.0f);
        _up = glm::vec3(0.0f, 1.0f, 0.0f);
        _worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        _yaw = CameraN::YAW;
        _pitch = CameraN::PITCH;
        _zoom = CameraN::ZOOM;
        _movementSpeed = CameraN::SPEED;
        _mouseSensitivity = CameraN::SENSITIVITY;
        updateCameraVectors();
    }

    [[nodiscard]] glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(_position, _position + _front, _up);
    }

    void processInput(const CameraN::CameraMotion direction, const float deltaTime)
    {
        const float velocity{_movementSpeed * deltaTime};
        switch (direction)
        {
            case (CameraN::CameraMotion::FORWARD):
                _position += _front * velocity;
                return;
            case CameraN::CameraMotion::BACKWARD:
                _position -= _front * velocity;
                return;
            case CameraN::CameraMotion::LEFT:
                _position -= _right * velocity;
                return;
            case CameraN::CameraMotion::RIGHT:
                _position += _right * velocity;
                break;
        }
    }

    void processMouseMovement(float xOffset, float yOffset, const GLboolean constrainPitch = true)
    {
        xOffset *= _mouseSensitivity;
        yOffset *= _mouseSensitivity;

        _yaw += xOffset;
        _pitch += yOffset;

        // cap pitch
        if (constrainPitch)
        {
            if (_pitch > 89.0f)
            {
                _pitch = 89.0f;
            }
            if (_pitch < -89.0f)
            {
                _pitch = -89.0f;
            }
        }

        // update front, right & up vectors
        updateCameraVectors();
    }

    void processMouseScroll(const float yOffset)
    {
        _zoom -= yOffset;
        if (_zoom < 1.0f)
        {
            _zoom = 1.0f;
        }
        if (_zoom > 45.0f)
        {
            _zoom = 45.0f;
        }
    }

    [[nodiscard]] float getZoom() const
    {
        return _zoom;
    }

    [[nodiscard]] glm::vec3 getPosition() const
    {
        return _position;
    }

    [[nodiscard]] glm::vec3 getFront() const
    {
        return _front;
    }

    [[nodiscard]] glm::vec3 getUp() const
    {
        return _up;
    }

    [[nodiscard]] glm::vec3 getRight() const
    {
        return _right;
    }

    [[nodiscard]] glm::vec3 getWorldUp() const
    {
        return _worldUp;
    }

    [[nodiscard]] float getYaw() const
    {
        return _yaw;
    }

    [[nodiscard]] float getPitch() const
    {
        return _pitch;
    }

    [[nodiscard]] float getMovementSpeed() const
    {
        return _movementSpeed;
    }

    [[nodiscard]] float getMouseSensitivity() const
    {
        return _mouseSensitivity;
    }

private:
    glm::vec3 _position{};
    glm::vec3 _front{};
    glm::vec3 _up{};
    glm::vec3 _right{};
    glm::vec3 _worldUp{};

    float _yaw;
    float _pitch;

    float _movementSpeed;
    float _mouseSensitivity;
    float _zoom;

    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = glm::cos(glm::radians(_yaw)) * glm::cos(glm::radians(_pitch));
        front.y = glm::sin(glm::radians(_pitch));
        front.z = glm::sin(glm::radians(_yaw)) * glm::cos(glm::radians(_pitch));
        _front = glm::normalize(front);
        _right = glm::normalize(glm::cross(_front, _worldUp));
        _up = glm::normalize(glm::cross(_right, _front));
    }
};


#endif //CAMERA_H
