# HD renderer corrections — 2026-10-19
Reports: fire retained fragments of the hero, opening gate had artifacts, turning hero became wider.

## Opaque copies and saved backgrounds
Native RGB/indexed surfaces have no alpha. HD RGBA replacements previously copied hidden RGB and transparent alpha into these buffers on opaque blits. Later blending/copying a saved background could skip those pixels and retain old content.
The shadow destination now follows the native no-alpha contract: alpha is flattened over black for opaque copied regions and becomes 255. Transparent sprite blits still alpha-blend normally. Native pixels are unchanged.
Regression uses all nine actual flame frames, overlapping foreground, saved background restore and redraw: 16998 incorrect pixel comparisons before, zero after.

## Undistorted poses and bounds
All 216 KID animation resources are repacked with ONE isotropic scale from pose height. Horizontal and vertical scaling can no longer differ.
Each HD canvas has an extra 64 HD pixels (16 logical pixels) on each side. Actor positions, native image dimensions, collision geometry and simulation stay unchanged.
The compositor maps the padded source to the original anchor; saved-background peels, dirty copies, screen clipping and mirrors cover the overhang.
Test verifies an actual sprite extends outside the old rectangle, the extended image reaches the onscreen buffer and restoration removes it completely.
Packing: tools/hd_fit.h, tools/pack_hd_hero.c and active base builder tools/build_hd_assets_isotropic.c.
Generated source art is unchanged. Artistic differences between generated poses still need manual polish.

## Gates and runtime validation
64 opening positions of original gate slices match native output pixel for pixel.
Test-only hd_scene_driver creates an in-memory gate in the starting room, starts native gate animation and captures seven intermediate frames. No level files are modified.
Gate modifier progressed 12,44,76,108,140,172,234; 80 rendered frames, zero HD failures/fallbacks. Intermediate captures visually inspected.
Real X11/PulseAudio movement, jump, turn and F6 comparison passed; screenshots after crossing flames show no retained head/body fragments.
All 15 level starts loaded with all animation resources bound and no native hero fallback.
ASan+UBSan with leak detection passed torch, gate, overhang/mirror/peel and core rendering tests.

## Commands and evidence
Run: make run-hd
Regression: make hd-artifact-check
Gate scene: make hd-gate-scene-check
Full HD smoke: make hd-game-check
Logs: build/torch-before.log, build/torch-after.log, build/artifact-regression.log, build/artifact-game-check.log, build/gate-fixture.log, build/overlap-live.log, build/test_hd_*-san.log.
Runtime captures: build/overlap-live-1.png through -4.png; gate captures build/gate-fixture-08.png through -56.png.
This is render validation, not full campaign completion.
