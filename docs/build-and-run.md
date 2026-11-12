# Build and run

## Dependencies

The application requires:

- A C++11-compatible compiler (`clang++` is the default in the Makefile).
- OpenGL 3.3 support from the graphics driver.
- GLFW 3 and GLEW development libraries.
- `pkg-config` so the Makefile can locate GLFW and GLEW.

## Build

Run these commands from the repository root:

```bash
make check-gl
make
```

The executable is named `baikonur`. Start it with:

```bash
make run
```

## Useful runtime options

```text
--hidden             Create a hidden window.
--no-vsync           Disable vertical synchronization.
--frames N           Render N frames and then exit.
--simulate SECONDS   Advance the rocket simulation before rendering.
--capture FILE       Save a frame as a PPM image.
--shaders DIRECTORY  Load shaders from a custom directory.
```

For example, render 120 frames with a simulated launch and save the final frame:

```bash
./baikonur --hidden --frames 120 --simulate 8 --capture launch.ppm
```

## Tests and cleanup

```bash
make test
make clean
```

`make test` builds and runs the core logic tests without opening an OpenGL window. `make clean` removes generated build files and the executable.
