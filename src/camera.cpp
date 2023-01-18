#include "camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : _front(glm::vec3(0.0f, 0.0f, -1.0f)), _movement_speed(CAMERA_SPEED), _mouse_sensitivity(CAMERA_SENSITIVITY), _zoom(CAMERA_ZOOM)
{
    _position = position;
    _world_up = up;
    _yaw      = yaw;
    _pitch    = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : _front(glm::vec3(0.0f, 0.0f, -1.0f)), _movement_speed(CAMERA_SPEED), _mouse_sensitivity(CAMERA_SENSITIVITY), _zoom(CAMERA_ZOOM)
{
    _position = glm::vec3(posX, posY, posZ);
    _world_up = glm::vec3(upX, upY, upZ);
    _yaw      = yaw;
    _pitch    = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() { return glm::lookAt(_position, _position + _front, _up); }

glm::mat4 Camera::getProjectionMatrix(float screen_width, float screen_height)
{
    return glm::perspective(glm::radians(90.f), screen_width / screen_height, 0.1f, 100.0f);
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = _movement_speed * deltaTime;
    switch (direction)
    {
        case FORWARD:
            _position += _front * velocity;
            break;
        case BACKWARD:
            _position -= _front * velocity;
            break;
        case LEFT:
            _position -= _right * velocity;
            break;
        case RIGHT:
            _position += _right * velocity;
            break;
        default:
            break;
    }
}

void Camera::processMouseMovement(float x_offset, float y_offset, bool constrain_pitch)
{
    x_offset *= _mouse_sensitivity;
    y_offset *= _mouse_sensitivity;

    _yaw += x_offset;
    _pitch += y_offset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrain_pitch)
    {
        if (_pitch > 89.0f)
        {
            _pitch = 89.0f;
        }
        else if (_pitch < -89.0f)
        {
            _pitch = -89.0f;
        }
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::processMouseScroll(float y_offset)
{
    _zoom -= (float)y_offset;
    if (_zoom < 1.0f)
    {
        _zoom = 1.0f;
    }
    else if (_zoom > 90.0f)
    {
        _zoom = 90.0f;
    }
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front  = glm::normalize(front);
    // also re-calculate the Right and Up vector
    _right = glm::normalize(glm::cross(
        _front,
        _world_up)); // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    _up    = glm::normalize(glm::cross(_right, _front));
}
