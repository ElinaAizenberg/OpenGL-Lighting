
#ifndef PROJECT_3_OBJECT_H
#define PROJECT_3_OBJECT_H
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include "../include/shader.h"

// struct that contains lighting parameters for 2 types of light: point light and spotlight
struct Light {
    int type;
    float rgb[3];
    glm::vec3 light_pos;
    glm::vec3 light_dir;
    float intensity;

    float linear{0.09f};
    float quadratic{0.032f};

    float cutOff{10.5f};
    float outerCutOff{12.5};
};


class Object{
public:
    Object(const std::string& obj_filepath, const std::string& shader_vert, const std::string& shader_frag);
    virtual void loadObjectBuffers();
    virtual void draw(glm::mat4& view, glm::mat4& projection, glm::vec3 camera_position, std::vector<Light> lights);
    void loadObjectFile(const std::string& filepath);
    virtual float* getObjectColor(){return rgb_;}
    float& getScale(){return scale_;}

protected:
    struct Vertex
    {
        float x, y, z;
    };
    float rgb_[3] = {1,1,1};
    float scale_{1};

    std::vector<GLfloat> vertices_{};
    std::vector<GLfloat> normals_{};
    std::vector<GLuint> indices_{};

    GLuint VAO_{};
    GLuint VBO_{};
    GLuint NBO_{};
    GLuint EBO_{};
    ShaderProgram shaderProgram_;

    static std::vector<float> calculateNormalsSimple(std::vector<float> vertices);

};

class FlashLightObject : public Object {
public:
    FlashLightObject(const std::string& obj_filepath, const std::string& shader_vert, const std::string& shader_frag, int id, int pick_id=0, float pick_r=0, float pick_g=0, float pick_b=0);
    bool checkPickColor(int pick_color_id) const;
    void loadObjectBuffers() override;
    void draw(glm::mat4& view, glm::mat4& projection, bool get_pick_color = false);

    void rotateObject(float delta_x=0, float delta_y=0);

    std::string ObjectIdToString() const {return std::to_string(id_);};
    float* getObjectColor() override {return light_.rgb;}
    float* getObjectCoordinates(){return xyz_;}
    float* getObjectRotation(){return light_obj_params_[light_.type].frame_rotate_xy_;}

    Light& getLight();

    void switchGuiEnabled(){object_gui_ = !object_gui_; is_position_initialized_ = false;}
    bool& guiEnabled() {return object_gui_;}
    bool& positionInited() {return is_position_initialized_;}
    int& lightObjectType(){return light_.type;}
    bool& lightOnOff(){return lightOnOff_;}

    void reset();

private:
    struct LightObjParams{
        glm::vec3 scale;
        float initial_rotate;
        glm::vec3 rotate_vec;
        float frame_rotate_xy_[2] = {0,0};
    };

    Light light_ = Light({0, {1,0,1}, glm::vec3(0,0,0), glm::vec3(0,0,0), 1.0});
    int id_;
    int pick_id_;
    float pick_rgb_[3] = {0,0,0};

    bool lightOnOff_{true};

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

    LightObjParams flash_light_params_ = LightObjParams({glm::vec3(0.15,0.15,0.15), 90.0f, glm::vec3(1.0, 0.0, 0.0)});
    LightObjParams light_bulb_params_ = LightObjParams({glm::vec3(0.1,0.1,0.1), 180.0f, glm::vec3(1.0, 0.0, 0.0)});
    std::vector<LightObjParams> light_obj_params_ = {flash_light_params_, light_bulb_params_};

    float xyz_[3] = {0,3,0};

    glm::vec3 translate_vec_ = glm::vec3(0, 0, 0);
    bool object_gui_{false};
    bool is_position_initialized_{false};

    glm::mat4 getTranslationMatrix(bool initial_rotate = false);
    glm::vec3 getFlashLightDirection();
    glm::vec3 getFlashLightPosition();

};

class AxisObject : public Object {
public:
    AxisObject(const std::string& shader_vert, const std::string& shader_frag);
    void loadObjectBuffers() override;
    void draw(glm::mat4& view, glm::mat4& projection);

private:
    float axis_scale_{5.0f};
    glm::mat4 model_ = glm::mat4(1.0f);

    GLuint arrows_VBO_{};
    GLuint arrows_EBO_{};

    std::vector<GLfloat> arrows_vertices_;
    std::vector<GLuint> arrows_indices_;
};


#endif //PROJECT_3_OBJECT_H
