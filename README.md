# gdk-audio_openal
The [gdk-audio](https://github.com/jfcameron/gdk-audio) interface, implemented over OpenAL: OpenAL Soft on Linux, Windows, macOS, FreeBSD, Android and iOS, and Emscripten's OpenAL over Web Audio in the browser.

API documentation: https://jfcameron.github.io/gdk-audio_openal/

## Getting Started
Install cmake 3.21 or greater, and a c++20 compatible compiler. The system's OpenAL Soft is used where it has one; otherwise the vendored OpenAL Soft is built and linked statically (`GDKAUDIO_OPENAL_VENDORED_OPENAL` forces it). On Linux, install the ALSA and PulseAudio development headers so the vendored one can reach them (on Debian and Ubuntu: `libasound2-dev libpulse-dev`).
```
# initialize the tree
git submodule update --init --recursive

# generate the script to build the library, its tests and the demos on linux using gcc
cmake --preset linux-gcc

# build them
cmake --build --preset linux-gcc

# run the tests, on OpenAL Soft's null driver
ctest --preset linux-gcc
```
See `CMakePresets.json` for the full list of supported platforms and compilers. See `.github/workflows/ci.yml` to see how CI runners build it on different platforms.

To use it from another project, add it with `add_subdirectory` and link `gdk::audio_openal`. It brings gdk-audio and, where needed, OpenAL Soft with it. Or install it (`cmake --install`) and use `find_package(gdk-audio_openal)`.

## Usage
```cpp
#include <gdk/audio/openal_context.h>
#include <gdk/audio/openal_emitter.h>

using namespace gdk::audio;

const auto pAudio = openal_context::make({.distance_model = distance_model::inverse_clamped});
const auto pScene = pAudio->make_openal_scene();

pScene->set_reverb({});

const auto pEmitter = pScene->make_emitter(pSound);

if (auto *const pOpenAL = dynamic_cast<openal_emitter *>(pEmitter.get())) pOpenAL->set_lowpass({1, 0.2f});

pEmitter->play();

// each frame
pScene->update();
```

OpenAL makes one context current per process, so there is one `openal_context` at a time: a second throws while the first's device is open. The device stays open while anything made from the context lives, its scenes and emitters included, so they may outlive the handle to it. None of it is thread-safe.

A sound shorter than `openal_policy::streaming_threshold_in_seconds` is decoded up front; a longer one, or one played through insert effects or from a place per channel, is streamed. `openal_scene` adds a shared reverb or echo to a scene and `openal_emitter` a lowpass to an emitter, through OpenAL's EFX extension, where the OpenAL has it.

## Why OpenAL specifically?

OpenAL libraries are widely available across platforms: virtually all desktop operating systems, mobile devices, even in browsers via emscripten. The only real disadvantage I've encountered is the inability to modify a sound after spatialization but before playback, which prevents you from implementing effects that require it (eg an accurate echo effect). Spatialized effects can be provided via extensions, but as extensions, their availability varies from platform to platform, and the effects are specific, there is no general way to provide your own.
