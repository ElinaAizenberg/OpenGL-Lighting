
#include <fstream>
#include <sstream>
#include "imgui.h"
#include "../include/gui.h"
#include "portable-file-dialogs.h"


void Gui::drawMainMenu() {

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {

            if (ImGui::MenuItem("Load central object")) {
                openFile();
            }
            if (ImGui::MenuItem("Add light source")) {
                session_.addLightObject();
            }
            if (ImGui::MenuItem("Help"))
            {
                help_window_ = true;
            }
            if (ImGui::MenuItem("Exit"))
            {
                exitConfirmMessage();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    if (help_window_){
        drawHelpWindow();
    }

}

void Gui::drawObjectsPanels()
{
    auto& flash_light_objects = session_.getFlashLightObjects();
    for (auto & object : flash_light_objects)
    {
        drawIndividualPanel(object);
    }
}

void Gui::drawIndividualPanel(FlashLightObject &object) const{
    auto& gui_enabled = object.guiEnabled();
    auto& position_initialized = object.positionInited();

    if (gui_enabled)
    {
        ImGui::SetNextWindowSize(ImVec2(object_panel_width_, object_panel_height_));
        if (!position_initialized)
        {
            ImGui::SetNextWindowPos(ImGui::GetMousePos());
            position_initialized = true;
        }

        std::string gui_window_name = "Panel: Flashlight " + object.ObjectIdToString();
        ImGui::Begin(gui_window_name.c_str(), &gui_enabled, ImGuiWindowFlags_NoResize);


        auto col = object.getObjectColor();
        std::string edit_col_name = "##individual fill color_" + object.ObjectIdToString();
        ImGui::ColorEdit3(edit_col_name.c_str(), col);
        ImGui::Spacing();

        ImGui::SeparatorText("Light position");
        auto xyz = object.getObjectCoordinates();

        ImGui::SliderFloat("##x_slider", &xyz[0], -10.0f, 10.0f, "x = %.2f");
        ImGui::SliderFloat("##y_slider", &xyz[1], -10.0f, 10.0f, "y = %.2f");
        ImGui::SliderFloat("##z_slider", &xyz[2], -10.0f, 10.0f, "z = %.2f");

        ImGui::Spacing();

        ImGui::SeparatorText("Light angle");
        auto rotate_xy = object.getObjectRotation();
        ImGui::SliderFloat("##angle_x_slider", &rotate_xy[0], -90.0f, 90.0f, "x = %.2f");
        ImGui::SliderFloat("##angle_y_slider", &rotate_xy[1], -90.0f, 90.0f, "y = %.2f");

        std::string button_name = "Reset rotation##" + object.ObjectIdToString();
        if (ImGui::Button(button_name.c_str())){
            object.reset();
        }

        ImGui::End();
    }
}

void Gui::openFile() {
    auto selection = pfd::open_file("Select a file", ".",
                                    { "Object Files", "*.obj"}).result();
    if (!selection.empty())
    {
        session_.loadCentralObject(selection[0]);
    }
    else
    {
        pfd::notify("System event", "An .obj file was not selected.",
                    pfd::icon::info);
    }
}

void Gui::exitConfirmMessage() {
    auto button = pfd::message("Action requested", "Are you sure you want to exit?",
                               pfd::choice::yes_no, pfd::icon::question).result();

    switch (button)
    {
        case pfd::button::yes:
            exit(0);
        case pfd::button::no:
            break;
    }
}

void Gui::drawHelpWindow() {
    ImGui::Begin("Help", &help_window_);
    ImGui::TextWrapped("%s", readme_txt_.c_str());
    ImGui::End();


}

std::string Gui::readTextFile(const std::string &filePath)
/** Reads the contents of a text file into a string. */
{
    std::ifstream file(filePath);
    std::stringstream buffer;

    if (file.is_open())
    {
        buffer << file.rdbuf();
        file.close();
    } else {
        return "Failed to open the file.";
    }

    return buffer.str();
}