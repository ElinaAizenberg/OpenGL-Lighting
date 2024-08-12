cmake_minimum_required(VERSION 3.10)
project(project_3)

set(CMAKE_CXX_STANDARD 14)

# Set policy to suppress the OpenGL warning
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 OLD)
endif()

set(GLAD /path_to_directory/libs/glad)
set(EXTERNAL_LIB_DIR /path_to_directory/libs)

# Define the path to the central ImGui directory
set(IMGUI_DIR ${EXTERNAL_LIB_DIR}/imgui)

# Add source files from the project
set(PROJECT_SRC
        src/main.cpp
        src/session.cpp
        src/object.cpp
        src/drawing_lib.cpp
        src/camera.cpp
        src/loader.cpp
        src/shader.cpp
        src/gui.cpp
)

# Add ImGui source files
set(IMGUI_SRC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${EXTERNAL_LIB_DIR}/tiny_obj_loader/tiny_obj_loader.cc
        ${EXTERNAL_LIB_DIR}/imgui_toggle/imgui_toggle.cpp
        ${EXTERNAL_LIB_DIR}/imgui_toggle/imgui_toggle_palette.cpp
        ${EXTERNAL_LIB_DIR}/imgui_toggle/imgui_toggle_presets.cpp
        ${EXTERNAL_LIB_DIR}/imgui_toggle/imgui_toggle_renderer.cpp
)


# Add ImGui source files
set(GLAD_SRC
        ${GLAD}/src/glad.c
)

include_directories(include)

include_directories(${GLAD}/include
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${EXTERNAL_LIB_DIR}/imgui_pfd
        ${EXTERNAL_LIB_DIR}/imgui_toggle
        ${EXTERNAL_LIB_DIR}/tiny_obj_loader)


find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED CONFIG)

add_executable(${PROJECT_NAME} ${PROJECT_SRC} ${GLAD_SRC} ${IMGUI_SRC})
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw dl)