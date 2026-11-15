# Core tests

The tests in `tests/core_tests.cpp` cover simulation and geometry logic that can be checked without launching the graphical application.

Current test groups include:

- **Rocket launch:** initial state, countdown, ignition, ascent, reset behaviour, completion, and frame-rate-independent altitude.
- **Transforms:** parent-child transform propagation and beam endpoint placement.
- **Camera:** frame-rate-independent movement, ground-height limits, finite view values after large mouse input, and home-position reset.
- **Mesh geometry:** triangle indices, unit normals, UV coordinate ranges, non-degenerate triangles, and outward-facing winding.

Run the suite from the repository root:

```bash
make test
```

A successful run prints the number of checks that passed. If a check fails, the test executable prints the failing condition and exits with a non-zero status.
