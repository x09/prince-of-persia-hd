# HD artwork status
Current native game: DOS / SDLPoP, Linux C + GCC + Make + SDL2 / SDL2_image.
No installed packages were added. Apple II and upstream/ were not changed.

## Coverage
All 827 PNGs in the eleven native game resource directories now have a
corresponding 4x HD resource accepted by the live renderer.
The 100 main font glyphs and built-in menu glyphs are rasterized from licensed
DejaVu Serif outlines using vendored C stb_truetype (no SDL_ttf dependency).
Native text widths, heights and layout remain unchanged.

This is resource coverage, not a claim that every image is a new painting
or that every animation is finished production-quality HD.

## New paintings
- Existing hero and ordinary guard HD sets remain active.
- SKEL, FAT, VIZIER, SHADOW, princess story/cutscene and story Jaffar:
  205 resources, 190 nonempty newly drawn poses and 15 native blank frames.
- New full-colour princess-room background, carpet/cushion foreground and
  castle title painting. The carpet contains no baked-in character.
- Background/foreground column sampling is matched at native x=60,y=120;
  the carpet preserves the native column opening x=64..73.
- Full and empty health indicators remain distinct (filled/outlined hearts).
- Text cards and title credits are redrawn from font outlines; original
  authors are retained.

## Precision assets
Thirteen prop images (seven hourglass states and six sword/flask images)
are separately painted and fit inside native ink bounds. The remaining
81 items/UI/effects/trap resources are procedural full-colour
drawings or antialiased material reconstructions of original geometry masks.
This includes hearts, bubbles, sand, metal pieces,
mirror fragments, spikes and narrow trap parts. These are not AI-painted
individual textures. Existing dungeon/palace surfaces likewise retain
original structural masks with higher-resolution material detail.

## Rendering
Expanded character bounds preserve proportions and participate in dirty
copies, mirrors and peel restoration. Native game pixels and collisions
remain unchanged. HD foreground layers preserve alpha even when original
DOS drawing uses opaque blits (e.g. title overlays and cutscene flames).
Monochrome copies retain HD alpha; outline/XOR operations preserve the HD
buffer rather than resampling the entire screen from native pixels.
F10 switches the comparison view. Window 1280x720; scene canvas1280x800.

## Reproduction and tests
make hd-assets
make all
make hd-game-check
make hd-guard-scene-check

Source atlas layout: tools/hd_characters.h.
Canonical precision-resource list: tools/hd_remaining.tsv.
Generated pose manifest: data/hd/characters-manifest.txt.
Sources and generation records: data/hd-source/CHARACTER_PROMPTS.md,
SCENE_PROMPTS.md, PROP_PROMPTS.md, GUARD_PROMPTS.md, HERO_PROMPTS.md.
Built-in imagegen was used, no CLI/API fallback.
Outline font license: data/hd/fonts/LICENSE.txt.
stb_truetype v1.26 with its embedded MIT/public-domain license:
tools/vendor/stb_truetype.h (source https://github.com/nothings/stb).

Tests cover every PNG binding/dimension/native-pixel integrity, every new
character mirror/alpha/blank frame, font AA/colour, alpha-layer regression,
XOR/outline preservation, prior fire/gate/peel regressions, fifteen level
starts, scripted movement and test-only enemy encounters. These checks
do not prove complete playthroughs or every possible sprite overlap.

## Remaining quality work
- Some poses still vary in anatomy or hand-to-sword alignment.
- Ordinary guards currently share one costume palette.
- Precise structural parts use reconstructed masks, not entirely new drawings.
- Scene layout, typography and artistic continuity differ from the DOS art.
- Custom mod resource sets are deliberately not replaced with base-game art.
- F10 comparison, menus and rare overlaps still merit longer manual playtests.

## Validation results for this build
- Full asset rebuild from saved sources: build/hd-repro-check.log.
- Final combined checks: build/hd-complete-final.log.
- 827 game PNG bindings accepted; 205 new character resources
  (190 nonempty,15 blank);100 main font glyphs plus built-in menu glyphs.
-15 level starts and scripted movement on levels1/2.
- Level3 skeleton,6 heavy guard,12 shadow,13 vizier native AI encounter
  fixtures: observed10,8,8,8 armed/same-room samples respectively.
  Logs build/hd-enemy-{3,6,12,13}.log. These are not victories/playthroughs.
- X11/PulseAudio title run: build/hd-title-live.log, nonzero PCM observed.
- Final cutscene capture: build/hd-room-final-{1,2,3,4}.png.
- Renderer/resource ASan+UBSan: build/hd-coverage-final-asan.log,
  build/hd-layers-final-asan.log.
- Executable checksum: build/hd-release-sha256.txt.
- make hd-enemies-check reproduces the test-only enemy fixtures.

## Guard weapon audit — 2026-10-20

See HD_GUARD_MOVEMENT_AUDIT.md. Added per-pose HD weapon attachment offsets for five enemy types and both facing directions, expanded background restoration around shifted blades, and replaced the HD Shadow OR/XOR colour-noise effect with translucent artwork. Native combat geometry and DOS XOR rendering are unchanged. The grip regression covers 220 type/pose/facing combinations; exhaustive engine-render fixtures additionally cover recovery, falls and death. Final logs: build/hd-grips-final.log, build/hd-grips-pose-final.log and build/hd-grips-asan.log. Fingers/hand shapes and temporal consistency still need artwork refinement; weapon sprites still use reconstructed native blade axes.

## Floor bone pile — 2026-10-20

Replaced VDUNGEON resources 230/231 with a newly painted skeleton bone pile, not a scaled native mask. Source and imagegen prompt: data/hd-source/bones.png and BONES_PROMPT.md. tools/pack_hd_bones.c uses one continuous image split over the existing two floor pieces, preserving their native bounds and perspective. Full make hd-assets runs this packer last. Coverage remains 827 accepted PNGs; reproducibility check: build/hd-bones-repro.log. Test fixture tests/hd_bones_scene_driver.c exercises the original level-3 skeleton wake event (tile 21 -> floor 1 and skeleton character 4); make hd-bones-check records screenshots before/after. Native resources and upstream are unchanged.

## Title logo and attribution — 2026-10-20

Permanent gold arabesque Prince of Persia wordmark added to the upper sky of the HD opening background. The animated title cards now explicitly label the original publisher and creator, and describe this version as an unofficial Linux / HD fork inspired by the original game, AI-assisted. Full credits retain original game attribution (1990, Jordan Mechner, DOS staff) and separately credit SDLPoP and AI-assisted development/artwork. HD credits dwell time increased from 360 to 900 ticks. Sources/prompt: data/hd-source/title-logo.png and TITLE_LOGO_PROMPT.md. Tested: 827 resource bindings and HD layers passed; 27-second title playback produced nonzero audio; tests/hd_credits_scene_driver.c rendered the full credits via the actual engine mono-text path without clipping/errors. Logs and captures: build/hd-title-credits-check.log, build/hd-title-credits-intro.log, build/hd-credits-page.log and build/hd-credits-page.png. Latest executable checksum: build/hd-title-release.sha256.

## Dungeon stone correction (2026-10-21)

Replaced the scratch-bearing blue material with dungeon-stone-matte.png and
removed the repeated 210x220 crop from dungeon sampling. Grain contrast is
reduced to 40%. Targeted make hd-dungeon-material repacks 99 VDUNGEON resources
while preserving painted bones and precision props. Original atlas, palace,
characters, native resources and simulation remain unchanged. All 99 replaced
PNGs retain identical dimensions and alpha masks. Prompt and provenance:
data/hd-source/DUNGEON_MATERIAL_PROMPT.md.

## Climbing and dungeon architecture correction (2026-10-21, alt7)

KID resources 546–561 now use a dedicated pose sheet matching the native
climbing sequence, including the dangling-leg phase before the hips clear
the ledge. Original generic poses tucked the legs too early. No collision,
sequence, actor position or clipping code changed. `pack_hd_hero --climb-only`
rebuilds these frames; the regular hero pack includes the correction last.

VDUNGEON 237–240 and 286–291 use new painted modular exit-frame and column
artwork. Resources 233/234 use a painted gate pattern with a 16-HD-pixel
vertical period matching the engine's overlapping four-native-pixel slices.
`make hd-architecture-pack` reproduces all twelve resources; `make hd-assets`
runs it after the general material packer. Native image dimensions stay
unchanged. This is targeted artwork replacement, not a claim that every
environment tile has been newly painted. Sources and exact prompts are in
`data/hd-source/CLIMB_ARCHITECTURE_PROMPTS.md`.

## Column joins correction (2026-10-22, alt8)

The alt7 local 32-HD-pixel alignment strip introduced another contour step
at its far edge. The packer now fits every scanline of all five column
pieces to a continuous interpolated native profile, including the base.
This replaces local join patches; the complete straight shaft retains
constant width. Shared shaft texture phase is preserved. No new generation,
engine or gameplay changes. `make hd-architecture-check` now also checks
shaft contours throughout both halves and limits color bands at all three
joins. Closed, half-open and open fixture captures and 827 resource bindings
passed. Preview and logs: release/verification/column-joins (outside archive).

## Pedestal composition correction (2026-10-22, alt9)

Native foreground resource 288 is an exact crop of resource 286 at x=8
native pixels. Independent HD drawings broke this identity. The base is now
composed from one painted column body and the normal floor 241; foreground
288 overlays the identical body at the same coordinates. Rear base 287 uses
the same body, the native perspective profile and floor 242. Its old diagonal
platform-side shadow is no longer stretched into the column painting.
Regression verifies opaque pedestal pixels across both overlapping images
and the unchanged surrounding floor. Architecture scene, column profiles,
827-resource coverage and reproducibility pass. No gameplay changes.

## Rear pedestal shape (2026-10-22, alt10)

The old rear tile combined column and diagonal platform silhouettes. Applying
that silhouette to the painted foot truncated it into a wedge. Rear body 287
now has a complete centered foot, using the existing round painted body in
a narrower depth profile with the unchanged 44px shaft anchor. Platform 242
is composed independently; the full foot ends before the side of the platform.
Tests require both sides of the foot around its center, unchanged floor below
it, the front overlay identity and shaft continuity. Architecture scene,
827-resource coverage and deterministic repacking pass. No gameplay changes.

## Loose-floor and climb rendering (2026-10-22, alt11)

HD draw_game_frame rebuilds changing room backgrounds while visible loose
slabs are active and on the following frame. This invalidates adjacent wall
areas outside native dirty rectangles that may intersect wider HD peels.
Input events and exit_room_timer are preserved during these extra refreshes.
The first attempted refresh reset that timer; replay parity detected it and
the final implementation restores it.

Native partial climb masks 232/350/351 end beside the narrow DOS actor. HD
composition extends their right edge by four native pixels at the same height
to cover the wider dangling HD limb without hiding the torso. Original game
buffers and PNGs remain unchanged. A real level-2 room-4 fixture checks falling
slabs together and in sequence, adjacent wall pixels, and the reproduced
right-facing climb frame (zero exposed foot pixels, visible upper body).

Full campaign: 10052 ticks, HP=4, Shadow/Jaffar/ending PASS and identical
DOS/HD state traces. Native smoke/asset tests, 827 bindings, HD overlap/peel
tests and the finishing scene passed. The latter check also exposed first-pose
time lost to initial rendering; scene timing now starts after preparing its
first visible pose, and the unchanged test confirms all sixteen poses in both
directions. Verification: release/verification/floors-alt11 (outside archive).

## Climb overlay surface continuity (2026-10-22, alt12)

The old partial climb tile was independently textured, and repeating its last
column exposed a rectangular patch on the floor in other animation phases.
The compositor now snapshots the completed background after backtable draws,
before actors. Climb masks 232/350/351 restore those existing background
pixels instead of painting the partial tile's RGB. Extensions cannot add
stone where the saved background is empty. This preserves texture alignment
and the physical floor silhouette while keeping padded HD limbs occluded.

Tests use all three masks, textured backgrounds, empty space and both viewport
corners. The level-2 fixture exercises ascending and descending sequences
with simultaneous/staggered falling slabs. Full campaign and ending passed
with identical DOS/HD state traces. AddressSanitizer checked edge clipping
with leak detection disabled because LeakSanitizer is unsupported under the
execution environment's ptrace. No sprite PNGs or gameplay rules changed.
