// © Joseph Cameron - MIT License

#include <gdk/audio/emitter.h>
#include <gdk/audio/openal_context.h>
#include <gdk/audio/openal_emitter.h>

#include <cstdint>
#include <memory>

int main() {
    using namespace gdk::audio;

    const std::int16_t silence[800] = {};

    const auto pScene = openal_context::make()->make_openal_scene();
    const auto pEmitter = pScene->make_emitter(make_pcm_sound(silence, 800, 1, 8000, pcm_format::signed_16));

    pEmitter->play();
    pScene->update();

    return std::dynamic_pointer_cast<openal_emitter>(pEmitter) && pEmitter->is_playing() ? 0 : 1;
}
