// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/audio/emitter.h>
#include <gdk/audio/ext/vorbis.h>
#include <gdk/audio/openal_context.h>
#include <gdk/audio/impl_openal_simple_emitter.h>
#include <gdk/audio/impl_openal_stream_emitter.h>
#include <gdk/audio/scene.h>

#include "data/long_tone_ogg.h"
#include "data/short_tone_ogg.h"

#include <cstddef>
#include <memory>
#include <vector>

using namespace gdk::audio;

namespace {
    template <std::size_t size_type>
    [[nodiscard]] std::vector<std::byte> bytes_of(const unsigned char (&aData)[size_type]) {
        const auto *const from = reinterpret_cast<const std::byte *>(aData);

        return {from, from + size_type};
    }
}

TEST_CASE("a vorbis sound plays as any other, short ones whole and long ones streamed", "[vorbis]") {
    auto pContext = openal_context::make();
    auto pScene = pContext->make_scene();

    auto pShort = pScene->make_emitter(ext::make_vorbis_sound(bytes_of(short_tone_ogg)));
    auto pLong = pScene->make_emitter(ext::make_vorbis_sound(bytes_of(long_tone_ogg)));

    REQUIRE(std::dynamic_pointer_cast<openal_simple_emitter>(pShort) != nullptr);
    REQUIRE(std::dynamic_pointer_cast<openal_stream_emitter>(pLong) != nullptr);

    pShort->play();
    pLong->play();

    REQUIRE(pShort->is_playing());
    REQUIRE(pLong->is_playing());
}
