// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>

#include <gdk/audio/exception.h>
#include <gdk/audio/context.h>
#include <gdk/audio/openal_context.h>
#include <gdk/audio/openal_policy.h>
#include <gdk/audio/openal_simple_emitter.h>
#include <gdk/audio/openal_stream_emitter.h>
#include <gdk/audio/emitter.h>
#include <gdk/audio/scene.h>
#include <gdk/audio/sound.h>

#include "tones.h"

#include <AL/al.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>
#include <span>

using namespace gdk;
using namespace gdk::audio;

namespace {
    [[nodiscard]] context_unique_ptr_type make_context() {
        return openal_context::make();
    }
}

TEST_CASE("context creation", "[context]") {
    SECTION("a context can be created") {
        REQUIRE(make_context() != nullptr);
    }
}

TEST_CASE("a second live context is refused", "[context]") {
    auto pFirst = make_context();

    REQUIRE(pFirst != nullptr);
    REQUIRE_THROWS_AS(make_context(), exception);
}

TEST_CASE("the refusal is about overlap, not about counting", "[context]") {
    for (int i = 0; i < 3; ++i) {
        auto pContext = make_context();

        REQUIRE(pContext != nullptr);
    }
}

TEST_CASE("the test tones straddle the streaming threshold", "[context][sound]") {
    const openal_policy defaults;

    REQUIRE(short_tone()->duration() < defaults.STREAMING_THRESHOLD_IN_SECONDS);
    REQUIRE(long_tone()->duration() > defaults.STREAMING_THRESHOLD_IN_SECONDS);
}

TEST_CASE("the context policy is honoured", "[context][policy]") {
    SECTION("its defaults are usable without saying anything") {
        auto pContext = openal_context::make();

        REQUIRE(pContext != nullptr);
        REQUIRE(pContext->make_scene() != nullptr);
    }

    SECTION("the distance model reaches the device") {
        auto pContext = openal_context::make(openal_policy{distance_model::exponent_clamped});

        REQUIRE(alGetInteger(AL_DISTANCE_MODEL) == AL_EXPONENT_DISTANCE_CLAMPED);
    }

    SECTION("switching the model off reaches it too") {
        auto pContext = openal_context::make(openal_policy{distance_model::none});

        REQUIRE(alGetInteger(AL_DISTANCE_MODEL) == AL_NONE);
    }

    SECTION("doppler factor and speed of sound reach the device") {
        auto pContext = openal_context::make(
            openal_policy{.DISTANCE_MODEL = distance_model::linear_clamped,
                .DOPPLER_FACTOR = 2.5f, .SPEED_OF_SOUND = 500.0f});

        REQUIRE(alGetFloat(AL_DOPPLER_FACTOR) == Approx(2.5f));
        REQUIRE(alGetFloat(AL_SPEED_OF_SOUND) == Approx(500.0f));
    }

    SECTION("the streaming threshold decides the emitter implementation") {
        auto pContext = openal_context::make(
            openal_policy{.DISTANCE_MODEL = distance_model::linear_clamped,
                .DOPPLER_FACTOR = 1, .SPEED_OF_SOUND = 343.3f,
                .STREAMING_THRESHOLD_IN_SECONDS = 0.5f});

        auto pScene = pContext->make_scene();

        auto pEmitter = pScene->make_emitter(short_tone());

        REQUIRE(std::dynamic_pointer_cast<openal_stream_emitter>(pEmitter) != nullptr);
    }

    SECTION("a scene made from the context carries its policy") {
        auto pContext = openal_context::make(
            openal_policy{.DISTANCE_MODEL = distance_model::linear_clamped,
                .DOPPLER_FACTOR = 1, .SPEED_OF_SOUND = 343.3f,
                .STREAMING_THRESHOLD_IN_SECONDS = 100.0f});

        auto pScene = pContext->make_scene();

        auto pEmitter = pScene->make_emitter(long_tone());

        REQUIRE(std::dynamic_pointer_cast<openal_simple_emitter>(pEmitter) != nullptr);
    }
}
