# Computer Graphics Fundamentals Final Project

## Topic: Baikonur Rocket Launch (9)
An interactive 3D rocket-launch simulation built with C++ and modern OpenGL.

### Group: SE-2424

## Team and Contributions

| Team member | Contributions |
| --- | --- |
| Ingkar Adilbek | Initial scene and rendering integration, launch-site geometry, procedural texturing, manual application testing, fps checks, documentation, repository coordination and combined verification |
| Dinara Dyussembayeva | Free and chase cameras, ignition camera shake, camera input handling and numerical stability of vector, matrix and projection operations, demo video and release |
| Fariza Arstanbek | Altitude-based sky transition, procedural stars and antialiasing, title-panel cleanup, booster landing paths and settling animation, 	launch-site equipment and directional sun shadows |
| Ulzhan Nabiyeva | Instanced fire and smoke, exhaust integration and cutoff, simulated-startup preparation, launch timing, argument validation and persistent landed boosters, 	pre-launch workers and engine ignition lighting |
| Syndaly Yerzhan | Project structure, booster separation and falling motion, impact detection, separation tests, build cleanup, rendering smoke-check target and fps recovery after minimization, 	service vehicle and retracting service arm |

## Features

#### 1. Launch and Simulation
- a key-triggered sequence with a three-second countdown, ignition, ascent and completion states.
- a rocket assembled from a central body and four attached boosters using parent-child transforms.
- 4 boosters detach at 54 scene units, drift outward through clear landing corridors, tumble, touch down and settle horizontally around the launch site.
- landed boosters remain visible and stationary until the scene is reset.
- instanced fire and smoke from all five engines before separation; only the central engine continues emitting afterward.
- time-based movement with small simulation steps to preserve launch timing during slow rendering frames.
- reset restores the rocket, boosters, camera, lighting and scene actors for another launch.
#### 2. Scene and Rendering
- a launch pad, service tower, utility building, storage tanks, road and steppe landscape built from procedural geometry.
- a parked service vehicle, crates, cable reels and a small equipment cart around the utility area.
- low-poly workers visible before ignition and cleared from the scene when ignition begins.
- a service arm that retracts during countdown before the rocket lifts off.
- model, view and projection matrices, depth testing and custom glsl vertex and fragment shaders.
- per-fragment phong lighting with ambient, diffuse and specular terms.
- directional sunlight, a toggleable launch-pad light and an orange engine light that illuminates nearby surfaces during ignition and fades with ascent.
- ground shadows beneath the scene objects and landed boosters.
- a generated concrete texture with uv coordinates, repeat wrapping and mipmapping.
- a sky that darkens with altitude and procedural stars that become visible as the sky darkens.
- additive fire blending and depth-sorted, alpha-blended smoke.
#### 3. Camera and Interface
- a keyboard-and-mouse free camera and a smooth rocket-following chase camera.
- camera shake during ignition in both camera modes.
- an on-screen mission-status panel with countdown, altitude and velocity.
- visible fps, camera mode, light status and keyboard-control help.
- a default framebuffer target of 1280 x 720; the fps counter shows the actual rate during execution.
#### 4. Requirements
- С++11- compatible compiler. `makefile` uses `clang++` by default, support for an opengl 3.3 core-profile context.
- `make`, `pkg-config`, GLFW 3 and GLEW

## Build and Run
Run build and application commands strictly from the repository root.

#### For MacOS
Install the <b> Apple command-line tools</b> if they are not already available:

``` xcode-select --install ```

Finish the installation dialog before continuing. With <b>homebrew</b> installed, install the graphics dependencies:

``` brew install glfw glew pkgconf ```

<b>Clone, build and launch:</b>
```
git clone https://github.com/daaingkaryaad/cgf-final.git
cd cgf-final 
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

To follow a launch, press `c`, then space. After ascent, press `h` to inspect the launch site. Press `r` to reset; press `c` again if chase mode is wanted for the next launch.

## Checks and runtime options

Run the core simulation + transformation + camera and geometry checks:
```
make test
```

Check application startup and shader compilation in a hidden window:
```
make check-render
```

| Option | Effect |
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

| Asset | Creator and source | License / Attribution status |
| --- | --- | --- |
| Rocket, boosters, launch-site structures, terrain and scene props | Project team. Procedural geometry in `src/Geometry.cpp`, scene assembly in `src/Scene.cpp` and `src/SiteDetails.cpp` | Generated for this project, so no external model asset license applies |
| Concrete surface texture | Project team. Pixels generated in `src/Texture.cpp` | Generated for this project, so no downloaded texture image |
| Fire, smoke and stars | Project team. Particle implementation and the programs in `shaders/` | Project-specific effects, so no imported particle sprites or skybox images |
| Interface lettering | Project team. Bitmap glyph data defined in `src/Hud.cpp` | Project-specific glyph data, so no external font file |

#### Adapted course material
Parts of the vector/matrix utilities, the original phong-reflection approach, and the initial window/build setup were adapted from the course assignment 4. They were extended with the launch hierarchy, world-space lighting, additional light sources, fog, camera behavior and final-scene integration.

#### External libraries

| Dependency | Authors/Project | Goal | License and source |
| --- | --- | --- | --- |
| GLFW | Marcus Geelnard, Camilla Lowy and GLFW contributors | Window creation, input and graphics-context setup | [zlib/libpng license](https://www.glfw.org/license.html) and [glfw project source](https://github.com/glfw/glfw) |
| GLEW | Milan Ikits, Marcelo E. Magallon, Lev Povalahev and GLEW contributors | Loading opengl entry points | [glew upstream license](https://github.com/nigels-com/glew/blob/master/LICENSE.txt) and [glew project source](https://github.com/nigels-com/glew) |

These libraries are installed as system dependencies. Their authors retain their respective copyright and license terms.

## Technical references
- [glfw window and framebuffer guide](https://www.glfw.org/docs/latest/window_guide.html) for GLFW contributors.
- [glew initialization guide](https://glew.sourceforge.net/basic.html) for GLEW contributors.
- [apple command-line tools installation](https://developer.apple.com/documentation/xcode/installing-the-command-line-tools) for Apple.
- [homebrew glfw package](https://formulae.brew.sh/formula/glfw) for Homebrew contributors.
- [homebrew glew package](https://formulae.brew.sh/formula/glew) for Homebrew contributors.
- [homebrew pkgconf package](https://formulae.brew.sh/formula/pkgconf) for Homebrew contributors.

## AI use

Claude AI was used to assist with organizing and structuring the report PDF, finding special symbols and characters not easily accessible on the MAC keyboard (→, ≥, ≤ etc), and planning the structure of the demonstration video. ChatGPT was used to organize the references, find the sufficient websites and define their usage in our project.