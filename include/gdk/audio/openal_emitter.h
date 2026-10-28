// © Joseph Cameron - MIT License

#ifndef GDK_AUDIO_OPENAL_EMITTER_H
#define GDK_AUDIO_OPENAL_EMITTER_H

#include <gdk/audio/emitter.h>
#include <gdk/audio/openal_effects.h>

namespace gdk::audio {
    /// \brief an emitter of an openal scene: every emitter an openal scene makes is one
    ///
    /// \warning for 3d spatial effects to be applied, its sound MUST be mono. A stereo or
    /// multichannel sound plays flat, as a speaker bed, unless it was made with a place for each channel.
    class openal_emitter : public emitter {
    public:
        //! muffle this emitter. Throws where the OpenAL has no effects extension; gdk-audio's make_lowpass_insert filters on any
        virtual void set_lowpass(const lowpass_parameters &aParameters) = 0;

        //! **removes the muffling**
        virtual void clear_lowpass() = 0;

        virtual ~openal_emitter() override = default;

    protected:
        openal_emitter() = default;
    };
}

#endif
