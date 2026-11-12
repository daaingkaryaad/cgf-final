# Baikonur Rocket Launch Simulation

A C++ and OpenGL 3.3 project that visualizes a rocket launch, including a launch pad, camera controls, a status HUD, and animated fire and smoke exhaust particles.

## Features

- Rocket launch sequence with ready, countdown, ignition, ascent, and complete states.
- Time-based rocket movement and camera controls.
- Instanced fire and smoke particles emitted from the rocket engines.
- Textured 3D scene, sky, lighting, and launch status HUD.
- Command-line options for headless frame rendering, simulation, and screenshots.

## Build and run

The project uses GLFW, GLEW, OpenGL, `pkg-config`, and a C++11-compatible compiler. See [docs/build-and-run.md](docs/build-and-run.md) for platform notes and commands.

```bash
make
make run
```

## Controls

See [docs/controls.md](docs/controls.md) for keyboard and mouse controls.

## Project structure

- `include/` — class declarations and shared math utilities.
- `src/` — application, scene, rocket, camera, mesh, shader, HUD, and particle implementations.
- `shaders/` — GLSL vertex and fragment shaders.
- `tests/` — core logic tests.
- `docs/` — project and development notes.

## Tests

Run the core tests with:

```bash
make test
```
