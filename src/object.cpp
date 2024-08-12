#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>

#include "../include/object.h"
#include "../include/loader.h"

Object::Object(const std::string& obj_filepath, const std::string& shader_vert, const std::string& shader_frag): shaderProgram_(shader_vert.c_str(), shader_frag.c_str()) {
    loadObjectFile(obj_filepath);

    // generates a single Vertex Array Object (VAO)  that stores the state needed to supply vertex data,
    // including information about vertex attribute pointers
    glGenVertexArrays(1, &VAO_);

    // generates a single Vertex Buffer Object (VBO) that stores the actual vertex data.
    glGenBuffers(1, &VBO_);
    // buffer for normals
    glGenBuffers(1, &NBO_);
    // buffer for indices
    glGenBuffers(1, &EBO_);
}


void Object::loadObjectFile(const std::string &filepath)
/**Loads vertices and indices from an .obj file using Loader class. If normals are not loaded by Loader, they are calculated with
class method 'calculateNormalsSimple'.*/
{
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
/** Loads data into all Object's buffers: vertices, indices, normals. */
{
    // Binds the VAO_ so that subsequent vertex attribute calls (like setting vertex attributes) are stored in this VAO.
    glBindVertexArray(VAO_);

    // Binds the VBO_ to the GL_ARRAY_BUFFER target, meaning that any subsequent calls related to GL_ARRAY_BUFFER will affect this VBO.
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    // Allocates memory in the GPU and copies the vertex data from the CPU to this allocated GPU memory.
    // GL_STATIC_DRAW indicates that the data will not change frequently, allowing the GPU to optimize memory storage for better performance.
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(GLfloat) * vertices_.size(),
                 vertices_.data(),
                 GL_STATIC_DRAW);
    // Specifies how the vertex data is laid out in memory, so the GPU knows how to interpret it.
    // Stride is the byte offset between consecutive vertex attributes.
    // Pointer is the offset of the first component in the array (0 means the data starts at the beginning).
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Enables the vertex attribute array at the specified index (0 corresponds to the vertex positions) within the currently bound VAO..
    glEnableVertexAttribArray(0);

    // GL_ELEMENT_ARRAY_BUFFER is a target to store indices of each element in the VBO/NBO buffers.
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

void Object::draw(glm::mat4& view, glm::mat4& projection, glm::vec3 camera_position, std::vector<Light> lights)
/** Render Object considering lighting parameters from Light source objects.*/
{
    // glPolygonMode sets the polygon drawing mode, determining how polygons will be rasterized.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    int numLights = static_cast<int>(lights.size());

    // sets ShaderProgram with its id as active current shader program to use for subsequent drawing functions.
    shaderProgram_.use();

    // all subsequent steps until binding VAO set various uniforms for vertex and fragment shaders.
    shaderProgram_.setInt("numLights", numLights);

    shaderProgram_.setVec3("objectColor", rgb_[0], rgb_[1], rgb_[2]);
    // Camera position (or viewer position in this context) is used to calculate specular lighting on the central object
    shaderProgram_.setVec3("viewPos", camera_position);

    // iterate through Light objects' parameters to pass to fragment shader
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

    // After binding VAO, OpenGL will use the vertex data, indices, and attribute configurations associated with this VAO for rendering.
    glBindVertexArray(VAO_);
    // glDrawElements is a rendering command that draws elements (typically triangles) from the currently bound VAO.
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, 0);
}

std::vector<float> Object::calculateNormalsSimple(std::vector<float> vertices)
/** Calculates normal by first determining the geometric center of the object, then computes the direction
from the center to each vertex and normalizes these directions to unit length. */
{
    std::vector<float> normals;
    Vertex center     = {0.0f, 0.0f, 0.0f};
    int num_vertices  = static_cast<int>(vertices.size()) / 3;
    int vertices_size = static_cast<int>(vertices.size());

    // Loop through all vertices to compute the center of the object
    for (int i = 0; i < vertices_size; i += 3)
    {
        // Accumulate the x, y, z coordinates of each vertex
        center.x += vertices[i];
        center.y += vertices[i + 1];
        center.z += vertices[i + 2];
    }

    // Calculate the average to find the center position
    center.x /= num_vertices;
    center.y /= num_vertices;
    center.z /= num_vertices;

    // Loop through all vertices to calculate normals
    for (int i = 0; i < vertices_size; i += 3)
    {
        // Calculate the normal by subtracting the center position from each vertex
        Vertex normal = {vertices[i] - center.x,
                         vertices[i + 1] - center.y,
                         vertices[i + 2] - center.z};

        // Calculate the length (magnitude) of the normal vector
        float length  = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

        // Normalize the normal vector (make it unit length)
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

void FlashLightObject::loadObjectBuffers()
/** Loads data into all Object's buffers: vertices, indices, normals. */
{
    Object::loadObjectBuffers();
    // if the Light object has a type of spotlight, then additional VAO, VBO and EBO buffers for central arrow are loaded.
    if (light_.type == 0)
    {
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
}

bool FlashLightObject::checkPickColor(int pick_color_id) const
/** Checks if Object's pick_color_id is equal to the provided id. */
{
    if (pick_color_id == pick_id_)
    {
        return true;
    }
    return false;
}

void FlashLightObject::draw(glm::mat4 &view, glm::mat4 &projection, bool get_pick_color)
/** Render Light Object using OpenGL.*/
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    shaderProgram_.use();
    glm::mat4 model = getTranslationMatrix(true);

    shaderProgram_.setMat4("projection", projection);
    shaderProgram_.setMat4("view", view);
    shaderProgram_.setMat4("model", model);

    // if the Light object is picked, it's rendered with its pick_color (buffers will not be switched in this case)
    if (get_pick_color)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        shaderProgram_.setVec4("ourColor", pick_rgb_[0], pick_rgb_[1], pick_rgb_[2], 1.0f);
    }
    else
    {
        shaderProgram_.setVec4("ourColor", 1, 1, 1, 1.0f);
    }

    glBindVertexArray(VAO_);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, 0);

    // if the Light object has a type of spotlight, then the arrow through the center of Flashlight object is rendered
    if (light_.type == 0)
    {
        shaderProgram_.setVec4("ourColor", light_.rgb[0], light_.rgb[1], light_.rgb[2], 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(arrow_VAO_);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(arrow_indices_.size()), GL_UNSIGNED_INT, 0);
    }
}

void FlashLightObject::rotateObject(float delta_x, float delta_y)
/** Adjusts the rotation of a Light object based on the changes in the x-axis (delta_x) and y-axis (delta_y).*/
{
    light_obj_params_[light_.type].frame_rotate_xy_[0] += delta_y*20;
    light_obj_params_[light_.type].frame_rotate_xy_[1] += delta_x*20;
}

void FlashLightObject::reset()
/** Resets the rotation of the Light object to its original state by setting the rotation angles to zero.*/
{
    light_obj_params_[light_.type].frame_rotate_xy_[0] = 0;
    light_obj_params_[light_.type].frame_rotate_xy_[1] = 0;
    rotateObject();
}

glm::mat4 FlashLightObject::getTranslationMatrix(bool initial_rotate)
/** Constructs and returns the transformation matrix for the Light object.
The matrix includes translation, optional initial rotation, dynamic frame rotation, and scaling transformations applied to the object.*/
{
    // the order of applied transformations (from right to left): scaling -> rotation -> translation.
    glm::mat4 model = glm::mat4(1.0f);
    translate_vec_ = glm::vec3(xyz_[0], xyz_[1], xyz_[2]);

    model = glm::translate(model, translate_vec_);
    if (initial_rotate)
    {
        model = glm::rotate(model, glm::radians(light_obj_params_[light_.type].initial_rotate), light_obj_params_[light_.type].rotate_vec);
    }
    model = glm::rotate(model, glm::radians(light_obj_params_[light_.type].frame_rotate_xy_[0]), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(light_obj_params_[light_.type].frame_rotate_xy_[1]), glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, glm::vec3(light_obj_params_[light_.type].scale));

    return model;
}

glm::vec3 FlashLightObject::getFlashLightDirection()
/** Calculates and returns the direction vector of the Light object by transforming a start and end point using
the object's transformation matrix. */
{
    // Define the start and end points of the Light object's direction vector
    glm::vec3 dir_start = glm::vec3(0.0, 8.0, 0.0);
    glm::vec3 dir_end = glm::vec3(0.0, -3.0, 0.0);

    // Get the current transformation matrix that includes all transformations
    glm::mat4 model = getTranslationMatrix();
    // Apply translation and scaling
    glm::vec4 transformedStart = model * glm::vec4(dir_start, 1.0);
    glm::vec4 transformedEnd = model * glm::vec4(dir_end, 1.0);

    // Calculate the direction vector by subtracting the transformed start point from the end point
    glm::vec3 result = glm::vec3(transformedEnd) - glm::vec3(transformedStart);

    return result;
}

glm::vec3 FlashLightObject::getFlashLightPosition()
/** Calculates and returns the position of the Light object by transforming its origin (center) point using
the object's transformation matrix.*/
{
    // define the default center point of the Light object
    glm::vec3 center = glm::vec3(0.0, 0.0, 0.0);
    // Get the current transformation matrix that includes all transformations
    glm::mat4 model = getTranslationMatrix();
    // Transform the center point using the transformation matrix
    glm::vec4 transformedCenter = model * glm::vec4(center, 1.0);

    return transformedCenter;
}

Light &FlashLightObject::getLight()
/** Updates the flashlight's direction and position based on its current transformations and returns
a reference to the Light struct. */
{
    light_.light_dir = getFlashLightDirection();
    light_.light_pos = getFlashLightPosition();

    return light_;
}

AxisObject::AxisObject(const std::string &shader_vert, const std::string &shader_frag)
        : Object("", shader_vert, shader_frag){

    // A single vertex of an ais-arrow consists of position and color values: x, y, z, r,g,b
    vertices_ = {
                 -axis_scale_, 0,0, 1.0f, 0.0f, 0.0f,  // vertex 1: red
                 axis_scale_, 0,0, 1.0f, 0.0f, 0.0f,  // vertex 2: red
                 0, -axis_scale_, 0, 0.0f, 1.0f, 0.0f,  // vertex 3: green
                 0, axis_scale_, 0, 0.0f, 1.0f, 0.0f,  // vertex 4: green
                 0,0,-axis_scale_, 0.0f, 0.0f, 1.0f,   // vertex 5: blue
                 0, 0,axis_scale_, 0.0f, 0.0f, 1.0f   // vertex 6: blue
    };

    arrows_vertices_ = {
            axis_scale_, 0 ,0.1,     1.0f, 0.0f, 0.0f,
            axis_scale_, 0 ,-0.1,     1.0f, 0.0f, 0.0f,
            axis_scale_ + 0.5f , 0 ,0,     1.0f, 0.0f, 0.0f,

            -0.1, axis_scale_, 0,   0.0f, 1.0f, 0.0f,
            0.1, axis_scale_, 0,   0.0f, 1.0f, 0.0f,
            0, axis_scale_ + 0.5f , 0,   0.0f, 1.0f, 0.0f,

            -0.1, 0, -axis_scale_,  0.0f, 0.0f, 1.0f,
            0.1, 0, -axis_scale_,  0.0f, 0.0f, 1.0f,
            0,0, -axis_scale_ -0.5f,  0.0f, 0.0f, 1.0f,
    };

    arrows_indices_ = {0,1,2,
                       3,4,5,
                       6,7,8};

    glGenBuffers(1, &arrows_VBO_);
    glGenBuffers(1, &arrows_EBO_);

}

void AxisObject::loadObjectBuffers()
/** Loads data into all Object's buffers: vertices (position + color), indices. */
{
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

void AxisObject::draw(glm::mat4 &view, glm::mat4 &projection)
/** Render coordinate system using OpenGL. */
{
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