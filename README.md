# GoldSrc Map Renderer

> A renderer for Valve GoldSrc maps (Half-Life, Counter-Strike 1.6, ...).

- Modern C++ and OpenGL
- Loads level geometry from BSP-files
- Loads textures from WAD-archives
- FPS-style flying camera
  
## Motivation
I chose this project as a non-trivial use-case for studying low-level graphics programming / OpenGL (something that goes beyond rendering a triangle). Additionally, I wanted to get a deeper understanding of how classic engines from the Quake-era worked. 

Also, I grew up with Half-Life and Counter-Strike ❤️.

## Screenshots

| de_dust | cs_assault |
|--------|------------|
| <img src="./screenshots/dedust.gif" width="400"/> | <img src="./screenshots/csassault.gif" width="400"/> |


  
## Running the application
The build has been tested on MacOS (Clangd on macOS Tahoe 26.0.1), Linux (Fedora) and Windows.

### Assets
As Valve's original maps and texture archives are copyrighted they are not distributed in this repository. If you own a copy of Half-life or Counter-Strike you can use their BSP- and WAD-files. The engine expects maps (BSP-files) to be included in `APP_ROOT/assets/maps/` and texture archives to be included in `APP_ROOT/assets/textures`. If the renderer doesn't find a texture, it skips rendering that plane.

### Build

This project uses **CMake** with **Vcpkg** for dependency management. 

Prerequisites:
- CMake >= 3.21
- Vcpkg
- A C++20 compiler

Vcpkg needs to be set up for this project to build. Usually this requires creating your own `CMakeUserPresets.json` in the repository root. For more instructions, check the [Vcpkg documentation](https://learn.microsoft.com/vcpkg/get_started/get-started).

Resolve all dependencies first with
``` bash
vcpkg install
cmake --preset default
```

build with:
``` bash
cmake --build build
```

#### Linux requirements

From my tests on Fedora, it appears that it is easier to not use Vcpkg for the SDL3 dependency. For linux, the following packages are required:

```
sudo dnf install \
  SDL3-devel \
  glew-devel \
  mesa-libGL-devel \
  mesa-libGLU-devel \
  libX11-devel \
  wayland-devel \
  libXcursor-devel \
  libXrandr-devel \
  libXinerama-devel \
  libXi-devel \
  pkg-config \
  cmake \
  ninja 
```
## Inspiration and resources
I used the resources listed here for documentation on the inner workings of the GoldSrc-engine. They're awesome.
- [OGS](https://github.com/headcrab-junkyard/OGS) - An open-source GoldSrc engine clone.
- [Valve Developer Community](https://developer.valvesoftware.com/wiki/BSP_(GoldSrc)) - Wiki on GoldSrc BSP file format.
- [HL Texture Tools](https://github.com/yuraj11/HL-Texture-Tools) - Create and view wad and spr files
