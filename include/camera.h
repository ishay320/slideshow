#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
static const float CAMERA_YAW         = -90.0f;
static const float CAMERA_PITCH       = 0.0f;
static const float CAMERA_SPEED       = 2.5f;
static const float CAMERA_SENSITIVITY = 0.1f;
static const float CAMERA_ZOOM        = 45.0f;

class Camera
{
public:
    /**
     * @brief Construct a new Camera object by vectors
     *
     * @param position
     * @param up
     * @param yaw
     * @param pitch
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = CAMERA_YAW,
           float pitch = CAMERA_PITCH);

    /**
     * @brief Construct a new Camera object by scalar values
     *
     * @param posX
     * @param posY
     * @param posZ
     * @param upX
     * @param upY
     * @param upZ
     * @param yaw
     * @param pitch
     */
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    /**
     * @brief Get the View Matrix calculated using Euler Angles and the LookAt Matrix
     *
     * @return glm::mat4
     */
    glm::mat4 getViewMatrix();

    /**
     * @brief Get the Projection Matrix
     *
     * @param screen_width
     * @param screen_height
     * @return glm::mat4
     */
    glm::mat4 getProjectionMatrix(float screen_width, float screen_height);

    /**
     * @brief Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it
     * from windowing systems)
     *
     * @param direction
     * @param deltaTime
     */
    void processKeyboard(Camera_Movement direction, float deltaTime);

    /**
     * @brief Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
     *
     * @param x_offset
     * @param y_offset
     * @param constrain_pitch
     */
    void processMouseMovement(float x_offset, float y_offset, bool constrain_pitch = true);

    /**
     * @brief Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
     *
     * @param y_offset
     */
    void processMouseScroll(float y_offset);

private:
    /**
     * @brief calculates the front vector from the Camera's (updated) Euler Angles
     *
     */
    void updateCameraVectors();

    // camera Attributes
    glm::vec3 _position;
    glm::vec3 _front;
    glm::vec3 _up;
    glm::vec3 _right;
    glm::vec3 _world_up;

    // euler Angles
    float _yaw;
    float _pitch;

    // camera options
    float _movement_speed;
    float _mouse_sensitivity;
    float _zoom;
};

#endif // CAMERA_H