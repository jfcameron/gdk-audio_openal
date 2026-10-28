// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/audio/emitter.h>
#include <gdk/audio/emitter_group.h>
#include <gdk/audio/null_context.h>
#include <gdk/audio/openal_context.h>
#include <gdk/audio/openal_policy.h>
#include <gdk/audio/scene.h>
#include <gdk/audio/sound.h>

#include "tones.h"

#include <array>
#include <chrono>
#include <thread>

using namespace gdk;
using namespace gdk::audio;

namespace {
    void pump(const scene_shared_ptr_type &aScene, const float aSeconds) {
        const auto until = std::chrono::steady_clock::now() + std::chrono::duration<float>(aSeconds);

        while (std::chrono::steady_clock::now() < until) {
            aScene->update();
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }

    constexpr openal_policy STREAMING{
        .distance_model = distance_model::linear_clamped,
        .doppler_factor = 1,
        .speed_of_sound = 343.3f,
        .streaming_threshold_in_seconds = 0,
    };
}

TEST_CASE("emitters in a group start, stop, pause and resume as one", "[group]") {
    auto pContext = openal_context::make(STREAMING);
    auto pScene = pContext->make_scene();

    auto pSound = long_tone();

    const std::array<emitter_shared_ptr_type, 3> members{
        pScene->make_emitter(pSound), pScene->make_emitter(pSound), pScene->make_emitter(pSound)};

    auto pGroup = pScene->make_emitter_group(std::span(members));

    REQUIRE(pGroup->size() == 3);
    REQUIRE_FALSE(pGroup->is_playing());

    pGroup->play();

    REQUIRE(pGroup->is_playing());
    for (const auto &pMember : members) REQUIRE(pMember->is_playing());

    pump(pScene, 0.2f);

    pGroup->pause();

    REQUIRE(pGroup->is_paused());
    REQUIRE_FALSE(pGroup->is_playing());
    for (const auto &pMember : members) REQUIRE(pMember->is_paused());

    pGroup->resume();

    REQUIRE(pGroup->is_playing());
    for (const auto &pMember : members) REQUIRE(pMember->is_playing());

    pump(pScene, 0.2f);

    pGroup->stop();

    REQUIRE_FALSE(pGroup->is_playing());
    for (const auto &pMember : members) REQUIRE_FALSE(pMember->is_playing());

}
