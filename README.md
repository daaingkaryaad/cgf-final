# Computer Graphics Fundamentals Final Project

## Topic: Baikonur Rocket Launch (9)
An interactive 3D rocket-launch simulation built with C++ and modern OpenGL.

### Group: SE-2424

## Team and Contributions

| Team member | Contributions |
| --- | --- |
| Ingkar Adilbek | Initial scene and rendering integration, launch-site geometry, procedural texturing, repository coordination and combined verification |
| Dinara Dyussembayeva | Free and chase cameras, ignition camera shake, camera input handling and numerical stability of vector, matrix and projection operations |
| Fariza Arstanbek | Altitude-based sky transition, procedural stars and antialiasing, title-panel cleanup, booster landing paths and settling animation |
| Ulzhan Nabiyeva | Instanced fire and smoke, exhaust integration and cutoff, simulated-startup preparation, launch timing, argument validation and persistent landed boosters |
| Syndaly Yerzhan | Project structure, booster separation and falling motion, impact detection, separation tests, build cleanup, rendering smoke-check target and fps recovery after minimization |

## Features

1. Launch and Simulation

2. Scene and Rendering

3. Camera and Interface

4. Requirements

## Build and Run
Run build and application commands strictly from the repository root.

#### For MacOS
Install the <b> Apple command-line tools</b> if they are not already available:

``` xcode-select --install ```

Finish the installation dialog before continuing. With <b>homebrew</b> installed, install the graphics dependencies:

``` brew install glfw glew pkgconf ```

<b>Clone, build and launch:</b>

``` 
make check-gl 
make 
make run 
```

## Controls

| Input | Action |
| --- | --- |
| `space` | Start the countdown and launch from the ready state |
| `r` | Reset the scene and prepare another launch |
| `c` | Switch between free and chase cameras; returning to free mode restores the saved free-camera view |
| `h` | Return to the home camera, useful for viewing the launch area and landed boosters after ascent |
| `l` | Toggle the launch-pad light |
| `w` / `s` | Move forward / backward in free-camera mode |
| `a` / `d` | Move left / right in free-camera mode |
| `q` / `e` | Move down / up in free-camera mode |
| `shift` | Increase free-camera movement speed |
| Left mouse click | Capture the mouse for looking around in free-camera mode |
| Mouse movement | Rotate the free camera while the mouse is captured |
| `tab` | Capture or release the mouse in free-camera mode |
| `esc` | Release a captured mouse; close the application when the mouse is already released |

Free-camera movement and mouse look are disabled while chase mode is active. Losing window focus releases mouse capture.

To follow a launch, press ``` c ```, then space. After ascent, press ``` h ``` to inspect the launch site. Press ``` r ``` to reset; press ``` c ``` again if chase mode is wanted for the next launch.

## Checks and runtime options

Run the core simulation + transformation + camera and geometry checks:
```
make test
```

Check application startup and shader compilation in a hidden window:
```
make check-render
```

| option | effect |
| --- | --- |
| `--hidden` | Create a hidden window |
| `--no-vsync` | Disable vertical synchronization |
| `--frames N` | Render a positive integer number of frames, then exit |
| `--simulate SECONDS` | Advance the rocket and exhaust together before rendering; the value must be finite and nonnegative |
| `--capture FILE` | Save the final requested frame as a binary ppm image; without `--frames`, capture one frame and exit |
| `--shaders DIRECTORY` | Load shaders from the specified directory |

For example, capture the scene eight seconds into a launch:
```
./baikonur --hidden --simulate 8 --frames 1 --capture launch.ppm
```

Remove the executable and generated build files with:
```
make clean
```

## Project Structure

| Path | Contains |
| --- | --- |
| `include/` | Class declarations and shared mathematical utilities |
| `src/` | Application loop, scene, rocket, camera, meshes, texture, particles, shaders and interface |
| `shaders/` | Separate glsl `.vert` and `.frag` programs |
| `tests/` | Core checks and booster landing tests |
| `docs/` | Supporting architecture, build, controls and development notes |
| `makefile` | Build, run, core-test, rendering-check and cleanup targets |

## Asset credits and licenses

#### Project generated assets

| asset | creator and source | license / attribution status |
| --- | --- | --- |
| Rocket, boosters, launch-site structures, terrain and scene props | Project team. Procedural geometry in `src/Geometry.cpp` and scene assembly in `src/Scene.cpp` | Generated for this project, so no external model asset license applies |
| Concrete surface texture | Project team. Pixels generated in `src/Texture.cpp` | Generated for this project, so no downloaded texture image |
| Fire, smoke and stars | Project team. Particle implementation and the programs in `shaders/` | Project-specific effects, so no imported particle sprites or skybox images |
| Interface lettering | Project team. Bitmap glyph data defined in `src/Hud.cpp` | Project-specific glyph data, so no external font file |

#### Adapted course material
Parts of the vector/matrix utilities, the original phong-reflection approach, and the initial window/build setup were adapted from the course assignment 4. They were extended with the launch hierarchy, world-space lighting, additional light sources, fog, camera behavior and final-scene integration.

#### External libraries

| Dependency | Authors/Project | Goal | License and source |
| --- | --- | --- | --- |
| GLFW | Marcus Geelnard, Camilla Lowy and GLFW contributors | Window creation, input and graphics-context setup | [zlib/libpng license](https://www.glfw.org/license.html); [project source](https://github.com/glfw/glfw) |
| GLEW | Milan Ikits, Marcelo E. Magallon, Lev Povalahev and GLEW contributors | Loading opengl entry points | Modified BSD terms, with Mesa and Khronos permission notices in the [upstream license file](https://github.com/nigels-com/glew/blob/master/LICENSE.txt); [project source](https://github.com/nigels-com/glew) |

These libraries are installed as system dependencies. Their authors retain their respective copyright and license terms.

## Technical references
- GLFW window and framebuffer guide, glfw contributors; window/context reference, with the library license linked above.
- FLEW initialization guide, glew contributors; library licensing is linked above.
- Apple command-line tools installation, Apple; installation reference.
- Homebrew GLFW, glew and pkgconf package pages, homebrew contributors; dependency installation references.

## AI use

Claude AI was used to assist with organizing and structuring the report PDF, finding special symbols and characters not easily accessible on the MAC keyboard (→, ≥, ≤ etc), and planning the structure of the demonstration video.