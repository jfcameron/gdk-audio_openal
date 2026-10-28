// © Joseph Cameron - MIT License

#ifndef GDK_AUDIO_OPENAL_CONTEXT_H
#define GDK_AUDIO_OPENAL_CONTEXT_H

#include <gdk/audio/context.h>
#include <gdk/audio/openal_policy.h>
#include <gdk/audio/openal_scene.h>

#include <memory>
#include <string>
#include <vector>

namespace gdk::audio {
    class openal_session;

    /// \brief openal implementation of context
    ///
    /// The device stays open while anything made from this context lives, scenes and emitters included.
    class openal_context final : public context {
    public:
        //! opens the default device. Throws if none can be opened, or while another context's device is open: OpenAL makes one context current per process. Not thread-safe.
        [[nodiscard]] static std::shared_ptr<openal_context> make(openal_policy aPolicy = {});

        //! an openal_scene, as make_openal_scene
        [[nodiscard]] virtual scene_shared_ptr_type make_scene() override;

        [[nodiscard]] std::shared_ptr<openal_scene> make_openal_scene();

        [[nodiscard]] virtual std::vector<std::string> capture_device_names() const override;

        [[nodiscard]] virtual microphone_shared_ptr_type make_microphone(
            const microphone::request &aRequest) override;

        virtual ~openal_context() override;

    private:
        explicit openal_context(openal_policy aPolicy);

        openal_policy m_Policy;

        std::shared_ptr<openal_session> m_pSession;
    };
}

#endif
