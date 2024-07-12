
#ifndef PROJECT_3_OBJECT_H
#define PROJECT_3_OBJECT_H
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include "../include/shader.h"

class Object{
public:
    Object(const std::string& obj_filepath, const std::string& shader_vert, const std::string& shader_frag);
    virtual void loadObjectBuffers();
    virtual void draw(glm::mat4& view, glm::mat4& projection);

protected:
    float rgb_[3] = {1,1,1};

    std::vector<GLfloat> vertices_{};
    std::vector<GLfloat> normals_{};
    std::vector<GLuint> indices_{};

    GLuint VAO_{};
    GLuint VBO_{};
    GLuint normals_buffer_object_{};
    GLuint EBO_{};

    glm::mat4 model_ = glm::mat4(1.0f);

    ShaderProgram shaderProgram_;

    void loadObjectFile(const std::string& filepath);

};

class FlashLightObject : public Object {
public:
    FlashLightObject(const std::string& obj_filepath, const std::string& shader_vert, const std::string& shader_frag, int id, int pick_id=0, float pick_r=0, float pick_g=0, float pick_b=0);
    bool checkPickColor(int pick_color_id) const;
    void loadObjectBuffers() override;
    void draw(glm::mat4& view, glm::mat4& projection, bool get_pick_color = false);

    void moveObject(float delta_x=0, float delta_y=0, float delta_z=0);
    void rotateObject(float delta_x=0, float delta_y=0);

    float* getObjectColor(){return rgb_;}
    float* getObjectCoordinates(){return xyz_;}
    float* getObjectRotation(){return rotate_xy_;}


    void switchGuiEnabled(){object_gui_ = !object_gui_; is_position_initialized_ = false;}

    bool& guiEnabled() {return object_gui_;}
    bool& positionInited() {return is_position_initialized_;}
    std::string ObjectIdToString() const {return std::to_string(id_);};

    void reset();





private:
    int id_;
    int pick_id_;
    float rgb_[3] = {1,0,1};
    float pick_rgb_[3] = {0,0,0};

    std::vector<GLfloat> arrow_vertices_{ -0.05f, 0.0f, -3.0f,
                                          0.05f, 0.0f, -3.0f,
                                          -0.05f, 0.0f, 8.0f,
                                          0.05f, 0.0f, 8.0f,

                                          -1.0f,  0.0f, 8.0f,
                                          0.0f, 0.1f, 9.0f,
                                          1.0f,  0.0f, 8.0f,
                                            };
    std::vector<GLuint> arrow_indices_{
            0,2,3,
            0,1,3,
            4,5,6
    };

    GLuint arrow_VAO_{};
    GLuint arrow_VBO_{};
    GLuint arrow_EBO_{};

    float scale_{0.15};
    float xyz_[3] = {0,3,0};
    float rotate_xy_[2] = {0,0};

    glm::vec3 translate_vec = glm::vec3(0, 0, 0);
    glm::vec3 rotate_vec = glm::vec3(1.0, 0.0, 0.0);

    bool object_gui_{false};
    bool is_position_initialized_{false};



};

class GridObject : public Object {
public:
    GridObject(const std::string& shader_vert, const std::string& shader_frag);
    void loadObjectBuffers() override;
    void draw(glm::mat4& view, glm::mat4& projection) override;

private:
    float grid_scale_{5.0f};

    GLuint arrows_VBO_{};
    GLuint arrows_EBO_{};

    std::vector<GLfloat> arrows_vertices_;
    std::vector<GLuint> arrows_indices_;

};


#endif //PROJECT_3_OBJECT_H
