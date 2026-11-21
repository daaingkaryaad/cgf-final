# Core tests

The tests in `tests/core_tests.cpp` and `tests/booster_landing_tests.h` cover simulation and geometry logic that can be checked without launching the graphical application.

Current test groups include:

- **Rocket launch:** initial state, countdown, ignition, ascent, reset behaviour, completion, and frame-rate-independent altitude.
- **Booster separation and landing:** attachment, release continuity, falling motion, terrain contact, horizontal settling, clear landing footprints, persistent resting poses, reset and relaunch.
- **Service arm:** smooth countdown retraction, clearance before ignition, frame-rate-independent timing and reset.
- **Transforms:** parent-child transform propagation and beam endpoint placement.
- **Camera:** frame-rate-independent movement, ground-height limits, finite view values after large mouse input, and home-position reset.
- **Mesh geometry:** triangle indices, unit normals, UV coordinate ranges, non-degenerate triangles, and outward-facing winding.

Run the suite from the repository root:

```bash
make test
```

A successful run prints the number of checks that passed. If a check fails, the test executable prints the failing condition and exits with a non-zero status.

## Rendering Smoke Check

With the graphics dependencies and a working desktop graphics session, run:

```bash
make check-render
```

This creates a hidden OpenGL window, compiles the shaders, renders one frame and exits. It checks startup rather than interactive behavior or sustained performance.

For the final manual check, launch with `make run`, verify both camera modes and the pad-light toggle, start a complete launch, return home with `H` to inspect all four landed boosters, and reset with `R`. Check that workers return, the arm reattaches and exhaust clears. Watch the FPS counter at 1280 × 720 through ignition and ascent; measurements depend on the machine and graphics driver.
