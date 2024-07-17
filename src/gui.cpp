
#include <fstream>
#include <sstream>
#include "imgui.h"
#include "../include/gui.h"
#include "portable-file-dialogs.h"
#include "imgui_toggle.h"


void Gui::drawMainMenu() {

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {

            if (ImGui::MenuItem("Load central object")) {
                openFile();
            }
            if (ImGui::MenuItem("Add light source")) {
                session_.addLightObject();
            }

            if (ImGui::BeginMenu("Central object")){
                auto col = session_.getCentralObject().getObjectColor();
                ImGui::ColorEdit3("color", col);
                ImGui::Spacing();

                ImGui::SliderFloat("scale", &session_.getCentralObject().getScale(), 0.1, 10.0f, "x = %.1f");

                ImGui::EndMenu();
            }
            ImGui::MenuItem("Grid", nullptr, &session_.grid());

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
    auto& light = object.getLight();

    if (gui_enabled)
    {
        ImGui::SetNextWindowSize(ImVec2(object_panel_width_, object_panel_height_));
        if (!position_initialized)
        {
            ImGui::SetNextWindowPos(ImGui::GetMousePos());
            position_initialized = true;
        }

        std::string gui_window_name = "Panel: light " + object.ObjectIdToString();
        ImGui::Begin(gui_window_name.c_str(), &gui_enabled, ImGuiWindowFlags_NoResize);

        std::string toggle_name = "Turn on/off##" + object.ObjectIdToString();
        ImGui::Toggle(toggle_name.c_str(), &object.lightOnOff(), ImGuiToggleFlags_Animated);

        if (ImGui::RadioButton("Spotlight", &object.lightObjectType(), 0)){
            object.loadObjectFile("/home/elina/MyProjects/object_files/Flashlight.obj");
            object.loadObjectBuffers();
        }
        if (ImGui::RadioButton("Point light", &object.lightObjectType(), 1)){
            object.loadObjectFile("/home/elina/MyProjects/object_files/LightBulb.obj");
            object.loadObjectBuffers();
        }
        ImGui::Spacing();

        ImGui::SeparatorText("Light colour");
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
        if (object.lightObjectType() == 0){
            ImGui::SeparatorText("Light angle");
            auto rotate_xy = object.getObjectRotation();
            ImGui::SliderFloat("##angle_x_slider", &rotate_xy[0], -90.0f, 90.0f, "pitch = %.2f");
            ImGui::SliderFloat("##angle_y_slider", &rotate_xy[1], -90.0f, 90.0f, "roll = %.2f");

            std::string button_name = "Reset rotation##" + object.ObjectIdToString();
            if (ImGui::Button(button_name.c_str())){
                object.reset();
            }
        }

        ImGui::SeparatorText("Light parameters");
        if (object.lightObjectType() == 0){
            ImGui::SliderFloat("##cutOff", &light.cutOff, 0.0f, light.outerCutOff, "cutOff = %.1f");
            ImGui::SliderFloat("##outerCutOff", &light.outerCutOff, light.cutOff, 30.0f, "outerCutOff = %.1f");
        }
        if (object.lightObjectType() == 1){
            ImGui::SliderFloat("##linear", &light.linear, 0.0f, 1.0f, "linear = %.2f");
            ImGui::SliderFloat("##quadratic", &light.quadratic, 0.0f, 2.0f, "quadratic = %.2f");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        std::string button_name_delete = "Remove light##" + object.ObjectIdToString();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 , 0.6f, 0.6f));
        if (ImGui::Button(button_name_delete.c_str())){
            session_.removeLightObject(object.ObjectIdToString());
        }
        ImGui::PopStyleColor();
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