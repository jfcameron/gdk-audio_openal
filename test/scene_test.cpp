// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/audio/impl_al_emitter.h>
#include <gdk/audio/exception.h>
#include <gdk/audio/emitter.h>
#include <gdk/audio/listener.h>
#include <gdk/audio/openal_context.h>
#include <gdk/audio/openal_emitter.h>
#include <gdk/audio/scene.h>
#include <gdk/audio/sound.h>

#include "tones.h"

#include <gdk/math/constants.h>

#include <AL/al.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <span>

using namespace gdk;
using namespace gdk::audio;

namespace {
    struct fixture final {
        context_shared_ptr_type pContext = openal_context::make();
        scene_shared_ptr_type pScene = pContext->make_scene();

        [[nodiscard]] emitter_shared_ptr_type emitter_for(const sound_shared_ptr_type &apSound) {
            return pScene->make_emitter(apSound);
        }
    };
}

namespace {
    [[nodiscard]] audio_vector3_type al_position_of(const emitter_shared_ptr_type &aEmitter) {
        const auto pImplementation = std::dynamic_pointer_cast<al_emitter>(aEmitter);

        REQUIRE(pImplementation != nullptr);

        ALfloat x = 0, y = 0, z = 0;
        alGetSource3f(pImplementation->source_handle(), AL_POSITION, &x, &y, &z);

        return {x, y, z};
    }

    [[nodiscard]] bool near(const audio_vector3_type &a, const audio_vector3_type &b) {
        return std::abs(a.x - b.x) < 1e-4f && std::abs(a.y - b.y) < 1e-4f
            && std::abs(a.z - b.z) < 1e-4f;
    }
}

TEST_CASE("emitters reach the device in listener space", "[scene][listener]") {
    fixture f;

    auto pEmitter = f.emitter_for(short_tone());
    auto &ears = f.pScene->get_listener();

    SECTION("with the listener at the origin, world space passes through") {
        pEmitter->set_position({1, 2, 3});
        f.pScene->update();

        REQUIRE(near(al_position_of(pEmitter), {1, 2, 3}));
    }

    SECTION("moving the listener onto the emitter puts it at the ear") {
        pEmitter->set_position({10, 0, 0});
        ears.set_position({10, 0, 0});
        f.pScene->update();

        REQUIRE(near(al_position_of(pEmitter), audio_vector3_type::zero));
    }

    SECTION("the listener's position is subtracted, not added") {
        pEmitter->set_position({4, 0, 0});
        ears.set_position({1, 0, 0});
        f.pScene->update();

        REQUIRE(near(al_position_of(pEmitter), {3, 0, 0}));
    }

    SECTION("the listener's rotation is applied as its inverse") {
        pEmitter->set_position({0, 0, -10});
        ears.set_rotation(audio_quaternion_type::from_euler({0, math::numbers::pi_f / 2, 0}));
        f.pScene->update();

        const auto heard = al_position_of(pEmitter);

        REQUIRE(std::abs(heard.y) < 1e-4f);
        REQUIRE(std::abs(heard.z) < 1e-3f);
        REQUIRE(heard.x > 9.0f);
    }

    SECTION("velocity is relative, so shared motion produces no shift") {
        pEmitter->set_velocity({5, 0, 0});
        ears.set_velocity({5, 0, 0});
        f.pScene->update();

        ALfloat x = 0, y = 0, z = 0;
        alGetSource3f(std::dynamic_pointer_cast<al_emitter>(pEmitter)->source_handle(),
            AL_VELOCITY, &x, &y, &z);

        REQUIRE(near({x, y, z}, audio_vector3_type::zero));
    }

    SECTION("every source is flagged listener relative") {
        ALint relative = AL_FALSE;
        alGetSourcei(std::dynamic_pointer_cast<al_emitter>(pEmitter)->source_handle(),
            AL_SOURCE_RELATIVE, &relative);

        REQUIRE(relative == AL_TRUE);
    }
}

TEST_CASE("emitter properties reach the device", "[emitter][listener]") {
    fixture f;

    auto pEmitter = f.emitter_for(short_tone());
    const auto source = std::dynamic_pointer_cast<al_emitter>(pEmitter)->source_handle();

    const auto al_float = [&source](const ALenum aParameter) {
        ALfloat value = 0;
        alGetSourcef(source, aParameter, &value);

        return value;
    };

    SECTION("pitch round trips through OpenAL rather than a local copy") {
        pEmitter->set_pitch(1.5f);

        REQUIRE(pEmitter->pitch() == Approx(1.5f));
        REQUIRE(al_float(AL_PITCH) == Approx(1.5f));
    }

    SECTION("attenuation is handed straight over") {
        pEmitter->set_attenuation(2.0f, 40.0f, 0.75f);

        REQUIRE(al_float(AL_REFERENCE_DISTANCE) == Approx(2.0f));
        REQUIRE(al_float(AL_MAX_DISTANCE) == Approx(40.0f));
        REQUIRE(al_float(AL_ROLLOFF_FACTOR) == Approx(0.75f));
    }

    SECTION("cone angles are converted to degrees") {
        pEmitter->set_cone(math::numbers::pi_f / 2, math::numbers::pi_f, 0.25f);

        REQUIRE(al_float(AL_CONE_INNER_ANGLE) == Approx(90.0f));
        REQUIRE(al_float(AL_CONE_OUTER_ANGLE) == Approx(180.0f));
        REQUIRE(al_float(AL_CONE_OUTER_GAIN) == Approx(0.25f));
    }

    SECTION("gain defaults to unattenuated") {
        f.pScene->update();

        REQUIRE(pEmitter->gain() == Approx(1.0f));
        REQUIRE(al_float(AL_GAIN) == Approx(1.0f));
    }

    SECTION("the listener's gain is folded into the emitter's") {
        pEmitter->set_gain(0.5f);
        f.pScene->get_listener().set_gain(0.5f);
        f.pScene->update();

        REQUIRE(pEmitter->gain() == Approx(0.5f));
        REQUIRE(f.pScene->get_listener().gain() == Approx(0.5f));
        REQUIRE(al_float(AL_GAIN) == Approx(0.25f));
    }

    SECTION("a silent listener silences its scene without touching the emitters") {
        pEmitter->set_gain(1.0f);
        f.pScene->get_listener().set_gain(0);
        f.pScene->update();

        REQUIRE(pEmitter->gain() == Approx(1.0f));
        REQUIRE(al_float(AL_GAIN) == Approx(0.0f));
    }
}

TEST_CASE("direction rotates with the listener but does not translate", "[emitter][listener]") {
    fixture f;

    auto pEmitter = f.emitter_for(short_tone());
    const auto source = std::dynamic_pointer_cast<al_emitter>(pEmitter)->source_handle();

    const auto al_direction = [&source]() {
        ALfloat x = 0, y = 0, z = 0;
        alGetSource3f(source, AL_DIRECTION, &x, &y, &z);

        return audio_vector3_type(x, y, z);
    };

    SECTION("it is unchanged when the listener is at rest") {
        pEmitter->set_direction({0, 0, -1});
        f.pScene->update();

        REQUIRE(near(al_direction(), {0, 0, -1}));
    }

    SECTION("moving the listener leaves it alone") {
        pEmitter->set_direction({0, 0, -1});
        f.pScene->get_listener().set_position({100, 50, -25});
        f.pScene->update();

        REQUIRE(near(al_direction(), {0, 0, -1}));
    }

    SECTION("turning the listener turns it") {
        pEmitter->set_direction({0, 0, -1});
        f.pScene->get_listener().set_rotation(
            audio_quaternion_type::from_euler({0, math::numbers::pi_f / 2, 0}));
        f.pScene->update();

        const auto heard = al_direction();

        REQUIRE(heard.x > 0.99f);
        REQUIRE(std::abs(heard.z) < 1e-3f);
    }

    SECTION("omnidirectional is the default and survives a transform") {
        f.pScene->get_listener().set_transform({7, 8, 9},
            audio_quaternion_type::from_euler({0.3f, 0.4f, 0.5f}));
        f.pScene->update();

        REQUIRE(near(al_direction(), audio_vector3_type::zero));
    }
}
