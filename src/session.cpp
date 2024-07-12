
#include <iostream>
#include "../include/session.h"


void Session::loadCentralObject(const std::string& obj_filepath ) {
    central_objects_.clear();
    Object central_object = Object(obj_filepath, "../shaders/shader_central.vert", "../shaders/shader_central.frag");

    central_object.loadObjectBuffers();
    central_objects_.push_back(std::move(central_object));


}
void Session::loadGrid() {
    GridObject grid = GridObject("../shaders/shader_grid.vert", "../shaders/shader_grid.frag");
    grid.loadObjectBuffers();
    grid_objects_.push_back(std::move(grid));
}

void Session::addLightObject() {
    current_object_id_ = current_object_id_ + 1;
    generateNewPickColor_();

    auto pick_color_id = generatePickColorID_();
    auto new_object = FlashLightObject("/home/elina/MyProjects/object_files/Flashlight.obj", "../shaders/shader_flashlight.vert", "../shaders/shader_flashlight.frag", current_object_id_, pick_color_id,current_pick_color_[0], current_pick_color_[1], current_pick_color_[2]);

    new_object.loadObjectBuffers();
    light_objects_.push_back(std::move(new_object));

}

void Session::removeLightObject(int id) {
    if (light_objects_.size() > (id))
    {
        light_objects_.erase(light_objects_.begin() + (id));
    }

}

void Session::drawSession(glm::mat4& view, glm::mat4& projection, bool get_pick_color) {
    for (auto& central_obj: central_objects_){
        central_obj.draw(view, projection);
    }
    for (auto& grid_obj: grid_objects_){
        grid_obj.draw(view, projection);
    }
    for (auto& light_obj: light_objects_){
        light_obj.draw(view, projection, get_pick_color);
    }
}

int Session::getObjectIdByPickColor(const unsigned char *pick_color) {

    int pick_color_id = pick_color[0] * (256 * 256) + pick_color[1] * 256 + pick_color[2];

    for (std::vector<FlashLightObject>::size_type i = 0; i < light_objects_.size(); i++)
    {
        if (light_objects_[i].checkPickColor(pick_color_id))
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int Session::generatePickColorID_() {
    int pick_color_id = current_pick_color_[0] * (256 * 256) + current_pick_color_[1] * 256 +
                        current_pick_color_[2];
    return pick_color_id;
}

void Session::generateNewPickColor_() {

    for (int i = 2; i >= 0; --i)
    {
        if (current_pick_color_[i] < 255)
        {
            current_pick_color_[i] += 10;
            return;
        }
        if (current_pick_color_[i] == 255)
        {
            current_pick_color_[i] = 0;
            current_pick_color_[i - 1]++;
            return;
        }
    }
}

void Session::moveObject(int object_id, float delta_x, float delta_y, float delta_z) {
    if (object_id <= light_objects_.size()){
        light_objects_[object_id].moveObject(delta_x, delta_y, delta_z);
    }
}

void Session::rotateObject(int object_id, float delta_x, float delta_y) {
    if (object_id <= light_objects_.size()){
        light_objects_[object_id].rotateObject(delta_x, delta_y);
    }
}





