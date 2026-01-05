# GoldSource Map Renderer

> A project for rendering GoldSrc BSP Maps (Half-Life, Counter-Strike 1.6) using C++ and modern OpenGL.

| de_dust | cs_assault |
|--------|------------|
| <img src="./screenshots/dedust.gif" width="400"/> | <img src="./screenshots/csassault.gif" width="400"/> |



## Building
### build
Configure build with
#+begin_src bash
  cmake -S ./ -B ./build
#+end_src

build with:
#+begin_src bash
  cmake --build ./
#+end_src

after installing a vcpkg package, run:
#+begin_src bash
  vcpkg install
  cmake --preset default
#+end_src
and then you can build again.
