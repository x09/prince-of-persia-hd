# Playable DOS-based HD mode
From the project root: make run-hd.
Direct: cd dos && ./prince --hd
F10 compares DOS/HD during the same game. ./prince keeps the native DOS view.
Window1280x720, internal scene1280x800, original game aspect8:5.
Simulation, collisions, controls, AI and audio stay in the native engine.

## Coverage
Every one of the 827 native game PNG resources has an accepted HD replacement.
The main 100 glyphs and built-in menu font use antialiased DejaVu outlines
with unchanged native text metrics. Font support is compiled C stb_truetype;
only SDL2/SDL2_image and libm are linked. No extra packages were installed.

All 216 hero/mouse animation resources remain covered (226 valid hero frame
entries), plus the ordinary guard set. The latest character pass adds205
SKEL/FAT/VIZIER/SHADOW/princess/story-Jaffar resources:190 new nonempty poses
and15 preserved native blank frames.
Titles, princess-room background, carpet, UI, potions, hourglass, sand,
remaining environment masks, mirrors, spikes and trap pieces are included.

Coverage is not the same as entirely newly painted artwork:
characters and major backgrounds are new paintings; precise structural
pieces retain original masks with higher-resolution materials/antialiasing,
and small props/effects include procedural drawings. See HD_ARTWORK_STATUS.md.

## Renderer
Parallel RGBA composition preserves native buffers, clipping, peels, dirty
copies, mirrored/padded character bounds, fades and F10 comparison.
Opaque DOS drawing of transparent title art uses HD alpha.
Cutscene effects restore a cached HD background before actors/effects draw,
avoiding black rectangles and repeated alpha-edge accumulation.
Font recolouring, outline and XOR operations retain HD image data.
The carpet contains no baked-in princess: actors animate separately.
--hd-preview remains a separate art stand; --hd is the playable game.

## Build and checks
make hd-assets
make all
make hd-game-check
make hd-guard-scene-check

hd-game-check includes all827 PNG bindings/native-pixel integrity, all new
characters/mirrors/blanks, fonts, alpha layers, previous hero/torch/gate/peel
regressions, fifteen level starts and scripted movement on levels1/2.
build/hd-release-check.log is the current combined check log.
Source atlases, manifests, prompts and licenses are documented in
HD_ARTWORK_STATUS.md. Keep data/ beside prince; the outline font is bundled
under data/hd/fonts with its license.

## Limits
Generated poses still need temporal/anatomical and hand/sword alignment polish.
Ordinary guards share one HD costume palette. Some environment/trap details
are reconstructions rather than newly hand-painted shapes.
Custom levelsets bypass base-game HD replacements.
`make campaign-full-check` now proves one complete input-only route through
levels1–14, Shadow union, Jaffar's defeat, the win room and ending. The route
uses two normal level restarts; no deaths or intermediate state loads.
DOS/HD gameplay traces match across10052 ticks. See CAMPAIGN_CHECK.md.
Alternative routes and every possible overlap/transition remain outside
that proof. See artwork test results in HD_ARTWORK_STATUS.md.
Apple II and upstream/ directories are preserved.
