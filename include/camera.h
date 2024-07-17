#ifndef PROJECT_3_CAMERA_H
#define PROJECT_3_CAMERA_H

#include <glm/glm.hpp>


class DomeCamera{
public:
    DomeCamera(glm::vec3 camera_position, glm::vec3 target_position, glm::vec3 up_direction);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix(float window_width, float window_height) const;

    void zoom(float yoffset);
    void rotate(float delta_x=0, float delta_y=0, float delta_z = 0);
    glm::vec3& cameraPosition(){return camera_position_;}

private:
    glm::mat4 view_          = glm::mat4(1.0f);


    glm::vec3 camera_position_{};
    glm::vec3 target_position_{};
    glm::vec3 up_direction_{};

    float near_{0.1};
    float far_{100};
    float fov_{70};

    float yaw_   = glm::radians(-135.0f);
    float pitch_ = glm::radians(-25.0f);
    float roll_ = glm::radians(0.0f);

    double radius_;

    void updateCameraVectors();
};

#endif //PROJECT_3_CAMERA_H
