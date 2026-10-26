# Character generation record
Built-in imagegen; no CLI/API fallback. Native pose atlases are made by
tools/character_refs.c; native resource IDs and grids: tools/hd_characters.h.
Second style reference: data/hd-preview/run-source.png (approved hero).
All source atlases below are preserved beside this record.

Shared prompt:
Use case: style-transfer. Production transparent HD animation sprite atlas for Prince of Persia Linux. Image 1 is the EXACT pose and cell-order reference; image 2 gives ONLY hand-painted full-colour illustrative style and human proportions. Preserve the EXACT grid, cell order, facing direction, arm and leg bends, hand reach, pose silhouette and foot stance in EVERY populated cell. Do not substitute a repeated generic pose. Consistent identity, costume, anatomy and shading throughout. Orthographic side-view as the reference, NO perspective floor. Crisp detailed painted surfaces, no pixelation or blur. Genuine transparent alpha background and wide empty gutters; no scenery, ground, cast shadow, text, labels, lines, checkerboard, watermark or border. Do not draw any weapon in empty hands: the engine draws swords separately. Leave all empty reference cells completely transparent. Each figure entirely within its own cell. Output high-resolution atlas, target 3072 pixels wide.

Subject and atlas additions:
- skel.png: animated human skeleton, aged ivory bone, warm beige shading,
  clear skull/ribcage/spine/pelvis, articulated bony limbs/fingers.
  No clothes, armour, flesh, glow or weapons. Six columns/five rows,
  26 slots; row4 column6 blank. Row5 columns1/2 crouching/rising.
- fat.png: stout heavy palace swordsman, round belly/broad shoulders, tan skin,
  black beard/moustache, white turban, pale lilac sleeveless overcoat, plum belt,
  royal-blue trousers/undersleeves, brown shoes. Six columns/six rows,
  first32 slots; native placeholders blank, last two figures non-graphic defeat.
- vizier.png: tall slender mature Jaffar, angular tan face, black pointed beard
  and moustache, purple turban, pale lilac cape over crimson tunic/trousers,
  purple sash/gold slippers. Six columns/six rows, first32 slots, native
  placeholders blank; last two figures non-graphic defeat.
- shadow.png: athletic young prince's dark double, same build/face as hero,
  short brown hair, charcoal sleeveless tunic/trousers, smoky violet shading,
  dark red sash. Six columns/five rows, original pose order, empty native
  cells transparent. The mistakenly blank 776 was supplied separately.
- princess-story.png: adult Persian princess, long chestnut hair, warm light
  olive skin, pale lilac sleeveless ankle-length gown with magenta-purple
  lower skirt, restrained gold trim/bracelet, barefoot, fully clothed.
  Five columns/four rows, first17 cells populated. Turning left to back,
  running right, walking/stopping right, matching native references.
- jaffar-story.png: same Jaffar identity/costume as vizier. Six columns/six
  rows,32 poses: walking left, turning through back, walking right,
  gesturing/raising arm. Exact cape positions, no weapon/staff.
- jaffar-end.png: same Jaffar, three columns/two rows, six left-facing
  gesturing poses. Preserve raised/lowered arms, no weapon/staff.
- princess-cut.png: same adult princess, six columns/five rows,30 slots;
  row1 column1 reclines, column2 blank. Running/reaching, then TWO people
  in row2 columns5/6 and row3 columns1–4: princess and white-clothed adult
  hero with red sash reach/embrace fully clothed. Remaining cells seated
  princess gestures/head turns. Preserve pairs together in their cells.
- shadow-fall.png: ONE recoiling/backwards falling dark prince, head/arched
  torso towards left, legs down/right, arms trailing left, exact native776
  pose. Reference identity: shadow.png. No weapon/scene/shadow/glow/text.

Extraction removes tiny disconnected alpha components, retains sizeable
parts belonging to the same cell, and preserves figure proportions.
Native blank frames are detected in RGBA, including indexed source PNGs.
The source atlases are new paintings; packing/resampling uses C/SDL2.

Correction for bed-empty.png (imagegen edit of bed.png):
REMOVE THE WOMAN COMPLETELY. Preserve ONLY the long shallow Persian carpet
and red/gold pillow at far left; reconstruct matching blue/gold diamond rug
under the removed body, same perspective, silhouette, woven texture and
gold edging. Genuine transparent alpha, no people or furniture.
The princess is always a separate animated game sprite.
