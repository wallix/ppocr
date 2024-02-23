LGPLv2.1

# Learning (./learning)

Generate `glyphs/*` and `datas/*` with

$ `./learning "$bindir"/write_glyphs "$bindir"/normalize_glyphs "$bindir"/write_datas`

The executables are compiled with `bjam`. `bindir` depends on the compiler used.

$ `bjam` or $ `bjam write_glyphs normalize_glyphs write_datas`

Note: an error ("error: bad pnm type; expected P3 or P6, get P4!") is always displayed.
It comes from Milena sources (include / mln), which displays an error when the file can not be read.
As ppocr trying 2 formats (PBM and PNM), if the first reading fails, the error appears.


# resources/fonts/{latin,cyrillic}/


## ./letters and ./generated/

`letters` contains glyphs of generated/*/*.pnm (generated by font2png (font2png.cpp) then converted to pnm with pngtopnm (imagemagick))


## resources/fonts/generated_extract_fail/

Images generated incorrectly (font with hyphens). The directory is ignored.


## ./mano/

Images belonging to the old ocr (OCRv1, see redemption project).


## resources/dict_*.trie

Generated from `resources/dict_*.txt` by `make_trie_dictionary`


## resources/glyphs/*

Generated from image files (font/*) by `write_glyphs_bin` and `normalizer_glyphs_bin`


## resources/datas/*

Generated from data glyphs (glyphs/*) by `write_datas`
