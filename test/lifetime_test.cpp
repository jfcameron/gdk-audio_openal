// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/audio/exception.h>
#include <gdk/audio/openal_context.h>
#include <gdk/audio/emitter.h>
#include <gdk/audio/scene.h>

#include "tones.h"

#include <AL/al.h>
#include <AL/alc.h>

#include <memory>

using namespace gdk::audio;

TEST_CASE("a scene and its emitters keep the device open after the context goes", "[openal][lifetime]") {
    auto pContext = openal_context::make();
    auto pScene = pContext->make_scene();
    auto pEmitter = pScene->make_emitter(short_tone());

    pContext.reset();

    REQUIRE(alcGetCurrentContext() != nullptr);

    pEmitter->play();
    pScene->update();

    REQUIRE(pEmitter->is_playing());
    REQUIRE(alGetError() == AL_NO_ERROR);

    SECTION("a second context waits for them to go") {
        REQUIRE_THROWS_AS(openal_context::make(), exception);

        pEmitter.reset();
        pScene.reset();

        REQUIRE(alcGetCurrentContext() == nullptr);
        REQUIRE_NOTHROW(openal_context::make());
    }
}
