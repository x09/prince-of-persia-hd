# Guard generation record
Tool: built-in imagegen. Outputs guard-0.png, guard-1.png, guard-2.png.
Native reference: tools/guard_refs.c -> build/guard-ref-{0,1,2}.png.
Style reference: data/hd-preview/run-source.png (approved hero illustration).

Shared prompt:
Use case: style-transfer. Asset: production HD animation sprites for the existing Prince of Persia Linux game.
Reference image 1 gives EXACT poses and the fixed grid positions. Reference image 2 gives ONLY the hand-painted rendering style and anatomical proportions; do NOT draw the white-clothed hero.
Draw ONE CONSISTENT ordinary palace guard in every populated cell: fit adult male, medium tan skin, short dark beard, burgundy cloth turban, deep plum purple sleeveless tunic with restrained gold edging, lavender loose trousers, brown belt and soft leather boots. Clean detailed face, crisp full-colour fabric shading, matching the existing HD hero's illustrative style. Orthographic side view, same body height and proportions across cells, natural anatomy.
Match every native pose's facing direction, limb bends, foot stance, reach and hand position precisely. Hands are closed grips but EMPTY: DO NOT draw any sword, blade, shield, scabbard or carried weapon, because the game draws and positions the sword as a separate moving sprite.
Keep every figure fully inside its own cell with wide TRANSPARENT gutters. Genuine transparent alpha background, no scenery, ground, shadow, glow, outlines around cells, text, numbering, watermark or checkerboard. No pixelation, no blur. Maintain all distinct subtle poses, no generic pose repeated as a substitute. Preserve the reference cell order exactly.

Sheet 0:
Square 4 columns by 4 rows. Top row columns 1 and 2 are tiny UI effects, NOT people: leave those TWO CELLS EMPTY AND TRANSPARENT. All other fourteen cells contain the guard poses shown. Output 1536x1536.
Sheet 1:
Square 4 columns by 4 rows. Row 3 column 2 is an EMPTY unused frame: LEAVE IT EMPTY AND TRANSPARENT. Other fifteen cells contain the corresponding guard poses. Row3 col3 is a collapsed kneeling guard; row3 col4 is fallen on his side. Non-graphic defeat, no added gore. Output 1536x1536.
Sheet 2:
Two equal columns and ONE row only. Exactly TWO fallen/defeated side-lying guard poses matching the two reference silhouettes, same costume and identity. Non-graphic, no added gore. Wide transparent padding. Landscape output.

Actual returned sizes: 1254x1254,1254x1254,1774x887.
Packing uses connected alpha components per cell and isotropic fitting into
padded native bounds. See guard-manifest.txt for exact boxes and output sizes.
