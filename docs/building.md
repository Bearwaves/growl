# Building Growl

Growl has out-of-the-box support for building on a variety of platforms. This
guide will detail the necessary steps and tools for each, as well as some
general advice on including Growl as a dependency for your project.

## Including Growl as a dependency

Growl is built using CMake, and thus can be included in your project using the
mechanisms which CMake supports. Please note that Growl currently does **not**
include an installation target - if this is something you would like, please
raise an issue for it.

### Submodule

Perhaps the simplest way to include Growl is to add it as a Git submodule. Add
it to your repository like so:

```bash
git submodule add git@github.com:Bearwaves/growl.git
git submodule update --init --recursive
```

Then include it via your project's top-level `CMakeLists.txt`:

```cmake
add_subdirectory(growl)
```

This is the approach taken by the [Growl template](
https://github.com/Bearwaves/growl-template).

### FetchContent

If you'd like a pure CMake solution, the more 'modern' way to include a CMake
module from a remote source is via `FetchContent`. You can set this up in your
`CMakeLists.txt` like so:

```cmake
include(FetchContent)

FetchContent_Declare(
	growl
	GIT_REPOSITORY git@github.com:Bearwaves/growl.git
	GIT_TAG main
	)
FetchContent_MakeAvailable(growl)
```

Because `FetchContent` doesn't output much, it may look like the CMake process
has frozen and isn't doing anything; in reality it's just fetching the various
Git submodules from within Growl itself, the speed of which will vary based on
your internet connection.

You may wish to pin a specific commit (rather than `main`) so that CMake
doesn't fetch the new version of Growl on every update.

You can read more about `FetchContent` in the [CMake documentation](
https://cmake.org/cmake/help/latest/module/FetchContent.html).

### Relative path

Those who already have Growl on their machine may wish to build against it
directly without messing around with submodules or symlinks.

The approach used by the example app within the Growl repo uses this pattern:

```cmake
set(GROWL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../growl)
add_subdirectory(${GROWL_PATH} ${CMAKE_BINARY_DIR}/growl)
```

This points to a Growl repo relative to your current source directory (that is,
where your `CMakeLists.txt` is).

## Windows, macOS & Linux

## iOS

## Android

## Web

## Custom thirdparty dependencies
