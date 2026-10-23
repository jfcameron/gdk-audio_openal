## gdk-audio_openal

An implementation of [gdk-audio](https://github.com/jfcameron/gdk-audio) using OpenAL.

### Quickstart

Install cmake version 3.21 or greater, and a c++20 compatible compiler 
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the demo programs on linux using gcc
cmake --preset linux-gcc-demo

# build the library and demo programs
cmake --build --preset linux-gcc-demo
```
The above is just a demonstration, see `CMakePresets.json` for the full list of supported platforms and compilers. `linux-gcc-demo` builds the demos. The scripts in that file are the same ones used by the CI configurations (`.github/workflows/ci.yml`). They cover Linux, Windows x86 64bit and 32bit, Mac, Android, iOS and emscripten.
