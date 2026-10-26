# HD prototype art prompts

Built-in imagegen, 2026-10-19. Generated artwork based on local DOS room and eight running poses 7–14.

## Room

Use case: style-transfer. Asset type: production background for one HD room in a 2D side scrolling Prince of Persia DOS remake. The input screenshot is the EXACT geometric layout reference. Redraw it at 1280x800 with rich full-color hand-painted stone textures, crisp inked contours and carefully controlled cel shading, clean high resolution illustrated game art, NOT pixel art, NO blurry filter. Lock every platform top, wall outline, ledge, opening, arch and floor position to the reference: multiply coordinates by four. Dungeon masonry blue slate and warm sandy accents, subtle torch illumination, dark background recesses, readable foreground stone. Keep torch bracket locations. Remove the small prince in upper-left entirely; rebuild the wall behind him. Remove the bottom black HUD band and all text/hearts; extend the lower floor only into this last small band. No characters, no new props, no numbers, no interface, no watermark, no fog, no depth of field. Preserve the black open spaces as dark readable recesses. IMPORTANT this is a functional 2D platformer room, do not add scenic perspective or move geometry.

## Run cycle

Use case: style-transfer. Asset type: production HD run-cycle sprite atlas for a C/SDL2 Prince of Persia DOS remake. The reference is an exact pose sheet: 4 columns x 2 rows, eight sequential running poses 7-14, all facing LEFT. Redraw every individual figure in the identical cell, preserving the precise limbs and silhouettes from its corresponding reference frame. Newly drawn detailed full-color illustrations, NOT pixel art and not upscaled pixels. Clean crisp contour lines and controlled cel shading, slight fabric folds, same athletic young adult man with short brown hair, ivory sleeveless tunic, ivory loose trousers, narrow crimson sash and small brown soft leather shoes in all eight frames. No weapon. Produce exactly 1536x1024, FOUR columns TWO rows with equal 384x512 cells; no extra figures. Torso center around x=192 per cell, same human scale, about 360px body height, bottom of lowest shoe at y=456 within each cell, 24px minimum empty margin. Genuine transparent alpha everywhere outside figures. No background, no checkerboard pattern, no labels, no borders, no text, no ground shadow. Crucial: preserve eight distinct poses in input order and orientation, keep hair, face, clothes, anatomy and lighting consistent across the loop. First and fifth poses swap leading leg, creating a coherent complete running cycle.

## Alpha extraction

Remove only the brown backdrop, preserving all eight figures, positions and canvas dimensions. Genuine transparent alpha. No checkerboard, no cast shadow.

## Technical integration

Source PNGs are retained as room.png and run-source.png. C/SDL2 tool tools/pack_hd_run.c extracts eight alpha-connected figures, registers heads and feet, and writes run.png. This step packs the generated art; it does not invent character details.
