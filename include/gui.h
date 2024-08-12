#ifndef PROJECT_3_GUI_H
#define PROJECT_3_GUI_H

#include "../include/session.h"

class Gui {
public:
    explicit Gui(Session& session) : session_(session){readme_txt_ = readTextFile("../docs/ReadMe.txt");};
    void drawMainMenu();
    void drawObjectsPanels();

private:
    Session& session_;
    bool help_window_{false};
    std::string readme_txt_;

    float object_panel_height_ = 210.0f;
    float object_panel_width_ = 230.0f;

    static std::string readTextFile(const std::string &filePath);
    void drawHelpWindow();
    static void exitConfirmMessage();
    void openFile();
    void drawIndividualPanel(FlashLightObject &object) const;
};


#endif //PROJECT_3_GUI_H
