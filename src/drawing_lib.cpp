#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include <tuple>

#include "../include/drawing_lib.h"

GLFWwindow *DrawingLib::createWindow() const
/** Creates and returns a new GLFW window with the specified width, height, and title. */
{
    return glfwCreateWindow(window_width_, window_height_, "OpenGL Project 3", nullptr, nullptr);
}

void DrawingLib::getWindowSize(GLFWwindow *window)
/** Retrieves the size of the specified GLFW window and updates the class variables for width, height, and dimension ratio. */
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    window_width_  = w;
    window_height_ = h;
}

void DrawingLib::drawScene(GLFWwindow *window, bool imGuiCaptureMouse)
/**  Manages the rendering pipeline and interaction handling for the graphical scene using OpenGL and ImGui. */
{
    imgui_capture_mouse_ = imGuiCaptureMouse;

    glViewport(0, 0, window_width_, window_height_);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // projection and view matrices are passed to every object and will be used in vertex shaders to calculate glPosition of vertices.
    auto projection_mat = dome_camera_.getProjectionMatrix(static_cast<float>(window_width_), static_cast<float>(window_height_));
    auto view_mat = dome_camera_.getViewMatrix();

    if (get_color_)
    {
        double x_coord, y_coord;
        unsigned char color[3] = {0};
        // When get_color_ is true, Objects are drawn with pick_colors (not general colors), and every Object has a unique pick_color,
        // that allows to identify Object id.
        session_.drawSession(view_mat, projection_mat, dome_camera_.cameraPosition(), get_color_);

        glfwGetCursorPos(window, &x_coord, &y_coord);
        glReadPixels(x_coord, window_height_ - y_coord, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);
        selected_object_id_ = session_.getObjectIdByPickColor(color);
        if (left_double_click_)
        {
            if (selected_object_id_ >= 0)
            {
                session_.getFlashLightObjects()[selected_object_id_].switchGuiEnabled();
            }
            left_double_click_ = false;
        }
    }
    else
    {
        // draw frame with regular colours
        session_.drawSession(view_mat, projection_mat, dome_camera_.cameraPosition(), get_color_);
    }
    ImGui::Render(); // Finalizes the ImGui frame and prepares the draw data for rendering.
    // Renders the compiled ImGui draw data using the OpenGL 3 backend.
    // Takes the draw data and issues the necessary OpenGL commands to display the ImGui interface.
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (!get_color_)
    {
        // Swaps the front and back buffers of the specified window.
        // In double-buffered mode, rendering is done to the back buffer while the front buffer is displayed on the screen.
        // Buffers should be swapped only when Objects are drawn with regular colors (not pick_colors).
        glfwSwapBuffers(window);
    }
    get_color_ = false;
    glfwPollEvents();
}

void DrawingLib::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
/** Handles mouse button events in a GLFW window. If the cursor position is not on any of ImGui elements,
it performs actions on left-click, double left-click and right-click. */
{
    if (!imgui_capture_mouse_)
    {
        double currentTime = glfwGetTime();
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            // get_color_ boolean enables part of the code that reads pixels in cursor position and defines if you interact with Objects
            get_color_ = true;
            if (currentTime - last_click_time_ < DOUBLE_CLICK_TIME)
            {
                // Reset last_click_time_ to avoid detecting triple clicks as double clicks
                last_click_time_ = 0.0;
                left_double_click_ = true;
            }
            else{
                last_click_time_ = currentTime;
                left_button_down_ = true;
            }
        }

        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            get_color_ = true;
            right_button_down_ = true;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            left_button_down_   = false;
            selected_object_id_ = -1;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            right_button_down_ = false;
            selected_object_id_ = -1;
        }
    }

}

void DrawingLib::cursorPositionCallback(GLFWwindow* window,
                                        double input_cursor_pos_x,
                                        double input_cursor_pos_y)
/** Handles cursor movement events in a GLFW window.*/
{
    prev_pos_x_    = current_pos_x_;
    prev_pos_y_    = current_pos_y_;
    current_pos_x_ = input_cursor_pos_x;
    current_pos_y_ = input_cursor_pos_y;
    auto delta_coordinates = calculateCoordinatesOnMouseMove(10);

    if (left_button_down_ && selected_object_id_ >= 0)
    {

    }
    if (right_button_down_ && selected_object_id_ >= 0)
    {
        session_.rotateObject(selected_object_id_,std::get<0>(delta_coordinates), std::get<1>(delta_coordinates) );
    }
    if (left_button_down_ && selected_object_id_ < 0)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS){
            dome_camera_.rotate(0, 0,std::get<0>(delta_coordinates));
        }
        else {
            dome_camera_.rotate(std::get<0>(delta_coordinates), std::get<1>(delta_coordinates), 0);
        }

    }
}

void DrawingLib::scrollCallback(GLFWwindow* window, double yoffset)
/** Callback function that handles scroll input from the mouse wheel to zoom in or out of the scene.
If ImGui is not capturing the mouse input and the zoom level is adjusted based on the scroll direction. */
{
    if (!imgui_capture_mouse_){
        dome_camera_.zoom(float(yoffset));
    }
}

void DrawingLib::defineCallbackFunction(GLFWwindow* window)
/** Sets up callback functions for a GLFW window.*/
{
    // Sets a custom user-defined pointer (current instance of DrawingLib) to be associated with the GLFW window.
    // It allows the instance's methods to be easily accessed within the GLFW callbacks, enabling event handling
    // (mouse clicks, cursor movements, and scrolling) to interact with the DrawingLib instance.
    glfwSetWindowUserPointer(window, this);

    glfwSetMouseButtonCallback(window, [](GLFWwindow* win, int button, int action, int mods) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->mouseButtonCallback(win, button, action, mods);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* win, double xpos, double ypos) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->cursorPositionCallback(win, xpos, ypos);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* win, double xoffset, double yoffset) {
        auto* drawing_lib = static_cast<DrawingLib*>(glfwGetWindowUserPointer(win));
        drawing_lib->scrollCallback(win, yoffset);
    });
}

std::tuple<double, double> DrawingLib::calculateCoordinatesOnMouseMove(int correction_factor) const
/** Calculates the change in object coordinates of mouse cursor, converting screen space coordinates to
 normalized device coordinates (NDC) and then to frustum coordinates, with depth correction applied. */
{
    // Convert the mouse movement (delta_x and delta_y) from screen space to normalized device coordinates (NDC)
    // where the range is [-1, 1].
    double ndc_delta_x = (current_pos_x_ - prev_pos_x_) / (window_width_ / 2.0f);
    double ndc_delta_y = (current_pos_y_ - prev_pos_y_) / (window_height_ / 2.0f);

    // Compute the width and height of the frustum at the near plane
    float dim_ratio = static_cast<float>(window_height_) / static_cast<float>(window_width_);
    double frustum_width = 1 - (-1);
    double frustum_height = (1 * dim_ratio) - (-1 * dim_ratio);

    // Scale normalized deltas to match the frustum coordinates
    double adjusted_delta_x = ndc_delta_x * frustum_width / 2.0f;
    double adjusted_delta_y = ndc_delta_y * frustum_height / 2.0f;

    // Apply depth correction factor (assuming linear scaling for simplicity)
    adjusted_delta_x *= correction_factor;
    adjusted_delta_y *= correction_factor;

    std::tuple<double, double> delta_coordinates = std::make_tuple(adjusted_delta_x, adjusted_delta_y);

    return delta_coordinates;
}