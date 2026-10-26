# Persiality — developer notes (not an in-game hint)

Hidden input: I D D Q D, lower/upper case (Shift/Caps Lock accepted), maximum
2.5 s between letters. Prefix keys are consumed before megahit cheats, so
Shift+I cannot invert the screen. Ordinary sword-ready frame 171 is accepted.
Only HD-visible gameplay with a living ordinary/heavy guard, Skeleton or
Jaffar in the same room/floor. The Shadow is excluded: level 12 requires reunion.
Disabled for DOS view/native DOS, Shadow/dead/offscreen enemies, dead hero,
menus/pause, recording/playback, reversed screen, hanging/falling and stairs.

The three-second scene freezes simulation/player input, hides the native hero,
shows sixteen isolated key poses and a gold Persiality wordmark, strikes at
about 1.3 seconds using native guard death logic, then restores the exact hero
state and controls.
Skeletons are dispelled during the flash, without nonexistent corpse frames;
their room entry is removed to prevent resurrection on revisiting. Jaffar uses
the native victory event, including the final exit, sound and flash.
One short potion-like flash is clipped to the game canvas;
render color/blend state is restored and letterbox clears are explicitly black.
Window close remains available. No new event added to the native replay format.

Current art: data/hd/PERSIALITY/pose00.png through pose15.png, each 576x384,
transparent padding, shared scale and foot anchor. Title: title.png.
Exactly ONE pose per presentation, at full texture alpha: NO pose crossfades,
NO runtime atlas crops. This prevents double silhouettes, neighbor fragments
and rectangular cuts seen with the previous crowded eight-pose sheet.
Sixteen actual drawn poses replace opacity interpolation. Additional display
updates during timer waits request a 16 ms interval; native simulation stays
unchanged. Spell overlay may cover foreground decorations.

Repack: make -C dos hd-persiality-pack. C tool finds clear source grid gutters,
rejects touching source silhouettes, and writes individually padded frames.
Source art/provenance/prompts: data/hd-source/PERSIALITY_PROMPTS.md.
Old eight-pose atlas is retained only as a historical source/reference.

Checks: make persiality-check. Test-only isolated room, both facings/cases,
forbidden contexts, slow typing in combat, exact actor freeze/restoration,
native enemy death, blocked controls, black physical borders, short duration,
all sixteen poses, and exactly one full-opacity independent sprite draw/frame.
Additional check: make persiality-special-check, real level 3/6/12/13 resources
in isolated encounters; persistent skeleton removal, Jaffar's exit and exclusion
of the Shadow without changing the reunion state are asserted.
