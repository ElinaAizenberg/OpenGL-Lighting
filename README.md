# OpenGL: Lighting
## Overview
This mini-application was developed as an educational tool to explore lighting techniques in OpenGL, including ambient, diffuse, and specular lighting, as well as the distinctions between spotlight and point light. The project also showcases the transition from the fixed-function pipeline to a more flexible and powerful rendering approach using shaders and a ShaderProgram. It serves as a practical example for learning how to implement and manipulate lighting in 3D scenes, providing an interactive environment to experiment with various light sources and their effects on objects.

## Features
- **Central object rendering:** load and display a central 3D object from any .obj file, with customizable scale and color.
- **Dome Camera system:** rotate the camera freely around the central object to view it from any angle, enhancing the study of lighting effects.
- **Coordinate system:** option to toggle the display of a coordinate system at the center of the scene for reference.
- **Multiple Light Sources:** add up to 4 light sources, with options for spotlight and point light types:
  - *Spotlight:* represented by a flashlight object, demonstrating focused light with adjustable parameters;
  - *Point light:* represented by a light bulb object, showing omnidirectional light.
- **Interactive light control panels:** double-click on any light source to open its individual GUI panel, where you can:
  1. adjust the light's location, angle, color, and type;
  2. turn the light on or off;
  3. modify light parameters based on its type, observing real-time effects on the scene.
- **Shader-Based Rendering**: demonstrates the use of shaders and ShaderProgram for rendering, moving away from the traditional fixed-function pipeline.

## Screenshots
<image src="https://github.com/user-attachments/assets/c7571f90-c7f6-4d2a-a32f-c10edda86fb3"  width="700">
  
<image src="https://github.com/user-attachments/assets/1be1738e-34e9-4e83-b8c6-4afd82a40d6a"  width="700">


## Getting Started
### Prerequisites
- Compiler that supports C++
- CMake (>= 3.10)
- OpenGL (version 3.3)
- GLFW
- GLAD 

**External libraries:**
- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImGui Portable File Dialogs](https://github.com/samhocevar/portable-file-dialogs)
- [ImGui Toggle](https://github.com/cmdwtf/imgui_toggle)
- [Tinyobjloader](https://github.com/tinyobjloader/tinyobjloader)

### Installation
1. Clone the repo
```
git clone https://github.com/ElinaAizenberg/OpenGL-Lighting
cd OpenGL-Lighting
```

2. In CMakeLists.txt update the path to the folder with external libraries
```
#TODO Specify the directory where imgui and other libraries are located
set(EXTERNAL_LIB_DIR /path_to_directory/libs)
```
Go to the [GLAD website](http://glad.dav1d.de/):
1. select language **C++**;
2. in the API section select an **OpenGL version 3.3**;
3. set the profile to **Core**;
4. select **Generate a loader**;
5. generate the resulting library files.
```
#TODO Specify the directory where glad files are located
set(GLAD /path_to_directory/glad)
```

3. Build project
```
mkdir build
cd build
cmake ..
make
```

4. Run project
```
./project_3
```
