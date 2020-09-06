# GLES3+SDL2 Demos

These demos are the incremental examples from Hans de Ruiter's amazing tutorials on OpenGL ES 3.0 and SDL2, check them out [here!](https://keasigmadelta.com/store/gles3sdl2-tutorial/)

### Contents

  - Tutorial 1: An empty SDL window with a GLContext
  - Tutorial 2: Draw a single triangle using vertex and fragment shaders
  - Tutorial 3: Draw a texture using SDL Image
  - Tutorial 4: Draw a cube, set up a mvpMatrix using OpenGL Mathematics (GLM)
  - Tutorial 5: Add a single point light source
  - Tutorial 5a: Animate the cube with a rotation

### Build Instructions

Tutorial 1:

```sh
$ g++ main.cpp `pkg-config --cflags --libs sdl2 glesv2`
```

Tutorial 2:

```sh
$ g++ main.cpp shader.cpp `pkg-config --cflags --libs sdl2 glesv2`
```

Tutorial 3 onwards:

```sh
$ g++ main.cpp shader.cpp texture.cpp `pkg-config --cflags --libs sdl2 SDL2_image glesv2`
```

### Dependencies

  - libsdl2-dev
  - libsdl2-image-dev
  - libglm-dev

### Preview

![Preview Image](https://user-images.githubusercontent.com/12631133/92319515-7c4f1080-efef-11ea-9933-c2aa695ce9c7.png)
