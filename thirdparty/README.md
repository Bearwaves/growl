# Growl third-party dependencies

| Package       | Version    | URL                                     |
|---------------|------------|-----------------------------------------|
| {fmt}         | `8.1.1`    | https://github.com/fmtlib/fmt           |
| fpng          | `bfe5f9c`  | https://github.com/richgel999/fpng      |
| FreeType      | `2.12.0`   | https://freetype.org/                   |
| glm           | `0.9.9.8`  | https://github.com/g-truc/glm           |
| HarfBuzz      | `4.2.0`    | https://github.com/harfbuzz/harfbuzz    |
| json          | `3.10.5`   | https://github.com/nlohmann/json        |
| libpng        | `1.6.37`   | http://libpng.org/pub/png/libpng.html   |
| libunibreak   | `5.0`      | https://github.com/adah1972/libunibreak |
| msdfgen       | `1.9.2`    | https://github.com/Chlumsky/msdfgen/    |
| SoLoud        | `20200207` | https://github.com/jarikomppa/soloud    |
| stb_image     | `v2.27`    | https://github.com/nothings/stb         |
| stb_rect_pack | `v1.0.1`   | https://github.com/nothings/stb         |
| UTF8-CPP      | `3.2.1`    | https://github.com/nemtrif/utfcpp       |
| zlib          | `1.2.12`   | https://www.zlib.net/                   |

## Modifications

This section lists modifications to third-party dependencies from their source.

### FreeType

Included only `src/` and `include/` directories, minus the `tools/` and `dlg/`
directories and `.mk` files.

### Harfbuzz

Only `src/` directory is included, with all non-C files removed (e.g. Python,
templates).

### libpng

Included all `.c` and `.h` files, except `example.c` and `pngtest.c`. Used
`scripts/pnglibconf.h.prebuilt` as `pnglibconf.h`.

### libunibreak

Only the base C files (`unibreakbase.*`, `unibreakdef.*`), plus files needed for
linebreaks (`linebreak.*`, `linebreakdef.*`, `linebreakdata.c`) are included.

Generators, templates, emoji/word/grapheme data are not included.

### msdfgen

Included `core/`, `ext/`, `include/` and `lib/`, along with top-level headers
and license.

### SoLoud

Included `src/` (with exception of `src/c_api/`), `include/`, and license.

Added custom CMakeLists.txt.

### UTF8-CPP

Included only `source/` and `CMakeLists.txt`, along with license and README.

### zlib

Included all `.c` and `.h` files from the root.
