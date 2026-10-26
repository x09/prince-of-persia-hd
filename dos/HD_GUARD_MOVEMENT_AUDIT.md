# HD guard movement and weapon audit — 2026-10-20

Reviewed ordinary guard, fat guard, skeleton, Jaffar and Shadow. Native frame/sword tables remain the source of movement and blade angle; no collision, reach, AI or timing changes.

Fixed:
- Floating blades and incorrect grip positions caused by fitting separately painted bodies to native frame bounds. Added per-character/per-pose visual offsets in src/hd_weapon.h; horizontal correction mirrors with facing direction.
- Offsets follow the native object table into the midtable, so object sorting and deferred drawing keep the correct owner. A draw scope limits corrections to one image and its HD surface. DOS pixels remain unchanged.
- Background peels include both original and corrected weapon bounds, including vertical displacement.
- Shadow's legacy OR/XOR double pass corrupted full-colour artwork into multicoloured noise. HD now draws a translucent painted figure and suppresses the second HD XOR pass. Native XOR still executes. Generic XOR effects retain their original path.

Validation:
- make hd-grips-check: 220 armed pose/type/facing combinations. Exact translated HD pixels, byte-identical native pixels, cleanup of shifted weapon bounds, and no offset leaking to subsequent draws. Also tests native XOR execution while HD Shadow painting remains intact.
- make hd-guard-poses-check: actual game renderer, frames 150..185 plus fall frames 102..106, both directions, only resources present for each character. 376 valid frame/direction cases: ordinary guard 80, skeleton 68, fat guard 78, Shadow 74, Jaffar 76. Some guard frames share the same body resource; absent/blank engine entries are excluded. These are controlled rendering fixtures, not 376 combat victories.
- make hd-enemies-check: short live AI encounters for skeleton, fat guard, Shadow and Jaffar.
- HD layer, coverage and UI checks; 827 PNG bindings.
- AddressSanitizer/UBSan instrument the HD compositor/font and exercise the grip regression; other engine objects are linked from the regular build.

Reports: build/hd-grips-final.log, build/hd-grips-asan.log, build/guard-engine-poses.tsv, build/guard-engine-level*.log. Contact sheets: build/guard-audit-*.png (native/HD pairs) and build/guard-engine-level*-page*.png (actual engine frames; adjacent left/right views).

Known limitations: painted poses still vary somewhat in anatomy, costume and hand shape; some grips are open hands rather than wrapped fingers. Thin blades remain antialiased reconstructions of the original weapon geometry, not newly painted ornate sabres. This pass verifies rendering coverage; full campaign playthrough and every possible combat interaction are not certified. User's custom intro music and cinematic title background remain installed.

Final follow-up: shared KID fall poses used by Shadow receive a scoped dark tint; mirroring retains that flag and normal hero draws restore their original colours. Covered by normal-colour isolation tests for both directions. Final renderer fixtures exclude native 1x1 placeholder images. Final pose log: build/hd-grips-pose-final.log. Last compositor changes checked in build/hd-grips-release-check.log; Shadow engine sheets regenerated after the tint fix. Executable SHA256: build/hd-grips-release.sha256.
