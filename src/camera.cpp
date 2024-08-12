
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "../include/camera.h"

DomeCamera::DomeCamera(glm::vec3 camera_position, glm::vec3 target_position, glm::vec3 up_direction) {
    camera_position_ = camera_position;
    target_position_ = target_position;
    up_direction_ = up_direction;

    radius_ = camera_position_.z;
    updateCameraVectors();
}

glm::mat4 DomeCamera::getViewMatrix()
/** Constructs the view matrix using the camera's position, target position, and up direction. */
{
    // View matrix transforms world coordinates to view space.
    // View space is what usually referred to as the camera of OpenGL.
    return glm::lookAt(camera_position_, target_position_, up_direction_);
}

glm::mat4 DomeCamera::getProjectionMatrix(float window_width, float window_height) const
/** Constructs the projection matrix using window height, width and field of view (fov)
that sets how large the viewspace is.*/
{
    // Projection matrix converts coordinates within this specified range (e.g.(-1000 and 1000) in each dimension) to
    // normalized device coordinates (-1.0, 1.0). Then transform view coordinates to clip coordinates in orthographic or
    // perspective form.
    return glm::perspective(glm::radians(fov_), static_cast<float>(window_width) / static_cast<float>(window_height), near_, far_);
}

void DomeCamera::zoom(float yoffset)
/** Adjusts the camera's field of view (fov) value to zoom in or out based on the provided zooming factor.
 The larger field of view, the smaller object will look.*/
{
    fov_ -= yoffset;
    if (fov_ < 10.0f)
        fov_ = 10.0f;
    if (fov_ > 100.0f)
        fov_ = 100.0f;
}

void DomeCamera::rotate(float delta_x, float delta_y, float delta_z)
/** Adjusts the camera's yaw, pitch, roll angles, ensuring the pitch stays within a safe range to avoid flipping,
then recalculates the camera's position by calling `updateCameraVectors`. */
{
    yaw_ += delta_x;
    pitch_ -= delta_y;
    roll_ += delta_z;

    // Limit the pitch angle to avoid flipping
    if (pitch_ > glm::radians(89.0)) {
        pitch_ = glm::radians(89.0);
    }
    if (pitch_ < glm::radians(-89.0)) {
        pitch_ = glm::radians(-89.0);
    }
    updateCameraVectors();
}

void DomeCamera::updateCameraVectors()
/** Updates 2 vectors that define camera system: camera position and up direction using Euler angles. */
{
    // Compute the camera's front direction using Euler angles
    glm::vec3 front;
    front.x = cos(yaw_) * cos(pitch_);
    front.y = sin(pitch_);
    front.z = sin(yaw_) * cos(pitch_);
    front = glm::normalize(front);

    // A way to calculate camera_direction vector:
    // glm::vec3 cameraDirection = glm::normalize(camera_position_ - target_position_);

    // Compute the right direction
    // Cross product is a method of multiplying two vectors that produces a vector perpendicular to both vectors involved.
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Compute the new up vector with the roll angle
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    // Create the rotation matrix around the front direction (z-axis rotation)
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), roll_, front);

    // Apply the rotation to the up vector
    up = glm::vec3(rotation * glm::vec4(up, 1.0f));

    // Update the camera position. Front vector was normalized, so we need to multiply it with radius to move it away from objetc.
    camera_position_ = target_position_ - front * static_cast<float>(radius_);

    // Update the up direction
    up_direction_ = up;
}