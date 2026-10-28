// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/audio/context.h>
#include <gdk/audio/openal_context.h>
#include <gdk/audio/scene.h>
#include <gdk/audio/emitter.h>
#include <gdk/audio/sound.h>

#include <gdk/audio/impl_openal_simple_emitter.h>
#include <gdk/audio/impl_openal_stream_emitter.h>

#include "tones.h"

#include <limits>
#include <memory>

using namespace gdk;
using namespace gdk::audio;

namespace {
    struct fixture final {
        context_shared_ptr_type pContext = openal_context::make();
        scene_shared_ptr_type pScene = pContext->make_scene();

        [[nodiscard]] std::shared_ptr<emitter> emitter_for(const sound_shared_ptr_type &apSound) {
            return pScene->make_emitter(apSound);
        }
    };
}

TEST_CASE("clip length selects the emitter implementation", "[emitter]") {
    fixture f;

    SECTION("a clip under five seconds is fully decoded") {
        auto pEmitter = f.emitter_for(short_tone());

        REQUIRE(std::dynamic_pointer_cast<openal_simple_emitter>(pEmitter) != nullptr);
        REQUIRE(std::dynamic_pointer_cast<openal_stream_emitter>(pEmitter) == nullptr);
    }

    SECTION("a clip over five seconds is streamed") {
        auto pEmitter = f.emitter_for(long_tone());

        REQUIRE(std::dynamic_pointer_cast<openal_stream_emitter>(pEmitter) != nullptr);
        REQUIRE(std::dynamic_pointer_cast<openal_simple_emitter>(pEmitter) == nullptr);
    }
}
