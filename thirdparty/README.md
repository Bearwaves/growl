# Growl third-party dependencies

| Package       | Version    | URL                                     |
|---------------|------------|-----------------------------------------|
| Dear ImGui    | `2685650`  | https://github.com/ocornut/imgui        |
| {fmt}         | `9.0.0`    | https://github.com/fmtlib/fmt           |
| fpng          | `b5267e5`  | https://github.com/richgel999/fpng      |
| FreeType      | `2.12.1`   | https://freetype.org/                   |
| glm           | `0.9.9.8`  | https://github.com/g-truc/glm           |
| HarfBuzz      | `5.1.0`    | https://github.com/harfbuzz/harfbuzz    |
| json          | `3.10.5`   | https://github.com/nlohmann/json        |
| libpng        | `1.6.37`   | http://libpng.org/pub/png/libpng.html   |
| libunibreak   | `5.0`      | https://github.com/adah1972/libunibreak |
| msdfgen       | `1.9.2`    | https://github.com/Chlumsky/msdfgen/    |
| SoLoud        | `master`   | https://github.com/jarikomppa/soloud    |
| stb_image     | `v2.27`    | https://github.com/nothings/stb         |
| stb_rect_pack | `v1.0.1`   | https://github.com/nothings/stb         |
| UTF8-CPP      | `3.2.1`    | https://github.com/nemtrif/utfcpp       |
| zlib          | `1.2.12`   | https://www.zlib.net/                   |

Dependencies are submodules, except in cases where that doesn't make sense.
The Growl root CMakeLists.txt ensures a submodule fetch happens at configure
time, so you can use Growl via FetchContent or similar.

Some things are built using their own CMake config. In other cases, where the
included config is problematic, Growl builds the dependency as part of
the growl-thirdparty target.
