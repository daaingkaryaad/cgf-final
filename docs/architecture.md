# Project architecture

The application is split into small components so that simulation logic, rendering, and input handling remain separate.

## Main components

- `main.cpp` creates the OpenGL window, parses command-line options, registers input callbacks, and runs the frame loop.
- `Rocket` owns the launch state machine and calculates altitude and velocity from elapsed simulation time.
- `Camera` stores the viewer position and orientation and handles movement and mouse look.
- `Scene` creates the environment and rocket meshes, updates the exhaust system, and coordinates scene rendering.
- `ParticleSystem` manages particle emission, lifetime, motion, and instanced rendering.
- `Mesh` and `Geometry` provide reusable mesh data and primitive geometry.
- `Shader` loads GLSL programs and updates shader uniforms.
- `Texture` loads texture resources used by scene materials.
- `Hud` draws launch information and control status on screen.

## Frame update order

Each frame processes window events, calculates a bounded time step, updates camera movement and rocket simulation, updates exhaust particles, renders the 3D scene, and finally renders the HUD. Keeping simulation updates time-based makes motion less dependent on the frame rate.
