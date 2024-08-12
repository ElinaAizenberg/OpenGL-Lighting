#version 330 core
out vec4 FragColor;  // Output color of the fragment

in vec3 Normal;      // Normal vector for the current fragment, passed from the vertex shader
in vec3 FragPos;     // Position of the current fragment in world space

// Uniforms passed to the shader
uniform int numLights;  // Number of active lights

uniform vec3 viewPos;  // Position of the camera
uniform vec3 objectColor;  // Base color of the object

uniform int lightType[4];  // Type of each light (0 for spotlight, 1 for point light)
uniform vec3 lightPos[4];  // Positions of the lights in world space
uniform vec3 lightDir[4];  // Directions of the lights (for spotlights)
uniform vec3 lightColor[4];  // Colors of the lights
uniform float lightIntensity[4];  // Intensity of the lights

uniform float linear[4];  // Linear attenuation factor for each light
uniform float quadratic[4];  // Quadratic attenuation factor for each light

uniform float cutOff[4];  // Inner cutoff angle (cosine) for spotlights
uniform float outerCutOff[4];  // Outer cutoff angle (cosine) for spotlights

void main()
{
    vec3 norm = normalize(Normal);  // Normalize the normal vector to ensure it has a length of 1

    // Initialize the ambient, diffuse, and specular components of lighting
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    // Lighting parameters
    float ambientStrength = 0.1;  // Ambient light strength
    float specularStrength = 0.5;  // Specular highlight strength
    float shininess = 32.0;  // Shininess factor for specular highlights
    float constant = 1.0;  // Constant attenuation factor (used for distance-based attenuation)

    // Iterate over all lights
    for (int i = 0; i < numLights; ++i)
    {
        // Calculate the distance from the light to the fragment
        float distance = length(lightPos[i] - FragPos);

        // Calculate attenuation based on distance
        float attenuation = 1.0 / (constant + linear[i] * distance + quadratic[i] * (distance * distance));

        // Ambient light is constant and affects all surfaces equally
        ambient += ambientStrength * lightColor[i];

        // Diffuse light depends on the angle between the light direction and the surface normal
        vec3 lightDirNormalized = normalize(lightDir[i]);  // Normalize light direction (for spotlights)
        vec3 lightDirToFrag = normalize(lightPos[i] - FragPos);  // Direction from the fragment to the light
        float diff = max(dot(norm, lightDirToFrag), 0.0);  // Lambertian reflectance (diffuse component)

        // Spotlight with soft edges is calculated using the dot product between the light direction and the direction to the fragment
        float theta = dot(lightDirToFrag, -lightDirNormalized);
        float epsilon = cutOff[i] - outerCutOff[i];  // Difference between inner and outer cutoff angles
        float intensity = clamp((theta - outerCutOff[i]) / epsilon, 0.0, 1.0);  // Smoothstep to create soft edges
        intensity *= lightIntensity[i];  // Apply light intensity to the spotlight

        // Apply diffuse lighting based on light type (spotlight or point light)
        if (lightType[i] == 0)  // Spotlight
        {
            diffuse += (diff * lightColor[i]) * intensity;
        }
        else  // Point light
        {
            diffuse += (diff * lightColor[i]) * attenuation;
        }

        // Specular light depends on the viewer's position and creates highlights
        vec3 viewDir = normalize(viewPos - FragPos);  // Direction from the fragment to the viewer
        vec3 reflectDir = reflect(-lightDirToFrag, norm);  // Reflection vector
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);  // Specular component

        // Apply specular lighting based on light type (spotlight or point light)
        if (lightType[i] == 0)  // Spotlight
        {
            specular += (specularStrength * spec * lightColor[i]) * intensity;
        }
        else  // Point light
        {
            specular += (specularStrength * spec * lightColor[i]) * attenuation;
        }
    }

    // Combine all lighting components (ambient, diffuse, specular) and multiply by the object's base color
    vec3 lighting = (ambient + diffuse + specular) * objectColor;

    // Set the final fragment color
    FragColor = vec4(lighting, 1.0);
};
