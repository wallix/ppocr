kContiens les fichiers de police avec un dossier par locale.

Le fichier `letters` contient les lettres présente dans les images du dossier 'generated'.

Le dossier `mano` contient les dossiers de l'ancien OCR, s'y référer pour plus de précision.

Le dossier 'generated_extract_fail' contient les polices générée qui sont mal reconnu par l'ocr du fait des césures
(une lettre est alors reconnue comme 2 lettres).
Ce dossier est ignoré.

Note: les polices sont générées à partir de font2png (src/font2png.cpp) puis transformer en image pnm avec pngtopnm.

## resources/dict_*.trie

Généré avec 'make_trie_dictionary' et les fichiers 'dict_*.txt' contenant un mot par ligne

## resources/{glyphs,datas,*.txt}

Générés avec 'learning' et les fichiers dans 'resources/fonts'.

'learning' prend en paramètre 3 executables construit via 'bjam'.

$ './learning ./bin/${compiler}/${target}/write_glyphs ./bin/${compiler}/${target}/normalize_glyphs ./bin/${compiler}${target}/write_datas

Note: une erreur ('error: bad pnm type; expected P3 or P6, get P4!') est systématiquement affichée.
Celle-ci provient des sources de Milena (include/mln) qui affiche une erreur quand le fichier ne peut être lu.
Comme ppocr essaye 2 formats (pnm et pbm), si la première lecture échoue, l'erreur apparaît.
