# OpenGL: Lighting
## Overview
An application to study OpenGL programming (shaders)

## Features

## Screenshots
<image src="https://github.com/user-attachments/assets/c7571f90-c7f6-4d2a-a32f-c10edda86fb3"  width="500">
  
<image src="https://github.com/user-attachments/assets/1be1738e-34e9-4e83-b8c6-4afd82a40d6a"  width="500">


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
4. select **Generate a loader**
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
