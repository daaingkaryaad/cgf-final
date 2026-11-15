# Booster separation: Topic 9, Baikonur Rocket Launch

Four side boosters ride the central rocket through ignition and early ascent.
They release together at 54 scene units above the pad, coast upward briefly,
then drift, tumble, and fall while the core continues to climb. This sequence
demonstrates hierarchical modeling, timed animation, and independent motion of
separate objects in the OpenGL launch scene.

## Timing and state

`Rocket::update(dt)` advances a launch clock using elapsed simulation seconds.
After the 3.0-second countdown and 1.2-second ignition, the core rises with
constant acceleration of 1.1 scene units/s². Separation is scheduled at
`4.2 + sqrt(2 * 54 / 1.1)`, about 14.11 seconds after launch is triggered.
`boostersSeparated()` becomes true at that time and stays true until
`reset()`. An update that crosses separation uses only the time past the
threshold for the detached pose. A very large finite update can cross several
events at once; nonpositive and nonfinite updates are ignored.

The core stops at 90 scene units at about 17.00 seconds. The boosters keep
moving until ground contact at about 17.84 seconds. The simulation finds
that time once by searching the descending trajectory for where the lowest
point of the rotated booster envelope reaches the terrain plane at
`y = -0.025`. The envelope approximates the nozzle, skirt, shell, nose, and
fin; it does not collide with site props. All four boosters share the same
contact time because their mounting yaws do not change height.
`boostersVisible()` becomes false at impact, and the stored pose stops at
contact even if an update spans the whole descent. Reset returns all four
boosters to their pad mounts and allows another launch.

## Model transforms and rendering

For booster index `i`, let `theta = i * pi / 2` and
`d = (cos(theta), 0, sin(theta))`. This places the boosters on the four
cardinal directions in the XZ plane. While attached, `Rocket::boosterModel(i)`
returns the world matrix `coreModel * T(1.38 * d) * Ry(-theta)`. The yaw
points each fin away from the core.

After release, let `a` be seconds since separation and
`e = a² / (a + 0.12)`. The center moves along `d` at radius
`1.38 + 4.5 * e`. Its height is
`0.65 + 54 + 3.2 + vRelease * a - 0.5 * 14 * a²`, where
`vRelease = 1.1 * sqrt(2 * 54 / 1.1)`. It therefore inherits the core's
upward velocity, then falls under the demo's constant downward acceleration.
The derivative of `e` starts at zero and rises toward one, so outward motion
and rotation start smoothly. The radius keeps increasing after release.

The detached world matrix is
`T(center) * Ry(-theta) * Rz(-1.25 * e) * Ry(0.32 * e) * T(-pivot)`,
with the local pivot at `(0, 3.2, 0)`. Shifting by the pivot before
rotating keeps the tumble centered on the booster. At release the two pose
formulas agree. All four boosters use the same motion rotated to their
respective mounting angles.

`Scene::buildBoosters()` defines one assembly of lit parts, separate from the
core geometry. An upper orange band pairs with the lower band, and two dark
lugs on the inward side bridge each booster shell to the core while attached.
The bands, fin, and lugs all stay with the booster on release. The new parts
reuse the cylinder and box meshes and fit inside the contact envelope.
`Scene::drawBoosters()` draws four instances, multiplying each booster's
world matrix by each part's local matrix. The existing shader uses
that model matrix for positions and its inverse transpose for normals. The
emissive nozzle glow uses the same booster pose while the engine is on and
the booster is attached; the physical nozzle remains part of the detached
assembly. Scene rendering reads visibility and pose directly from `Rocket`,
so there is no separate render state to reset.

## Verify and demo

1. Run `make test`. The booster checks cover all four mount positions and
   fin directions, attachment, exact release and pose continuity, long
   updates, symmetry, smooth and monotonic outward motion, frame partitioning,
   visibility after core stop, terrain contact with fine and coarse updates,
   reset, and relaunch.
2. With a C++ compiler, `pkg-config`, GLEW, and GLFW installed, run `make`
   and then `./baikonur` from the repository root.
3. Press **Space** and watch the attached ascent. Use **E** to rise and
   click or press **Tab** to aim with the mouse if the rocket leaves the
   starting view. At roughly 14 seconds the four boosters peel outward;
   follow them as they tumble and fall while the core climbs. **R** restores
   the pad view and all four attached boosters; press **Space** again to repeat.
4. For an OpenGL smoke check from the default view, render hidden frames on
   the pad, during ignition and ascent, during separation and the final fall,
   and after impact:

   ```sh
   ./baikonur --hidden --frames 1 --capture /tmp/baikonur-pad.ppm
   for time in 3.5 7.0 14.9 17.78 17.9; do
       ./baikonur --hidden --frames 1 --simulate "$time" \
           --capture "/tmp/baikonur-$time.ppm" || break
   done
   ```

AI assistance disclosure: OpenAI Codex inspected the existing booster
implementation, made the follow-up changes to the animation, terrain contact,
and booster details, added tests, ran the build and verification commands, and
updated this document. The original booster implementation and the rest of the
team's project were already present.
