# Persiality assets

Created with the built-in image_gen tool, 2026-10-21. Transparent RGBA outputs.
Current runtime: ../hd/PERSIALITY/pose00.png through pose15.png and title.png.
Editable provenance copies: persiality-atlas.png and persiality-title.png.

## Atlas prompt

Use case: stylized-concept. Production 2D game animation sprite atlas for PoPHD. Reference: the existing young prince character, ivory sleeveless tunic and trousers, red sash, brown hair. Create a single transparent PNG atlas, precisely FOUR columns and TWO rows, 8 equal cells with generous transparent margins. Each cell contains ONE full body figure, all facing LEFT in side/three-quarter view, all centered at SAME foot baseline and consistent scale. Row-major animation: 1 prince standing with hands slightly raised and faint blue smoke at feet; 2 prince transforming, blue magical mist rising, arms lifting; 3 full majestic blue-skinned Persian djinn with gold cuffs, crimson sash and ivory trousers, muscular upper body, small dark topknot; 4 same djinn raising both hands; 5 same djinn sweeping arms in a circular spell gesture; 6 same djinn thrusting open palms forward LEFT, turquoise magic around palms; 7 same djinn lowering hands as smoke fades; 8 prince restored in the initial standing stance with residual tiny blue sparkles. No weapons. Hand-painted crisp outlined sprites matching reference game illustration style. ALL eight figures separated, no overlaps, no cropping, no lettering, NO backgrounds or checkerboard, real alpha transparency. Four by two regular grid. Preserve natural anatomy and a readable silhouette.

Reference image: ../hd/KID/res403.png.

## Title prompt

Use case: logo-brand. Game spell finishing move wordmark, exact word 'Persiality' spelled P e r s i a l i t y. ONE large readable word in elegant ornate Persian/Arabian-inspired LATIN lettering, golden amber beveled ink strokes and subtle turquoise magic accents, restrained flourishes and sharp tapered terminals. Dark thin outline for legibility against a blue stone dungeon. Wide horizontal composition, word centered, generous transparent margins, no other text, no backdrop, real transparent alpha. Original fantasy adventure aesthetic, not an imitation of any existing game logo. Spell exactly Persiality.

## Replacement 16-pose atlas (artifact correction)

Built-in image_gen, reference: original persiality-atlas.png (identity only).
Output: persiality-clean-16.png. Runtime exports: ../hd/PERSIALITY/pose00.png
through pose15.png. Packed with tools/pack_persiality.c, which finds empty
row/column gutters, uses one common scale and foot anchor, and adds clear margins.

Exact prompt:
Use case: precise-object-edit. Redraw the reference as a production animation sprite atlas with STRICTLY SIXTEEN completely SEPARATE animation frames in an exact FOUR-column FOUR-row grid. One figure per equal square cell. Reference supplies hero/djinn identity and painterly outlined style ONLY; do NOT reuse its overlapping layout. ALL figures must fit INSIDE the CENTER 60% of their cell width and CENTER 70% of cell height, with GENEROUS fully transparent empty space on EVERY side. No figure, cloth, spark, hand or smoke may touch a cell boundary or another figure. No grid lines. Consistent scale, side view facing LEFT, identical ground baseline and body pivot. Young brown-haired prince: ivory sleeveless shirt, ivory trousers, red sash, brown shoes. Djinn: same trousers, sash and shoes, muscular blue bare torso, gold cuffs, dark topknot. Sixteen chronological frames, read row-major: 1 prince relaxed standing; 2 prince lifts hands slightly; 3 prince palms raised with a little blue magic close to torso; 4 prince half transformed with blue skin appearing; 5 full djinn, hands low; 6 djinn lifts forearms; 7 djinn raises palms to chest; 8 djinn begins slow circular hand motion; 9 djinn elbows bending, circular gesture; 10 djinn palms gather near chest; 11 djinn extends palms halfway toward LEFT; 12 djinn extends both palms toward LEFT, magic impact; 13 djinn draws hands back; 14 djinn lowers hands; 15 half restored prince with small fading blue aura; 16 original prince fully restored, arms relaxed. Anatomically consistent hands and limbs; no duplicate silhouettes within a frame. Magic is SMALL, TRANSLUCENT and CLOSE TO HANDS/BODY, never large clouds spanning cells. No swords, no environment, no text, no checkerboard. Actual transparent alpha background. Prioritize clean isolated complete full-body sprites with generous padding over large figures.
