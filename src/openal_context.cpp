// © Joseph Cameron - MIT License

#include <gdk/audio/openal_context.h>

#include <gdk/audio/exception.h>
#include <gdk/audio/impl_al_scene.h>
#include <gdk/audio/impl_efx.h>
#include <gdk/audio/impl_openal_microphone.h>
#include <gdk/audio/impl_openal_session.h>

#include <AL/al.h>
#include <AL/alc.h>

#include <string>

namespace gdk::audio {
    namespace {
        bool gSessionIsOpen = false;

        [[nodiscard]] std::string al_error_code_to_string(const ALenum aError) {
            switch (aError) {
                case AL_NO_ERROR: return "AL_NO_ERROR";
                case AL_INVALID_NAME: return "AL_INVALID_NAME";
                case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
                case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
                case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
                case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
            }

            return "unrecognized OpenAL error code " + std::to_string(aError);
        }

        [[nodiscard]] std::string alc_failure(ALCdevice *const apDevice) {
            const auto error = alcGetError(apDevice);
            const auto *const pError = alcGetString(apDevice, error);
            const auto *const pName = apDevice ? alcGetString(apDevice, alcIsExtensionPresent(apDevice, "ALC_ENUMERATE_ALL_EXT")
                ? ALC_ALL_DEVICES_SPECIFIER : ALC_DEVICE_SPECIFIER) : nullptr;

            return std::string(pName ? pName : "the default device") + ": "
                + (pError ? pError : "unrecognized ALC error code " + std::to_string(error));
        }

        [[nodiscard]] ALenum al_distance_model_from(const distance_model aModel) {
            switch (aModel) {
                case distance_model::none: return AL_NONE;
                case distance_model::inverse: return AL_INVERSE_DISTANCE;
                case distance_model::inverse_clamped: return AL_INVERSE_DISTANCE_CLAMPED;
                case distance_model::linear: return AL_LINEAR_DISTANCE;
                case distance_model::linear_clamped: return AL_LINEAR_DISTANCE_CLAMPED;
                case distance_model::exponent: return AL_EXPONENT_DISTANCE;
                case distance_model::exponent_clamped: return AL_EXPONENT_DISTANCE_CLAMPED;
            }

            throw exception("unrecognized distance model");
        }
    }

    openal_session::openal_session(const openal_policy &aPolicy) {
        if (gSessionIsOpen) throw exception("a gdk::audio openal context's device is still open. OpenAL makes one "
            "context current per process, so a second would silence the first");

        m_pDevice = alcOpenDevice(nullptr);

        if (!m_pDevice) throw exception("could not open an audio device: " + alc_failure(nullptr));

        const auto fail = [this](const std::string &aWhy) {
            alcMakeContextCurrent(nullptr);

            if (m_pContext) alcDestroyContext(m_pContext);

            alcCloseDevice(m_pDevice);

            throw exception(aWhy);
        };

        m_pContext = alcCreateContext(m_pDevice, nullptr);

        if (!m_pContext) fail("could not start the audio device, " + alc_failure(m_pDevice));

        if (!alcMakeContextCurrent(m_pContext)) fail("could not make the audio context current");

        if (const auto error = alGetError(); error != AL_NO_ERROR) fail(al_error_code_to_string(error));

        alDistanceModel(al_distance_model_from(aPolicy.distance_model));
        alDopplerFactor(aPolicy.doppler_factor);
        alSpeedOfSound(aPolicy.speed_of_sound);

        gSessionIsOpen = true;
    }

    openal_session::~openal_session() {
        forget_efx();

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_pContext);
        alcCloseDevice(m_pDevice);

        gSessionIsOpen = false;
    }

    ALCdevice *openal_session::device() const { return m_pDevice; }

    std::shared_ptr<openal_context> openal_context::make(openal_policy aPolicy) {
        return std::shared_ptr<openal_context>(new openal_context(std::move(aPolicy)));
    }

    openal_context::openal_context(openal_policy aPolicy)
    : m_Policy(std::move(aPolicy))
    , m_pSession(std::make_shared<openal_session>(m_Policy))
    {}

    openal_context::~openal_context() = default;

    std::vector<std::string> openal_context::capture_device_names() const {
        if (!alcIsExtensionPresent(nullptr, "ALC_EXT_CAPTURE")) return {};

        const auto *pAt = alcGetString(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER);

        if (!pAt) return {};

        std::vector<std::string> out;

        while (*pAt) {
            out.emplace_back(pAt);

            pAt += out.back().size() + 1;
        }

        return out;
    }

    microphone_shared_ptr_type openal_context::make_microphone(const microphone::request &aRequest) {
        return std::make_shared<openal_microphone>(aRequest);
    }

    std::shared_ptr<openal_scene> openal_context::make_openal_scene() {
        return std::make_shared<al_scene>(m_pSession, m_Policy);
    }

    scene_shared_ptr_type openal_context::make_scene() { return make_openal_scene(); }
}
