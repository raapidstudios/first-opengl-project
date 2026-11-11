# First OpenGL Project

> Journey of learning modern OpenGL graphics programming with C++.

A real-time 3D graphics rendering project built with **C++** and **OpenGL**. This repository documents my progress learning core graphics concepts—ranging from custom GLSL vertex/fragment shaders and texture mapping (such as a lit, rotating 3D pyramid) to a 3D camera system and external 3D model loading.

## Features

- **Custom GLSL Shaders:** Dedicated vertex and fragment shaders (`shader.vs`, `shader.fs`, `model_loading.vs`, `model_loading.fs`) managed via a reusable `Shader` class (`shader.h`).
- **3D Camera System:** Interactive camera movement and view matrix transformations implemented in `camera.h` and `camera.cpp`.
- **Texture Mapping & Lighting:** Loads image textures using `stb_image.h` and applies dynamic multi-colored lighting and surface shading.
- **3D Mesh & Model Loading:** Supports loading and rendering complex 3D models and meshes (`mesh.h`, `model.h`, `model_demo.cpp`).
- **CMake Build System:** Cross-platform build configuration via `CMakeLists.txt`.

## Repository Structure

```text
first-opengl-project/
├── resources/            # Textures, 3D models, and media assets
├── CMakeLists.txt        # CMake build configuration
├── main.cpp              # Main application entry point & render loop
├── model_demo.cpp        # Demo scene for 3D model loading
├── camera.h / camera.cpp # 3D camera class & movement logic
├── shader.h              # Shader compilation & uniform utility class
├── shader.vs / shader.fs # Core vertex and fragment shaders
├── model_loading.vs/.fs  # Shaders for 3D model rendering
├── mesh.h                # Mesh data structure & vertex buffer setup
├── model.h               # 3D model loader class
├── stb_image.h           # Single-header image loading library
└── .gitignore
```

## Getting Started

### Prerequisites

Make sure you have the following installed on your system:

- **C++ Compiler:** GCC, Clang, or MSVC (supporting C++11 or newer)
- **CMake:** Version 3.10 or higher
- **Dependencies:** OpenGL, GLFW, GLAD, GLM, and Assimp

### Building the Project

1. **Clone the repository:**
   ```bash
   git clone https://github.com/raapidstudios/first-opengl-project
   cd first-opengl-project
   ```

2. **Configure and build with CMake:**
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

3. **Run the executable:**
   ```bash
   # On Linux / macOS
   ./first-opengl-project

   # On Windows
   .\Debug\first-opengl-project.exe
   ```
   *(Note: Run the executable from the root project directory if needed so relative paths to `resources/` and shader files resolve properly.)*

## Controls

- **W / A / S / D:** Move camera forward, left, backward, and right
- **Mouse Movement:** Look around (pitch & yaw)
- **Mouse Scroll while holding:** rotate the model
- **ESC:** Close the application

## Acknowledgments

- Inspired by [LearnOpenGL](https://learnopengl.com) by Joey de Vries.
- Uses [`stb_image.h`](https://github.com) by Sean Barrett for texture loading.
