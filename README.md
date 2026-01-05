# GoldSrc Map Renderer

> A renderer for Valve GoldSrc maps (Half-Life, Counter-Strike 1.6, ...).

- Modern C++ and OpenGL
- Loads level geometry from BSP-files
- Loads textures from WAD-archives
- FPS-style flying camera

## Screenshots

| de_dust | cs_assault |
|--------|------------|
| <img src="./screenshots/dedust.gif" width="400"/> | <img src="./screenshots/csassault.gif" width="400"/> |



## Building
### build
Configure build with
``` bash
  cmake -S ./ -B ./build
```

build with:
``` bash
  cmake --build ./
```

after installing a vcpkg package, run:
```
  vcpkg install
  cmake --preset default
```
and then you can build again.
