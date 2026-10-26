# Dungeon stone correction — 2026-10-21

Generated with the built-in image_gen tool, editing/reference input materials-soft.png.
Output: dungeon-stone-matte.png. Original source atlas is retained unchanged.

Prompt:
Use case: precise-object-edit. Game material texture correction. Reference image is a two-material atlas: blue gray dungeon stone on left, golden sandstone on right. Produce a square image showing ONLY the corrected BLUE GRAY STONE MATERIAL covering the entire image. Match the left half's muted slate-blue color and fine matte granular stone grain. REMOVE ALL pale scratches, thin white strokes, diagonal lines, bands, vertical streaks, veins, fissures and recognizable repeating marks. Uniform low contrast isotropic fine stone grain only, subtle pores, even diffuse illumination, same brightness everywhere, no gradients, no shadows, no edges, no blocks, no brick seams. Intended as seamless repeatable small texture for game wall blocks; unobtrusive natural matte stone surface. No text or other elements.

The packer uses the whole material rather than the former repeating 210x220 crop.
Fine grain has 40% of its original contrast around the mean material color;
original DOS brightness, resource dimensions and alpha masks are retained.

Repack only dungeon structural material:
    make hd-dungeon-material

The targeted pass preserves painted bones and the precision props generated
by finish_hd_assets.c. Full make hd-assets also uses this material.
