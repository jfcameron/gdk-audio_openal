// © Joseph Cameron - MIT License

#ifndef GDK_AUDIO_IMPL_AL_SCENE_H
#define GDK_AUDIO_IMPL_AL_SCENE_H

#include <gdk/audio/impl_al_emitter.h>
#include <gdk/audio/impl_openal_emitter_group.h>
#include <gdk/audio/impl_openal_listener.h>
#include <gdk/audio/impl_openal_session.h>
#include <gdk/audio/openal_effects.h>
#include <gdk/audio/openal_policy.h>
#include <gdk/audio/openal_scene.h>

#include <jfc/shared_handle.h>

#include <memory>
#include <span>
#include <vector>

namespace gdk::audio {
    class al_scene final : public openal_scene {
    public:
        using emitter_collection_type = std::vector<std::weak_ptr<al_emitter>>;

        using group_collection_type = std::vector<std::weak_ptr<openal_emitter_group>>;

        al_scene(openal_session_ptr_type apSession, openal_policy aPolicy);

        [[nodiscard]] virtual listener &get_listener() override;

        [[nodiscard]] virtual const listener &get_listener() const override;

        [[nodiscard]] virtual emitter_shared_ptr_type make_emitter(
            sound_shared_ptr_type aSound) override;

        [[nodiscard]] virtual emitter_shared_ptr_type make_emitter(sound_shared_ptr_type aSound,
            insert_effect_collection_type aInserts) override;

        [[nodiscard]] virtual emitter_shared_ptr_type make_emitter(sound_shared_ptr_type aSound,
            std::span<const audio_vector3_type> aChannelPositions) override;

        [[nodiscard]] virtual emitter_group_shared_ptr_type make_emitter_group(
            std::span<const emitter_shared_ptr_type> aEmitters) override;

        virtual void remove(const emitter_shared_ptr_type &aEmitter) override;

        [[nodiscard]] virtual std::size_t emitter_count() const override;

        virtual void set_reverb(const reverb_parameters &aParameters) override;

        virtual void set_echo(const echo_parameters &aParameters) override;

        virtual void clear_send_effect() override;

        [[nodiscard]] virtual bool has_send_effect() const override;

        /// \brief the underlying auxiliary effect slot, or AL_EFFECTSLOT_NULL if this scene is dry
        [[nodiscard]] ALuint effect_slot_handle() const;

        virtual void update() override;

    private:
        //! the emitter for a sound, its inserts and its channels' places, placed and routed
        [[nodiscard]] emitter_shared_ptr_type add_emitter(sound_shared_ptr_type aSound,
            insert_effect_collection_type aInserts, std::vector<audio_vector3_type> aChannelOffsets);

        //! makes the slot and effect if this scene has none yet, and sets the effect's type
        [[nodiscard]] ALuint ensure_effect(const ALint aType);

        //! puts the configured effect in the slot and wires every emitter to it
        void bind_effect_to_slot();

        openal_session_ptr_type m_pSession;
        openal_policy m_Policy;
        openal_listener m_Listener;
        emitter_collection_type m_Emitters;
        group_collection_type m_Groups;

        jfc::shared_handle<ALuint> m_Effect;
        jfc::shared_handle<ALuint> m_EffectSlot;
    };
}

#endif
