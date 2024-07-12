#ifndef PROJECT_3_SESSION_H
#define PROJECT_3_SESSION_H
#include "../include/object.h"

class Session{
public:
    Session() = default;
    void loadCentralObject(const std::string& obj_filepath = "/home/elina/MyProjects/object_files/multiple_objects.obj");
    void loadGrid();
    void addLightObject();
    void removeLightObject(int id);


    void drawSession(glm::mat4& view, glm::mat4& projection, bool get_pick_color = false);

    int getObjectIdByPickColor(const unsigned char* pick_color);

    void moveObject(int object_id, float delta_x=0, float delta_y=0, float delta_z=0);
    void rotateObject(int object_id, float delta_x=0, float delta_y=0);

    std::vector<FlashLightObject>& getFlashLightObjects(){return light_objects_;};


private:
    int current_object_id_{0};
    int current_pick_color_[3]{0, 0, 0};

    std::vector<Object> central_objects_;
    std::vector<FlashLightObject> light_objects_;
    std::vector<GridObject> grid_objects_;


    int generatePickColorID_();
    void generateNewPickColor_();

   // Object grid_object_;


};

#endif //PROJECT_3_SESSION_H
