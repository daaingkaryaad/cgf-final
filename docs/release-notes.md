# Baikonur Rocket Launch v1.0

An interactive C++11 and OpenGL 3.3 core rocket-launch simulation for Computer Graphics Fundamentals, Topic 9, SE-2424.

## Included Features

- Countdown, retracting service arm, engine ignition, ascent and reset.
- Four independently animated boosters that separate, fall, settle and remain around the launch site.
- Instanced fire and smoke, altitude-based sky darkening and procedural stars.
- Free and chase cameras with ignition shake.
- Directional sunlight and shadows, a toggleable pad light and ignition lighting.
- Procedural launch-site structures, workers, service vehicle and equipment.
- Generated concrete texture with UV mapping, repeat wrapping and mipmaps.
- Mission status, altitude, velocity, FPS and control help.

## Build and Run on macOS

Install the Apple command-line tools and Homebrew first, then run from the repository root:

```bash
brew install glfw glew pkgconf
make check-gl
make
make test
make check-render
make run
```

`make test` runs 4,842 core checks. `make check-render` requires a working graphics session and renders one hidden frame.

Press `C` for chase mode and `Space` to launch. Press `H` to return to the launch site, `L` to toggle the pad light and `R` to reset.

The release contains source code. Build dependencies are installed separately. Full setup instructions, controls and contributions are in `README.md`; asset and dependency attribution is in `CREDITS.md`.
