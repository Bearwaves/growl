# Growl third-party dependencies

| Package       | Version    | URL                                     |
|---------------|------------|-----------------------------------------|
| Dear ImGui    | `2685650`  | https://github.com/ocornut/imgui        |
| {fmt}         | `9.0.0`    | https://github.com/fmtlib/fmt           |
| fpng          | `b5267e5`  | https://github.com/richgel999/fpng      |
| FreeType      | `2.12.1`   | https://freetype.org/                   |
| glm           | `2993560`  | https://github.com/g-truc/glm           |
| HarfBuzz      | `3377ddf`  | https://github.com/harfbuzz/harfbuzz    |
| json          | `3.11.3`   | https://github.com/nlohmann/json        |
| libpng        | `1.6.37`   | http://libpng.org/pub/png/libpng.html   |
| libunibreak   | `5.0`      | https://github.com/adah1972/libunibreak |
| lua           | `v5.4.6`   | https://github.com/lua/lua              |
| msdfgen       | `1.9.2`    | https://github.com/Chlumsky/msdfgen/    |
| SoLoud        | `1157475`  | https://github.com/jarikomppa/soloud    |
| stb_image     | `v2.27`    | https://github.com/nothings/stb         |
| stb_rect_pack | `v1.0.1`   | https://github.com/nothings/stb         |
| UTF8-CPP      | `4.0.4`    | https://github.com/nemtrif/utfcpp       |
| zlib          | `1.2.12`   | https://www.zlib.net/                   |

Dependencies are submodules, except in cases where that doesn't make sense.
The Growl root CMakeLists.txt ensures a submodule fetch happens at configure
time, so you can use Growl via FetchContent or similar.

Some things are built using their own CMake config. In other cases, where the
included config is problematic, Growl builds the dependency into its own module.

One need only link against `growl-thirdparty` to link against all dependencies,
or `growl-thirdparty::[name]` can be used to select a specific one.
