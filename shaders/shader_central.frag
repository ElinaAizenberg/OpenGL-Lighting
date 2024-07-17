#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform int numLights;

uniform vec3 viewPos;
uniform vec3 objectColor;

uniform int lightType[4];
uniform vec3 lightPos[4];  // Positions of the lights
uniform vec3 lightDir[4];  // Positions of the lights
uniform vec3 lightColor[4];  // Colors of the lights
uniform float lightIntensity[4];

uniform float linear[4];
uniform float quadratic[4];

uniform float cutOff[4];
uniform float outerCutOff[4];


void main()
{
    vec3 norm = normalize(Normal);

    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    float ambientStrength = 0.1;
    float specularStrength = 0.5;
    float shininess = 32.0;
    float constant = 1.0;

    for (int i = 0; i < numLights; ++i)
    {
    // attenuation
        float distance    = length(lightPos[i] - FragPos);
        float attenuation = 1.0 / (constant + linear[i] * distance + quadratic[i] * (distance * distance));

       // Ambient
           ambient += ambientStrength * lightColor[i];

           // Diffuse
         vec3 lightDirNormalized = normalize(lightDir[i]);
        vec3 lightDirToFrag = normalize(lightPos[i] - FragPos);
        float diff = max(dot(norm, lightDirToFrag), 0.0);

       // Spotlight (soft edges)
        float theta = dot(lightDirToFrag, -lightDirNormalized);
        float epsilon = cutOff[i] - outerCutOff[i];
       float intensity = clamp((theta - outerCutOff[i]) / epsilon, 0.0, 1.0);
       intensity *= lightIntensity[i];

       if (lightType[i] == 0)
       {diffuse += (diff * lightColor[i]) * intensity;}
       else {diffuse += (diff * lightColor[i]) * attenuation;}



        // Specular
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDirToFrag, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        if (lightType[i] == 0) {specular += (specularStrength * spec * lightColor[i])*intensity;}
        else {specular += (specularStrength * spec * lightColor[i])* attenuation;}

    }

    vec3 lighting = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(lighting, 1.0);
};