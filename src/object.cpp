#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

#include "../include/object.h"
#include "../include/loader.h"

Object::Object(const std::string& obj_filepath, const std::string& shader_vert, const std::string& shader_frag): shaderProgram_(shader_vert.c_str(), shader_frag.c_str()) {
    loadObjectFile(obj_filepath);

    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &NBO_);
    glGenBuffers(1, &EBO_);
}


void Object::loadObjectFile(const std::string &filepath) {
    if (filepath.empty()){
        return;
    }

    vertices_.clear();
    normals_.clear();
    indices_.clear();

    try{
        ObjectLoader::loadObjFileData(filepath, vertices_, normals_, indices_);
    }
    catch(...) {
        std::cerr << "Error: Unable to load file: " << filepath;
        return;
    }
    if (normals_.empty() || normals_.size() != vertices_.size()){
        normals_ = calculateNormalsSimple(vertices_);
    }
}

void Object::loadObjectBuffers()
{
    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLuint) * indices_.size(),
                 indices_.data(),
                 GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, NBO_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * normals_.size(),
                 normals_.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}

void Object::draw(glm::mat4& view, glm::mat4& projection, glm::vec3 camera_position, std::vector<Light> lights) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    int numLights = static_cast<int>(lights.size());

    shaderProgram_.use();
    shaderProgram_.setInt("numLights", numLights);

    shaderProgram_.setVec3("objectColor", rgb_[0], rgb_[1], rgb_[2]);
    shaderProgram_.setVec3("viewPos", camera_position);


    for (int i = 0; i < numLights; ++i) {
        std::string typeName = "lightType[" + std::to_string(i) + "]";
        shaderProgram_.setInt(typeName, lights[i].type);

        std::string posName = "lightPos[" + std::to_string(i) + "]";
        shaderProgram_.setVec3(posName, lights[i].light_pos);

        std::string dirName = "lightDir[" + std::to_string(i) + "]";
        shaderProgram_.setVec3(dirName, lights[i].light_dir);

        std::string colorName = "lightColor[" + std::to_string(i) + "]";
        shaderProgram_.setVec3(colorName, lights[i].rgb[0], lights[i].rgb[1], lights[i].rgb[2]);

        std::string intensityName = "lightIntensity[" + std::to_string(i) + "]";
        shaderProgram_.setFloat(intensityName, lights[i].intensity);

        std::string linearName = "linear[" + std::to_string(i) + "]";
        shaderProgram_.setFloat(linearName, lights[i].linear);

        std::string quadraticName = "quadratic[" + std::to_string(i) + "]";
        shaderProgram_.setFloat(quadraticName, lights[i].quadratic);

        std::string cutOffName = "cutOff[" + std::to_string(i) + "]";
        shaderProgram_.setFloat(cutOffName, glm::cos(glm::radians(lights[i].cutOff)));

        std::string outerCutOffName = "outerCutOff[" + std::to_string(i) + "]";
        shaderProgram_.setFloat(outerCutOffName, glm::cos(glm::radians(lights[i].outerCutOff)));
    }
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(scale_, scale_, scale_));

    shaderProgram_.setMat4("projection", projection);
    shaderProgram_.setMat4("view", view);
    shaderProgram_.setMat4("model", model);


    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, 0);

}

std::vector<float> Object::calculateNormalsSimple(std::vector<float> vertices) {
    std::vector<float> normals;
    Vertex center     = {0.0f, 0.0f, 0.0f};
    int num_vertices  = static_cast<int>(vertices.size()) / 3;
    int vertices_size = static_cast<int>(vertices.size());

    for (int i = 0; i < vertices_size; i += 3)
    {
        center.x += vertices[i];
        center.y += vertices[i + 1];
        center.z += vertices[i + 2];
    }

    center.x /= num_vertices;
    center.y /= num_vertices;
    center.z /= num_vertices;

    for (int i = 0; i < vertices_size; i += 3)
    {
        Vertex normal = {vertices[i] - center.x,
                         vertices[i + 1] - center.y,
                         vertices[i + 2] - center.z};
        float length  = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (length > 0)
        {
            normal.x /= length;
            normal.y /= length;
            normal.z /= length;
        }
        normals.push_back(normal.x);
        normals.push_back(normal.y);
        normals.push_back(normal.z);
    }

    return normals;
}


FlashLightObject::FlashLightObject(const std::string &obj_filepath, const std::string &shader_vert,
                                   const std::string &shader_frag, int id, int pick_id, float pick_r, float pick_g,
                                   float pick_b): Object(obj_filepath, shader_vert, shader_frag), id_(id), pick_id_(pick_id) {
    pick_rgb_[0] = pick_r/255.0;
    pick_rgb_[1] = pick_g/255.0;
    pick_rgb_[2] = pick_b/255.0;

    glGenVertexArrays(1, &arrow_VAO_);
    glGenBuffers(1, &arrow_VBO_);
    glGenBuffers(1, &arrow_EBO_);

}


bool FlashLightObject::checkPickColor(int pick_color_id) const {
    if (pick_color_id == pick_id_)
    {
        return true;
    }
    return false;
}

void FlashLightObject::draw(glm::mat4 &view, glm::mat4 &projection, bool get_pick_color) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    shaderProgram_.use();
    glm::mat4 model = glm::mat4(1.0f);
    translate_vec = glm::vec3(xyz_[0], xyz_[1], xyz_[2]);

    model = glm::translate(model, translate_vec);
    model = glm::rotate(model, glm::radians(light_obj_params_[light_.type].initial_rotate), light_obj_params_[light_.type].rotate_vec);

    model = glm::rotate(model, glm::radians(light_obj_params_[light_.type].frame_rotate_xy_[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(light_obj_params_[light_.type].frame_rotate_xy_[1]), glm::vec3(0.0f, 1.0f, 0.0f));

    model = glm::scale(model, light_obj_params_[light_.type].scale);

    shaderProgram_.setMat4("projection", projection);
    shaderProgram_.setMat4("view", view);
    shaderProgram_.setMat4("model", model);

    if (get_pick_color){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        shaderProgram_.setVec4("ourColor", pick_rgb_[0], pick_rgb_[1], pick_rgb_[2], 1.0f);
    }
    else{
        shaderProgram_.setVec4("ourColor", 1, 1, 1, 1.0f);
    }

    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, 0);

    if (light_.type == 0){
        shaderProgram_.setVec4("ourColor", light_.rgb[0], light_.rgb[1], light_.rgb[2], 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(arrow_VAO_);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(arrow_indices_.size()), GL_UNSIGNED_INT, 0);
    }
}

void FlashLightObject::moveObject(float delta_x, float delta_y, float delta_z) {
    translate_vec.x += delta_x;
    translate_vec.y -= delta_y;
    translate_vec.z += delta_z;
}

void FlashLightObject::rotateObject(float delta_x, float delta_y) {
    light_obj_params_[light_.type].frame_rotate_xy_[0] += delta_y*20;
    light_obj_params_[light_.type].frame_rotate_xy_[1] += delta_x*20;

}

void FlashLightObject::loadObjectBuffers() {
    Object::loadObjectBuffers();

    glBindVertexArray(arrow_VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, arrow_VBO_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * arrow_vertices_.size(),
                 arrow_vertices_.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arrow_EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLuint) * arrow_indices_.size(),
                 arrow_indices_.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

}

void FlashLightObject::reset() {
    light_obj_params_[light_.type].frame_rotate_xy_[0] = 0;
    light_obj_params_[light_.type].frame_rotate_xy_[1] = 0;
    rotateObject();
}

glm::vec3 FlashLightObject::getFlashLightDirection() {
    glm::vec3 dir_start = glm::vec3(0.0, 8.0, 0.0);
    glm::vec3 dir_end = glm::vec3(0.0, -3.0, 0.0);

    glm::mat4 model = glm::mat4(1.0f);
    translate_vec = glm::vec3(xyz_[0], xyz_[1], xyz_[2]);

    model = glm::translate(model, translate_vec);
  //  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(light_obj_params_[light_.type].frame_rotate_xy_[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(light_obj_params_[light_.type].frame_rotate_xy_[1]), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(light_obj_params_[light_.type].scale));

    // Apply translation and scaling
    glm::vec4 transformedStart = model * glm::vec4(dir_start, 1.0);
    glm::vec4 transformedEnd = model * glm::vec4(dir_end, 1.0);

// Convert back to a 3D vector
    glm::vec3 result = glm::vec3(transformedEnd) - glm::vec3(transformedStart);

    return result;
}

glm::vec3 FlashLightObject::getFlashLightPosition() {
    glm::vec3 center = glm::vec3(0.0, 0.0, 0.0);

    glm::mat4 model = glm::mat4(1.0f);
    translate_vec = glm::vec3(xyz_[0], xyz_[1], xyz_[2]);

    model = glm::translate(model, translate_vec);
    //  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
    model = glm::rotate(model, glm::radians(light_obj_params_[light_.type].frame_rotate_xy_[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(light_obj_params_[light_.type].frame_rotate_xy_[1]), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(light_obj_params_[light_.type].scale));

    glm::vec4 transformedCenter = model * glm::vec4(center, 1.0);

// Convert back to a 3D vector

    return transformedCenter;
}

Light &FlashLightObject::getLight() {
    light_.light_dir = getFlashLightDirection();
    light_.light_pos = getFlashLightPosition();

    return light_;
}

GridObject::GridObject(const std::string &shader_vert, const std::string &shader_frag)
        : Object("", shader_vert, shader_frag) {

    vertices_ = {-grid_scale_, 0,0, 1.0f, 0.0f, 0.0f,  // vertex 1: red
                 grid_scale_, 0,0, 1.0f, 0.0f, 0.0f,  // vertex 2: red
                 0, -grid_scale_, 0, 0.0f, 1.0f, 0.0f,  // vertex 3: green
                 0, grid_scale_, 0, 0.0f, 1.0f, 0.0f,  // vertex 4: green
                 0,0,-grid_scale_, 0.0f, 0.0f, 1.0f,   // vertex 5: blue
                 0, 0,grid_scale_, 0.0f, 0.0f, 1.0f   // vertex 6: blue
    };

    arrows_vertices_ = {
            grid_scale_, 0 ,0.1,     1.0f, 0.0f, 0.0f,
           grid_scale_, 0 ,-0.1,     1.0f, 0.0f, 0.0f,
           grid_scale_ + 0.5f , 0 ,0,     1.0f, 0.0f, 0.0f,

           -0.1, grid_scale_, 0,   0.0f, 1.0f, 0.0f,
           0.1, grid_scale_, 0,   0.0f, 1.0f, 0.0f,
           0, grid_scale_ + 0.5f , 0,   0.0f, 1.0f, 0.0f,

           -0.1, 0, -grid_scale_,  0.0f, 0.0f, 1.0f,
            0.1, 0, -grid_scale_,  0.0f, 0.0f, 1.0f,
            0,0, -grid_scale_ -0.5f,  0.0f, 0.0f, 1.0f,
    };

    arrows_indices_ = {0,1,2,
                       3,4,5,
                       6,7,8};

    glGenBuffers(1, &arrows_VBO_);
    glGenBuffers(1, &arrows_EBO_);

}

void GridObject::loadObjectBuffers() {

    glBindVertexArray(VAO_);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), vertices_.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, arrows_VBO_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * arrows_vertices_.size(),
                 arrows_vertices_.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, arrows_EBO_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLuint) * arrows_indices_.size(),
                 arrows_indices_.data(),
                 GL_STATIC_DRAW);

}

void GridObject::draw(glm::mat4 &view, glm::mat4 &projection) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    shaderProgram_.use();

    shaderProgram_.setMat4("projection", projection);
    shaderProgram_.setMat4("view", view);
    shaderProgram_.setMat4("model", model_);

    glBindVertexArray(VAO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_LINES, 0, vertices_.size());

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glBindBuffer(GL_ARRAY_BUFFER, arrows_VBO_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(arrows_indices_.size()), GL_UNSIGNED_INT, 0);
}
