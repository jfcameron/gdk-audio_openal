## gdk-audio_openal

An implementation of [gdk-audio](https://github.com/jfcameron/gdk-audio) using OpenAL.

## OpenAL

`extern/OpenAL.cmake` picks one, unless the host has already defined an `OpenAL` target:

- **emscripten's**, over Web Audio, in the browser;
- **the system's**, through `find_package(OpenAL CONFIG)`;
- **openal-soft**, built from `extern/OpenAL` and linked statically, where the system has none or
  `GDK_AUDIO_VENDORED_OPENAL` asks for it.

## Building

```
cmake --preset linux-clang
cmake --build --preset linux-clang
ctest --preset linux-clang
```

`CMakePresets.json` lists the supported builds. `linux-gcc-demo` builds the demos, and `emscripten` builds the library
for the browser (`emcmake cmake --preset emscripten`).

