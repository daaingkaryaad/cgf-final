# Exhaust particle effects

The exhaust effect is implemented by `ParticleSystem` and uses the particle shaders in `shaders/particles.vert` and `shaders/particles.frag`.

## Emission

The system receives five engine outlet positions from the scene. While the rocket engines are active, it emits two particle types:

- **Fire:** short-lived particles with a downward velocity and a warm colour range.
- **Smoke:** longer-lived particles that spread outward, rise over time, and become more transparent as they age.

Particle births are distributed along the movement path of each outlet between updates. This helps avoid visible gaps when the rocket moves between frames.

## Updating and rendering

The update step advances particle age and position, applies the smoke rise effect, and removes particles after their lifetime expires. The system limits the total number of active particles to avoid unbounded growth.

Rendering uses instanced triangles. Smoke particles are depth-sorted before drawing, while fire uses additive blending. Depth writes are temporarily disabled for the transparent particle passes and restored afterward.

Calling `reset()` clears active particles and emission timing so a restarted launch begins with a clean exhaust effect.
