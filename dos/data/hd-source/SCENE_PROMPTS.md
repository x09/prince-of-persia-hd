# Scene generation record

Built-in imagegen, no CLI/API fallback. References are native PV951, PV981 and TITLE51; princess appearance reference is princess-story.png.

## room.png

Use case: style-transfer. HD background painting for the original Prince of Persia princess-room cutscenes. Repaint image1 in crisp full-colour hand-painted detail, maintaining EXACT original architecture geometry, column centers, arch openings, tile floor horizon and screen framing. Intricate Persian golden carved arches and columns, dark midnight-blue recesses with delicate blue tile frieze, warm terracotta tiled floor. Keep all three foreground column centers and bottoms exactly as reference, because sprites and occlusion overlays depend on these coordinates. Wide 8:5 composition, no crop, camera stationary. No people, bed, hourglass, text, lamps or extra furniture; these are separately drawn by engine. No pixelation, blur or new perspective. Target 1280x800, fully opaque.

## bed.png

Use case: style-transfer. One transparent HD panoramic foreground sprite for Prince of Persia cutscenes. Image1 is exact placement/layout and silhouette reference: a thin horizontal Persian carpet extending most of width, pillow at far left, adult princess reclining in the left third, feet towards right. Image2 defines the princess's consistent face/costume: long chestnut hair, pale lilac gown with magenta-purple lower skirt and gold trim. Draw a low horizontal carpet with a rich blue and gold diamond pattern, gold edging, red cushion at left; princess reclining fully clothed, natural adult proportions, same pose and scale relative to carpet as original. Extremely wide and shallow silhouette, about 6:1 occupied bounding box, centered with transparent padding above/below. Preserve the carpet length and the princess's head position; don't add bed legs, mattress, palace, scenery, ground, shadow or text. Render crisp hand-painted full-colour detail, genuine transparent alpha background. Do not draw the black vertical column occlusion stripe in the original; complete the art behind it.

## title.png

Use case: style-transfer. HD title-screen background painting for Prince of Persia. Repaint reference image with EXACT framing and geometry: decorative red/blue/gold narrow tiled frame at all four edges, enormous ornate pointed golden Persian arch framing a distant golden Arabian palace on a rocky violet canyon, sunset amber sky, blue tiled panels low on either side. Keep upper arch tip at top middle, castle at center distance, rocky path foreground. Rich crisp hand-painted game illustration, fine stone and mosaic details, full colour, no blur or pixelation. No characters, modern objects, lettering, logos or title text; engine overlays credits and game title later. Original 8:5 landscape composition, no crop, entire border visible. Target1280x800 fully opaque.
