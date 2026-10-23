# gdk-audio_openal
An implementation of [gdk-audio](https://github.com/jfcameron/gdk-audio) using OpenAL.

## Getting Started
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

## Why OpenAL specifically?

OpenAL libraries are widely available across platforms: virtually all desktop operating systems, mobile devices, even in browsers view emscripten. The only real disadvantage I've encountered is the inability to modify a sound after spatialization but before playback, which prevents you from implementing effects that require it (eg an accurate echo effect). spatialized effects can be provided via extensions, but as extensions, their availablility varies from platform to platform, and the effects are specific, there is no general way to provide your own.  
