// © Joseph Cameron - MIT License

#ifndef GDK_AUDIO_OPENAL_SCENE_H
#define GDK_AUDIO_OPENAL_SCENE_H

#include <gdk/audio/openal_effects.h>
#include <gdk/audio/scene.h>

namespace gdk::audio {
    /// \brief a scene of an openal context, which can give every emitter in it a shared effect
    class openal_scene : public scene {
    public:
        //! give this scene a reverb, heard by every emitter in it. Throws where the OpenAL has no effects extension, and the scene stays dry
        virtual void set_reverb(const reverb_parameters &aParameters) = 0;

        //! give this scene an echo, heard by every emitter in it. Throws as set_reverb does
        virtual void set_echo(const echo_parameters &aParameters) = 0;

        //! **removes whatever send effect this scene has**
        virtual void clear_send_effect() = 0;

        //! **whether this scene has a send effect**
        [[nodiscard]] virtual bool has_send_effect() const = 0;

        virtual ~openal_scene() override = default;

    protected:
        openal_scene() = default;
    };
}

#endif
