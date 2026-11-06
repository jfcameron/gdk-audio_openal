// © Joseph Cameron - MIT License

#include <gdk/audio/context.h>
#include <gdk/audio/emitter.h>
#include <gdk/audio/listener.h>
#include <gdk/audio/openal_context.h>
#include <gdk/audio/openal_policy.h>
#include <gdk/audio/scene.h>
#include <gdk/audio/sound.h>

#include <gdk/math/constants.h>

#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <numbers>
#include <thread>
#include <vector>

using namespace gdk;

namespace {
    constexpr float ORBIT_RADIUS = 6.0f;
    constexpr float FRAME_SECONDS = 1.0f / 60.0f;
    constexpr float WALK_AWAY_DISTANCE = 45.0f;
    constexpr std::size_t TONE_SAMPLE_RATE = 44100;
    constexpr std::size_t TONE_FRAMES = TONE_SAMPLE_RATE * 16;
    constexpr double TONE_HERTZ = 440.0;
    constexpr double TONE_AMPLITUDE = 8000.0;

    [[nodiscard]] float lerp(const float a, const float b, const float t) {
        return a + (b - a) * t;
    }

    [[nodiscard]] audio::sound_shared_ptr_type make_tone() {
        std::vector<std::int16_t> samples(TONE_FRAMES);

        for (std::size_t i = 0; i < samples.size(); ++i) {
            const auto seconds = static_cast<double>(i) / static_cast<double>(TONE_SAMPLE_RATE);

            samples[i] = static_cast<std::int16_t>(
                TONE_AMPLITUDE * std::sin(2.0 * std::numbers::pi * TONE_HERTZ * seconds));
        }

        return audio::make_pcm_sound(samples.data(), samples.size(), 1, TONE_SAMPLE_RATE, audio::pcm_format::signed_16);
    }
}

int main() {
    const audio::openal_policy policy{
        .distance_model = audio::distance_model::inverse_clamped,
        .doppler_factor = 1.0f,                 
        .speed_of_sound = 343.3f,               
    };

    auto pAudio = audio::openal_context::make(policy);

    auto pScene = pAudio->make_scene();

    auto pSound = make_tone();

    std::cout << "sound: " << pSound->channel_count() << " channel, "
        << pSound->sample_rate() << " Hz, " << pSound->duration() << "s\n";

    auto pEmitter = pScene->make_emitter(pSound);
    auto &ears = pScene->get_listener();

    pEmitter->set_position({0, 0, 0});

    pEmitter->set_attenuation(1.0f, 40.0f, 1.0f);

    pEmitter->play();

    const float phase = pSound->duration() / 4.0f;

    int announced = -1;

    for (float elapsed = 0; pEmitter->is_playing(); elapsed += FRAME_SECONDS) {
        const int current = static_cast<int>(elapsed / phase);
        const float t = std::fmod(elapsed, phase) / phase;   

        if (current != announced) {
            static const char *const NAMES[] = {
                "1/4 orbit: the listener circles the emitter. Only the ears move.",
                "2/4 attenuation: the listener walks away and back. set_attenuation decides the curve.",
                "3/4 cone: the emitter now points one way. Outside the cone it drops to a quarter.",
                "4/4 gain: the emitter fades out, then the listener fades the whole scene out."
            };

            if (current < 4) std::cout << NAMES[current] << "\n";

            announced = current;
        }

        switch (current) {
            case 0: {
                const float angle = 2.0f * math::numbers::pi_f * t;

                ears.set_transform({std::sin(angle) * ORBIT_RADIUS, 0, std::cos(angle) * ORBIT_RADIUS},
                    audio::audio_quaternion_type::from_euler({0, angle, 0}));

                const float speed = 2.0f * math::numbers::pi_f * ORBIT_RADIUS / phase;

                ears.set_velocity({std::cos(angle) * speed, 0, -std::sin(angle) * speed});
            } break;

            case 1: {
                const float away = t < 0.5f ? t * 2 : (1 - t) * 2;

                ears.set_transform({0, 0, lerp(1.0f, WALK_AWAY_DISTANCE, away)},
                    audio::audio_quaternion_type::identity);

                ears.set_velocity(audio::audio_vector3_type::zero);
            } break;

            case 2: {
                pEmitter->set_direction({1, 0, 0});
                pEmitter->set_cone(math::numbers::pi_f / 4, math::numbers::pi_f / 2, 0.25f);

                const float angle = 2.0f * math::numbers::pi_f * t;

                ears.set_transform({std::sin(angle) * ORBIT_RADIUS, 0, std::cos(angle) * ORBIT_RADIUS},
                    audio::audio_quaternion_type::from_euler({0, angle, 0}));
            } break;

            case 3: {
                pEmitter->set_direction(audio::audio_vector3_type::zero);

                ears.set_transform({0, 0, 2}, audio::audio_quaternion_type::identity);

                pEmitter->set_gain(t < 0.5f ? lerp(1.0f, 0.25f, t * 2) : 0.25f);

                ears.set_gain(t < 0.5f ? 1.0f : lerp(1.0f, 0.0f, (t - 0.5f) * 2));
            } break;

            default: break;
        }

        pScene->update();

        std::this_thread::sleep_for(std::chrono::duration<float>(FRAME_SECONDS));
    }

    return EXIT_SUCCESS;
}
