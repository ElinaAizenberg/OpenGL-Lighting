#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // this is used for objects that are not colored and do not reflect light, therefore there is no need for any
    // vertex attributes other than position.
    gl_Position = projection * view * model * vec4(aPos, 1.0f);   // correct order of matrix multiplication
};