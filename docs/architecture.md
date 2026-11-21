# Project architecture

The application is split into small components so that simulation logic, rendering, and input handling remain separate.

## Main components

- `main.cpp` creates the OpenGL window, parses command-line options, registers input callbacks, and runs the frame loop.
- `Rocket` owns the launch clock, altitude, velocity, booster separation and settling poses, and service-arm timing.
- `Camera` stores the free and chase views, handles movement and mouse look, and applies ignition shake.
- `Scene` creates the environment and rocket meshes, updates the exhaust system, and coordinates the shadow and scene passes.
- `SiteDetails.cpp` builds the service vehicle, workers, equipment and retracting arm as procedural assemblies.
- `ParticleSystem` manages particle emission, lifetime, motion, and instanced rendering.
- `Mesh` and `Geometry` provide reusable mesh data and primitive geometry.
- `Shader` loads GLSL programs and updates shader uniforms.
- `Texture` generates the concrete texture and configures repeat wrapping and mipmaps.
- `Hud` draws launch information and control status on screen.

## Frame update order

Each frame processes window events and camera input, then advances the rocket, exhaust, chase camera and ignition shake in simulation steps of at most 1/60 second. The renderer draws the directional shadow map, sky, lit scene, transparent particles and HUD in that order. Opaque scene geometry is shared by the shadow and colour passes, including the moving service arm and workers while they are visible.

The simulation uses elapsed time rather than a fixed movement amount per rendered frame. Gaps longer than one second are treated as pauses. Minimized windows and zero-size framebuffers skip rendering and reset the FPS sampling interval, preventing an inactive window from distorting the displayed rate.
