#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos; // output to fragment shader
out vec3 Normal; // output to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Normal matrix is a trick to keep normals perpendicular even if non-uniform scaling is applied
    Normal = mat3(transpose(inverse(model))) * aNormal;

    FragPos = vec3(model * vec4(aPos, 1.0));  // FragPos is used further in fragment shader for lighting calculation
    gl_Position = projection * view * vec4(FragPos, 1.0);
};