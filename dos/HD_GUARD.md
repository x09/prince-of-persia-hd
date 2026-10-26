# Ordinary guard HD pass — 2026-10-19

The ordinary GUARD.DAT character now has 31 newly drawn full-colour poses.
GUARD resources 753–784 are replaced, including transparent unused 776.
UI resources 751/752 remain native. New art is loaded only in --hd mode.
The renderer accepts padded guard bounds, preserving isotropic proportions,
native pixels, collision geometry, animation tables and game sword offsets.

Sources: data/hd-source/guard-{0,1,2}.png, created with the built-in imagegen tool.
Pack: make hd-guard-pack. Manifest: data/hd/guard-manifest.txt.
The pose sheets use empty hands because the native game draws weapons separately.
make hd-swords-pack reconstructs all 34 PRINCE 701–734 sword axes with
analytical antialiasing and steel shading. These are procedural reconstructions,
not separately painted detailed weapons. Native weapon bounds remain unchanged.
make hd-assets reproduces guard and sword output together with previous assets.

Validation:
- make hd-guard-check: every guard binding, transparency, exact horizontal
  reflection, unchanged native pixels, frame-table coverage; all 34 sword
  bindings, bounds, coverage and antialiased edges.
- make hd-game-check: prior hero/torch/gate/peel regressions, starts of levels
  0–14, scripted 10-second input runs on levels 1 and 2.
- make hd-guard-scene-check: test-only level-2 encounter, native guard AI and
  drawn sword; captures build/guard-scene-*.png and logs observed frames.
  This modifies only memory, never level files. It is not a complete battle
  or playthrough verification.
- ASan/UBSan guard binding/mirroring check: build/guard-asan.log.
- Gallery: build/guard-complete-gallery.png.

Known limits:
- Ordinary guards currently share one purple/burgundy costume; native palette
  variants are not yet applied to these full-colour drawings.
- FAT, skeleton, vizier and shadow now have separate HD sets; see HD_ARTWORK_STATUS.md.
- Generated poses need further animation polish; subtle anatomy and hand-to-blade
  alignment can vary. Native weapon offsets alone do not guarantee exact grip
  alignment for every new drawing.
- This guard pass does not complete all game graphics or verify all-level passage.

Run on lenovo:
cd /home/anton/Work/remote-commander-for-chat-gpt/Prince-of-Persia/dos
./prince --hd
F10 compares with DOS graphics. Keep data/ beside the executable.
