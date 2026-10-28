// © Joseph Cameron - MIT License

#ifndef GDK_AUDIO_OPENAL_POLICY_H
#define GDK_AUDIO_OPENAL_POLICY_H

#include <gdk/audio/types.h>

#include <cstddef>

namespace gdk::audio {
    /// \brief how a sound's volume falls off with distance
    enum class distance_model {
        none,
        inverse,
        inverse_clamped,
        linear,
        linear_clamped,
        exponent,
        exponent_clamped
    };

    /// \brief user-definable configuration for the implementation
    struct openal_policy final {
        /// \brief the attenuation curve every emitter is heard through
        gdk::audio::distance_model distance_model{gdk::audio::distance_model::linear_clamped};

        /// \brief how pronounced doppler shift is. 0 disables it, 1 is physical.
        audio_floating_point_type doppler_factor{1};

        /// \brief propagation speed in world units per second, against which doppler is computed.
        /// The default is metres per second in dry air, so it assumes one unit is one metre.
        audio_floating_point_type speed_of_sound{343.3f};

        /// \brief a sound longer than this is streamed rather than decoded up front
        audio_floating_point_type streaming_threshold_in_seconds{5};

        /// \brief how much audio one streaming buffer holds
        std::size_t stream_frames_per_buffer{4096};

        /// \brief how many buffers a streaming emitter cycles through. Two is the minimum that lets
        /// one play while the next is filled.
        std::size_t stream_buffer_count{2};

        /// \brief how much audio is pulled per read while decoding a sound up front
        std::size_t decode_frames_per_read{4096};
    };
}

#endif
