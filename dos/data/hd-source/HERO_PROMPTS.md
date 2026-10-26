# Complete hero animation expansion

Built-in imagegen skill; twelve distinct generated pose sheets. The approved 24 prior drawings are retained. Source files hero-full-00.png through hero-full-11.png. Native pose references: build/hero-ref-00.png through hero-ref-11.png. Character reference: data/hd-preview/run-source.png. Source resource-to-cell mapping: hero-map.txt (only groups 0..11 are packed; group 12 contains unrelated health/damage symbols and is excluded).

## Shared exact prompt

Use case: style-transfer. Asset type: production 2D sprite sheet for Prince of Persia Linux HD.
Image 1 is an EXACT POSE AND CELL-LAYOUT reference, 4 equal columns by 4 equal rows, read left-to-right top-to-bottom. Image 2 is CHARACTER IDENTITY AND RENDERING STYLE reference only.
Redraw EVERY individual pose from image 1 as the SAME detailed young male hero from image 2: tousled brown hair, ivory sleeveless shirt, ivory loose trousers, crimson waist sash with short tails, brown soft shoes, clean hand-painted contours and full-colour shaded fabric. Preserve precisely the facing direction, limb bends, torso lean, foreshortening and each pose's silhouette from image 1. Some poses turn away or face the opposite direction; keep that. Do not replace them all with a running pose. Match the physical size of each figure relative to its cell in image 1. Keep cell centers aligned. Exactly sixteen separated figures in a perfectly regular 4x4 square grid, no border lines or text. Every figure fully inside its own cell with wide transparent gutter. Orthographic side-view game art. Background completely transparent alpha, no ground, shadow, glow, checkerboard, scenery, captions, numbers or watermark. Smooth crisp contours, no pixel blocks and no blurry halo. Natural anatomy, consistent costume proportions. Output square 1536x1536 if possible.

## Sheet 00 appended text

Sheet notes: Sequence of bending forward, starting a broad jump, low landing then recovery; preserve all sixteen slightly different poses.

## Sheet 01 appended text

Sheet notes: Sequence of landing/recovery, jump extension, then turning through front-facing to opposite-side profile.

## Sheet 02 appended text

Sheet notes: Sequence includes back-facing frames and walking away. Cells row3 col4 and row4 col1/col2 are heavily occluded/headless upper-body-cropped transit frames: preserve the visible cropped torso/legs only, do NOT invent a head outside its original visible crop.

## Sheet 03 appended text

Sheet notes: Sequence includes recoil and falling. Row3 col4 is a side-lying fallen hero, row4 col1 kneeling collapsed, row4 col2 prone. Keep blood modest and non-graphic, match pose.

## Sheet 04 appended text

Sheet notes: Sequence of crouch preparation, vertical jump, arms reaching straight overhead, side profile suspended.

## Sheet 05 appended text

Sheet notes: Sequence of ledge hanging with BOTH hands overhead; bent/swinging legs and transition to low crouch.

## Sheet 06 appended text

Sheet notes: Cells row3 col3/col4 and row4 col1 are NOT the hero: they are tiny pale mice in successive running poses. Draw tiny detailed ivory mice with thin pink tails, preserve original tiny scale. All other cells are hero crouch/recovery/standing.

## Sheet 07 appended text

Sheet notes: Sequence of cautious steps, ledge reaching, rising and leaning forward. Preserve facing direction.

## Sheet 08 appended text

Sheet notes: Sequence of crouched recovery followed by standing with arms extended horizontally forward, exact hand positions.

## Sheet 09 appended text

Sheet notes: Sequence of drawing a sword, guarded combat steps and lunges. Body poses exact. Preserve original only parts of sword visible in pose reference; long sword blade may be rendered separately by engine.

## Sheet 10 appended text

Sheet notes: Sequence of combat footwork followed by picking up a small potion and raising it toward mouth.

## Sheet 11 appended text

Sheet notes: Sequence of drinking potion, leaning back, sheathing/drawing sword, falling prone in last cell.

## Technical packing

C/SDL2 connected-component extraction at alpha >=192, discard isolated components smaller than 1% of the main figure, alpha-aware area resampling to original resource bounds at 4x with one-pixel transparent border. No native upscale or repeated nearest pose is substituted. 192 new resources = 189 hero poses + 3 mouse frames; total complete animation set 213 hero poses + 3 mouse frames.
