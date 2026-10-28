// © Joseph Cameron - MIT License

#ifndef GDK_AUDIO_IMPL_OPENAL_SIMPLE_EMITTER_H
#define GDK_AUDIO_IMPL_OPENAL_SIMPLE_EMITTER_H

#include <gdk/audio/impl_al_emitter.h>
#include <gdk/audio/openal_policy.h>
#include <gdk/audio/sound.h>

#include <jfc/shared_handle.h>

#include <AL/al.h>
#include <AL/alc.h>

#include <memory>
#include <optional>

namespace gdk::audio {
	/// \brief emitter that decodes its sound once rather than streaming it
    class openal_simple_emitter : public al_emitter {
		jfc::shared_handle<ALuint> m_ALBufferHandle;

		ALsizei m_SampleRate;

		double m_SecondsPerLoop = 0;

		std::size_t m_Loops = 0;

		double m_LastOffset = 0;

    public:
        openal_simple_emitter(openal_session_ptr_type apSession, const sound_shared_ptr_type &apSound,
            const openal_policy &aPolicy);

        virtual void set_looping(const bool aLooping) override;

        virtual void play() override;

		virtual void stop() override;

		virtual void discard_queued() override;

		[[nodiscard]] virtual std::optional<double> playback_position() const override;

		virtual void begin_play() override;

        virtual void update() override;
    };
}

#endif
