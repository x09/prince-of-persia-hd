# Permanent title wordmark and credit attribution — 2026-10-20

Built-in imagegen, transparent output title-logo.png. SHA256 ae830d18315f97f9c1c55b59771d6ced5bf6ae61624d2a35bdda2506370cd7ab. Source painting title.png remains separate. tools/finish_hd_assets.c composites the cropped logo isotropically in the upper sky of TITLE/res51.png, centered at HD x=700, y=64, within 800x292 pixels. It is part of the background so title transitions cannot erase it.

Prompt:
Use case: logo-brand. Create a single premium cinematic game title wordmark on genuinely transparent background. Exact text, Latin letters: "PRINCE OF PERSIA". Two balanced lines: PRINCE OF on first line, PERSIA larger below. Lettering inspired by flowing Persian / Arabic calligraphy, but every word must remain plainly legible ENGLISH LATIN lettering, NOT Arabic script. Elegant elongated curved terminals, pointed nib strokes, restrained intertwined arabesque flourishes, heroic historical adventure character. Antique gold metal with subtle bevel, warm champagne highlights, dark bronze engraved shading, crisp refined contours. No large outer glow, no drop-shadow backdrop, no black rectangle, no scenery, no frame, no other words or symbols. Wide horizontal composition roughly 3.5:1. Entire wordmark and modest flourishes fully inside image with transparent margins. Suitable for the upper dark indigo sky of our Persian palace cinematic title screen. This is an original custom wordmark, not a copy of an existing game logo.

HD credit wording:
- Original game publisher: Broderbund Software.
- Original game by Jordan Mechner.
- Inspired by the original game.
- Unofficial Linux / HD fork - AI-assisted.
- Full page retains original 1990 attribution, all existing original staff names and roles; a separate fork section acknowledges inspiration, SDLPoP by David Nagy and contributors, and AI-assisted development and artwork.

Expanded HD credits display for 900 ticks rather than the original 360, allowing time to read. DOS comparison resources retain original content/timing. Playback and skip controls unchanged.

Checks: build/hd-title-credits-check.log (827 PNG bindings, transparency/layers), build/hd-title-credits-intro.log (27 second intro, audio output), build/hd-credits-page.log (actual engine credit rendering). Captures: build/hd-title-credits-4.png and build/hd-credits-page.png. Executable checksum: build/hd-title-release.sha256.
