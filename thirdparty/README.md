# Growl third-party dependencies

| Package       | Version   | URL                                     |
|---------------|-----------|-----------------------------------------|
| glm           | `0.9.9.8` | https://github.com/g-truc/glm           |
| fpng          | `bfe5f9c` | https://github.com/richgel999/fpng      |
| json          | `3.10.5`  | https://github.com/nlohmann/json        |
| stb_image     | `v2.27`   | https://github.com/nothings/stb         |
| stb_rect_pack | `v1.0.1`  | https://github.com/nothings/stb         |
| {fmt}         | `8.1.1`   | https://github.com/fmtlib/fmt           |
| HarfBuzz      | `4.2.0`   | https://github.com/harfbuzz/harfbuzz    |
| libunibreak   | `5.0`     | https://github.com/adah1972/libunibreak |
| FreeType      | `2.12.0`  | https://freetype.org/                   |

## Modifications

This section lists modifications to third-party dependencies from their source.

### Harfbuzz

Only `src/` directory is included, with all non-C files removed (e.g. Python,
templates).

### libunibreak

Only the base C files (`unibreakbase.*`, `unibreakdef.*`), plus files needed for
linebreaks (`linebreak.*`, `linebreakdef.*`, `linebreakdata.c`) are included.

Generators, templates, emoji/word/grapheme data are not included.

### FreeType

Only `src/` and `include/` directories, minus the .mk files.
