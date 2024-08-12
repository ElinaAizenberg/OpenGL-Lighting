
#include <iostream>
#include <algorithm>
#include "portable-file-dialogs.h"
#include "../include/session.h"


void Session::loadCentralObject(const std::string& obj_filepath )
/** Loads a central object from a specified OBJ file into the session.
The object is initialized by loading its buffer data.*/
{
    central_objects_.clear();
    Object central_object = Object(obj_filepath, "../shaders/shader_central.vert", "../shaders/shader_central.frag");
    central_object.loadObjectBuffers();
    central_objects_.push_back(std::move(central_object));
}

void Session::loadCoordinateSystem()
/** Loads and initializes the coordinate system object for the session with predefined vertex and fragment shaders
for rendering the coordinate axes. */
{
    AxisObject axis = AxisObject("../shaders/shader_axis.vert", "../shaders/shader_axis.frag");
    axis.loadObjectBuffers();
    axis_objects_.push_back(std::move(axis));
}

void Session::addLightObject()
/** Adds a new light object to the session, provided the current number of light objects is less than 4.
It generates a unique pick color for object selection, and creates a new FlashLightObject using predefined shaders.
The new light object is initialized by loading its buffers.
If there are already 4 light objects, it shows a warning notification.*/
{
    if (light_objects_.size() < 4)
    {
        current_object_id_ = current_object_id_ + 1;
        generateNewPickColor_();

        auto pick_color_id = generatePickColorID_();
        auto new_object = FlashLightObject("../objects/Flashlight.obj", "../shaders/shader_flashlight.vert", "../shaders/shader_flashlight.frag", current_object_id_, pick_color_id,current_pick_color_[0], current_pick_color_[1], current_pick_color_[2]);

        new_object.loadObjectBuffers();
        light_objects_.push_back(std::move(new_object));
    }
    else
    {
        pfd::notify("System event", "The maximum amount of light sources is 4.",
                    pfd::icon::warning);
    }
}

void Session::removeLightObject(const std::string& id)
/** Defines index of the Light object to be removed in the vector light_objects_.*/
{
    for (int i = 0; i < light_objects_.size(); i++)
    {
        if (light_objects_[i].ObjectIdToString() == id)
        {
            id_to_remove_ = i;
            break;
        }
    }
}

void Session::drawSession(glm::mat4& view, glm::mat4& projection, glm::vec3& camera_position, bool get_pick_color)
/** Iterates through the vector of Light objects, central object and axis and applies member function to draw every object. */
{
    std::vector<Light> lights;
    // if Light object is On, include its data relating to light (position, direction, type, color etc) to the vector,
    // that is passed to the drawing function of the central object. It will be used in fragment shader of the central object.
    for (auto& light_obj: light_objects_)
    {
        if (light_obj.lightOnOff())
        {
            lights.push_back(light_obj.getLight());
        }
        light_obj.draw(view, projection, get_pick_color);
    }
    // draw central object
    for (auto& central_obj: central_objects_)
    {
        central_obj.draw(view, projection, camera_position, lights);
    }

    // draw coordinate system
    if (coordinate_system_)
    {
        for (auto& axis_obj: axis_objects_)
        {
            axis_obj.draw(view, projection);
        }
    }

    if (id_to_remove_ >= 0)
    {
        light_objects_.erase(light_objects_.begin() + id_to_remove_);
        id_to_remove_ = -1;
    }
}

int Session::getObjectIdByPickColor(const unsigned char *pick_color)
/** Calculates pick_color_id for a colour values (see formula above), iterates through the vector of Objects to get an id
of an Object with specified pick colour. */
{

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

int Session::generatePickColorID_()
/** Calculates pick color id using with the formula: R*256^2 + G*256^1 + B*256^0. Afterwards, pick_color_id is assigned
to an Object instance. */
{
    int pick_color_id = current_pick_color_[0] * (256 * 256) + current_pick_color_[1] * 256 +
                        current_pick_color_[2];
    return pick_color_id;
}

void Session::generateNewPickColor_()
/** Generates a new pick color by incrementing the RGB values by 10.
If a component reaches 255, it resets to 0 and increments the previous component. */
{

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


void Session::rotateObject(int object_id, float delta_x, float delta_y)
/** Iterates through the vector of Light objects and applies a member function
to update rotation angle of the corresponding Object. */
{
    if (object_id <= light_objects_.size())
    {
        light_objects_[object_id].rotateObject(delta_x, delta_y);
    }
}





