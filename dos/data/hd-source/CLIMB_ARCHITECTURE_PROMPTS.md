# Climb and dungeon architecture — 2026-10-21

Generated with the built-in image_gen tool. Project source assets are retained below. Packing uses C/SDL crop and resampling only; original data/KID and data/VDUNGEON are unchanged.

## hero-climb-corrected.png
Reference 1: native KID 546–561 laid out as 4x4 equal cells, transparent, 180px silhouette height. Reference 2: hero-full-07.png (identity/style).

Use case: style-transfer. Asset type: 4 by 4 climbing animation sprite sheet for side-view platform game. Image 1 is the STRICT pose/layout target: 16 poses row-major, each in its own equal 256x256 cell. Image 2 is character identity/style reference ONLY. Redraw all 16 poses of image 1 in smooth HD illustrated style of image 2: young brown-haired Persian hero, ivory sleeveless vest and baggy ivory trousers, red waist sash, brown shoes. Preserve EXACT body posture and limb directions of each pixel silhouette in image 1. This is climbing a ledge: first row hangs vertically by raised arms, second row lifts head and chest over ledge with legs still dangling STRAIGHT DOWN, NOT crouching. Third row hips move over ledge with arms down supporting weight and bent knees. Last row rises from crouch. Particularly row2 col3 must retain the long dangling lower leg and row2 col4 the forward horizontal torso with one dangling foot. Do not invent standing/running/boxing poses. All face LEFT. Each figure centered in its cell, entire silhouette, same relative height as image1, at least 25px transparent gap at every cell edge. Transparent background, no ground, no ledge, no shadow, no aura, no ghost contours, no labels, no text. 1024x1024 canvas.

## dungeon-architecture-hd.png
Reference: a diagnostic contact sheet of existing VDUNGEON HD resources 237–240, 286–291, 5 columns x 2 rows, 1000x500. This intermediate is retained for provenance.

Use case: precise-object-edit. Asset type: HD 2D side-view game environment sprite sheet. Input image is the edit target, 1000x500 layout. Redraw ONLY the ten stone architectural sprites in exactly the same positions, sizes and silhouettes. Keep labels and dark gray background untouched. Replace all large square pixel steps and bright white pixel stripes with finely painted smooth blue-gray stone, subtle soft highlights and carved Persian geometric ornament on the four exit-door frame pieces at top left. Columns at top right and bottom row must have smooth sculpted edges and muted continuous reflections, no pure white stripes. Preserve the straight vertical shafts, horizontal joins, L-shaped door jambs, projecting diagonal floor shapes and empty spaces exactly, because these are modular game pieces. Match the existing muted blue dungeon stone. Fine illustrated game rendering, no new objects, no scenery, no glow, no text changes. Keep exact canvas 1000x500 and every sprite's bounding box unchanged.

## dungeon-architecture-cutout.png
Reference: dungeon-architecture-hd.png. Runtime packer ignores label areas and low-alpha fringe.

Use case: background-extraction. Edit this architectural game sprite sheet. Remove ONLY the dark charcoal background and ALL white text labels, replacing them with true transparency. Preserve every blue stone architectural sprite pixel, shape, position, size and color unchanged. No rearrangement. Ten separate pieces stay at exactly the same locations. Do not remove dark shading within the stone. Transparent PNG.

## dungeon-exit-surface.png

Use case: stylized-concept. Asset type: seamless horizontally and vertically repeatable surface texture for a blue-gray stone Persian dungeon exit gate, side-view 2D HD game. Flat orthographic closeup, the entire square is filled edge-to-edge by dark desaturated slate blue stone, subtle fine mineral grain, a repeating small engraved interlocking eight-point Persian star lattice in slightly lighter muted steel blue. Smooth thin rounded continuous carved lines, elegant fine detailed illustration, low contrast, no white, no pixel art, no stair-step shapes, no bevel frame or door silhouette, no perspective, no text, no lighting gradients across the whole image. Exact seamless repeating pattern, matched opposite edges.


## architecture-base/

Unmodified matte floor reference PNGs 237 and 239 copied from the prior alt6
source release, preserving platform facets under the new door ornament.
Straight shaft anchors retain the native coordinates at top/bottom joins.
Both halves sample one continuous strip of the painted shaft with a shared
vertical phase, avoiding a material seam between independently drawn pieces.

2026-10-22 correction: the full per-scanline native column profile is now
interpolated and used through shaft and base. The former local 32px alignment
strip was removed because its end introduced another contour step. This
uses the existing generated sources; no new image generation was performed.

Alt9 packing correction: runtime bases 286/287 are compositions of the
existing painted body 288 with regular matte floor resources 241/242.
The foretable body 288 must match the x=8 native-pixel crop of base 286.
This is deterministic packing of existing artwork, no new generation.

Alt10 rear packing: the round painted pedestal is fitted as a complete centered
body independent of the diagonal platform silhouette. Existing body artwork
is resampled to the rear depth profile; shaft anchor remains 44px wide and
the ordinary floor is preserved below the foot. No new generated image.
