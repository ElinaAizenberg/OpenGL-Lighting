#ifndef PROJECT_3_DRAWING_LIB_H
#define PROJECT_3_DRAWING_LIB_H

#include <GLFW/glfw3.h>
#include "../include/session.h"
#include "../include/camera.h"

class DrawingLib{
public:
    explicit DrawingLib(Session& session): session_(session){};
    GLFWwindow* createWindow() const;
    void getWindowSize(GLFWwindow* window);
    void defineCallbackFunction(GLFWwindow* window);

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    void scrollCallback(GLFWwindow* window, double yoffset);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void drawScene(GLFWwindow* window, bool imGuiCaptureMouse);


private:
    Session& session_;
    bool imgui_capture_mouse_{false};

    int window_width_{1920};
    int window_height_{1080};

    bool get_color_{false};
    bool left_button_down_{false};
    bool right_button_down_{false};
    bool ctrl_key_{false};

    int selected_object_id_{-1};
    double cursor_pos_x_{0}, cursor_pos_y_{0};
    double current_pos_x_{0}, current_pos_y_{0}, prev_pos_x_{0}, prev_pos_y_{0};

    double last_click_time_{0.0};
    const double DOUBLE_CLICK_TIME{0.25}; // 250 ms
    bool left_double_click_{false};

    DomeCamera dome_camera_ = DomeCamera(glm::vec3(0.0f, 1.0f, 10.0), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    std::tuple<double, double> calculateCoordinatesOnMouseMove(int correction_factor) const;

};

#endif //PROJECT_3_DRAWING_LIB_H
