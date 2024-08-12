#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <iostream>

#include "../include/session.h"
#include "../include/drawing_lib.h"
#include "../include/gui.h"



int main() {

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Session session = Session();
    DrawingLib drawingLib = DrawingLib(session);
    Gui gui = Gui(session);


    GLFWwindow* window = drawingLib.createWindow();
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    drawingLib.defineCallbackFunction(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGui::StyleColorsDark();
    ImGui_ImplOpenGL3_CreateFontsTexture();

    session.loadCentralObject();
    session.loadCoordinateSystem();
    session.addLightObject();

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        gui.drawMainMenu();
        gui.drawObjectsPanels();

        // Check if ImGui wants to capture the mouse
        bool ioWantCaptureMouse = ImGui::GetIO().WantCaptureMouse;

        drawingLib.getWindowSize(window);
        drawingLib.drawScene(window, ioWantCaptureMouse);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
