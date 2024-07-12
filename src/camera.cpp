
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

glm::mat4 DomeCamera::getViewMatrix() {
    return glm::lookAt(camera_position_, target_position_, up_direction_);
}

glm::mat4 DomeCamera::getProjectionMatrix(float window_width, float window_height) const {
    return glm::perspective(glm::radians(fov_), static_cast<float>(window_width) / static_cast<float>(window_height), near_, far_);
}

void DomeCamera::zoom(float yoffset) {
    fov_ -= yoffset;
    if (fov_ < 10.0f)
        fov_ = 10.0f;
    if (fov_ > 100.0f)
        fov_ = 100.0f;
}

void DomeCamera::rotate(float delta_x, float delta_y, float delta_z) {
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
{
    // Compute the camera's front direction
    glm::vec3 front;
    front.x = cos(yaw_) * cos(pitch_);
    front.y = sin(pitch_);
    front.z = sin(yaw_) * cos(pitch_);
    front = glm::normalize(front);

    // Compute the right direction
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    // Compute the new up vector with the roll angle
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    // Create the rotation matrix around the front direction (z-axis rotation)
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), roll_, front);

    // Apply the rotation to the up vector
    up = glm::vec3(rotation * glm::vec4(up, 1.0f));

    // Update the camera position
    camera_position_ = target_position_ - front * static_cast<float>(radius_);

    // Update the up direction
    up_direction_ = up;
}