![screenshot](https://github.com/snej55/pipeline_visualisation/blob/main/screenshots/screenshot_1.png)

## Building:

The project is currently only set up to build on Linux (Windows build is faulty), with OpenGL v4.1. It has been tested on Arch Linux and Ubuntu. To build it, install GLFW3, GLAD, GLU, Assimp, and Freetype (v2) from your distro's package manager. CMake and CXX compiler are required. The recommended generator is Ninja, but ordinary Makefile should suffice fine.

Then run:
```
# Clone the repo
mkdir visualisation
cd visualisation
git clone https://github.com/snej55/pipeline_visualisation.git .

# Build it
cmake -S . -B build/ -G Ninja
cmake --build build/ -j14

# Run!
cd build; ./main
```

>[!NOTE]
>Please make sure to run the compiled binary from the build folder, so it has access to the required assets (shaders, models, etc).

Create an issue or contact me directly if you encounter any problems during the build process.

## Running:

- WASD to move around
- Use the mouse to look around
- ESC to exit
- Up arrow / down arrow to change animation speed
- C to change the viewing mode
- B to toggle the bar chart mode
- M/N to change the max amount of bars in the bar chart

## Libraries in use:

- GLFW3 & GLAD for OpenGL and rendering.
- GLM for matrix calculations.
- Freetype2 for font rendering.
- Convex Hull 3D library (https://github.com/leomccormack/convhull_3d)

<p float="left">
  <img src="https://github.com/snej55/pipeline_visualisation/blob/main/screenshots/Screenshot_20250516_165311.png" width="200">
  <img src="https://github.com/snej55/pipeline_visualisation/blob/main/screenshots/Screenshot_20250521_160203.png" width="300">
  <img src="https://github.com/snej55/pipeline_visualisation/blob/main/screenshots/Screenshot_20250521_170309.png" width="300">
  <img src="https://github.com/snej55/pipeline_visualisation/blob/main/screenshots/Screenshot_20250521_171522.png" width="300">
  <img src="https://github.com/snej55/pipeline_visualisation/blob/main/screenshots/Screenshot_20250522_151029.png" width="300">
  <img src="https://github.com/snej55/pipeline_visualisation/blob/main/screenshots/Screenshot_20250522_154708.png" width="300">
  <img src="https://github.com/snej55/pipeline_visualisation/blob/main/screenshots/Screenshot_20250522_160759.png">
</p>
