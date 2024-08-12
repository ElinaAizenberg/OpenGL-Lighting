#ifndef PROJECT_3_SESSION_H
#define PROJECT_3_SESSION_H
#include "../include/object.h"

class Session{
public:
    Session() = default;
    void loadCentralObject(const std::string& obj_filepath = "../objects/sphere.obj");
    void loadCoordinateSystem();
    void addLightObject();
    void removeLightObject(const std::string& id);
    void rotateObject(int object_id, float delta_x=0, float delta_y=0);
    int getObjectIdByPickColor(const unsigned char* pick_color);

    void drawSession(glm::mat4& view, glm::mat4& projection, glm::vec3& camera_position, bool get_pick_color = false);

    std::vector<FlashLightObject>& getFlashLightObjects(){return light_objects_;};
    bool& coordinate_system(){return coordinate_system_;}
    Object& getCentralObject(){return central_objects_[0];}


private:
    int current_object_id_{0};
    int current_pick_color_[3]{0, 0, 0};
    int id_to_remove_{-1};
    bool coordinate_system_{true};

    std::vector<Object> central_objects_;
    std::vector<FlashLightObject> light_objects_;
    std::vector<AxisObject> axis_objects_;

    int generatePickColorID_();
    void generateNewPickColor_();

};

#endif //PROJECT_3_SESSION_H
