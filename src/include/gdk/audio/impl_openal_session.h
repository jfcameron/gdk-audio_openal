// © Joseph Cameron - MIT License

#ifndef GDK_AUDIO_IMPL_OPENAL_SESSION_H
#define GDK_AUDIO_IMPL_OPENAL_SESSION_H

#include <gdk/audio/openal_policy.h>

#include <AL/alc.h>

#include <memory>

namespace gdk::audio {
    /// \brief the device and the AL context made current on it, kept open by whatever was made from it
    class openal_session final {
    public:
        explicit openal_session(const openal_policy &aPolicy);

        ~openal_session();

        openal_session(const openal_session &) = delete;
        openal_session &operator=(const openal_session &) = delete;

        [[nodiscard]] ALCdevice *device() const;

    private:
        ALCdevice *m_pDevice = nullptr;
        ALCcontext *m_pContext = nullptr;
    };

    using openal_session_ptr_type = std::shared_ptr<openal_session>;
}

#endif
