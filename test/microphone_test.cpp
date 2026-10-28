// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include <gdk/audio/exception.h>
#include <gdk/audio/openal_context.h>
#include <gdk/audio/impl_openal_microphone.h>

#include <chrono>
#include <thread>
#include <vector>
#include <span>

using namespace gdk::audio;

namespace {
    [[nodiscard]] microphone::request small_request() {
        return microphone::request{
            .sample_rate = 8000,
            .channel_count = 1,
            .format = pcm_format::signed_16,
            .capacity_in_frames = 8000,
        };
    }
}

TEST_CASE("more channels than openal captures are refused before anything opens", "[microphone][openal]")
{
    auto pContext = openal_context::make();

    auto r = small_request();
    r.channel_count = 6;

    REQUIRE_THROWS_WITH(pContext->make_microphone(r), Catch::Contains("one or two channels"));
}

TEST_CASE("capturing from a real device", "[microphone][openal]")
{
    auto pContext = openal_context::make();

    if (pContext->capture_device_names().empty())
    {
        WARN("no capture device on this machine; the openal capture path was not exercised");

        return;
    }

    microphone_shared_ptr_type pMic;

    try
    {
        pMic = pContext->make_microphone(small_request());
    }
    catch (const exception &e)
    {
        WARN("a capture device is listed but would not open (" << e.what() << "); the openal capture path was not exercised");

        return;
    }

    REQUIRE(pMic != nullptr);

    SECTION("it reports what was opened, not what was asked for")
    {
        REQUIRE_FALSE(pMic->device_name().empty());
        REQUIRE(pMic->sample_rate() == 8000);
        REQUIRE(pMic->channel_count() == 1);
        REQUIRE(pMic->capacity_in_frames() == 8000);
    }

    SECTION("nothing is captured before start")
    {
        pMic->update();

        REQUIRE(pMic->available_in_frames() == 0);
        REQUIRE_FALSE(pMic->capturing());
    }

    SECTION("and frames arrive once it is started and polled")
    {
        const auto began = std::chrono::steady_clock::now();

        pMic->start();

        REQUIRE(pMic->capturing());

        for (int i = 0; i < 10; ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));

            pMic->update();
        }

        pMic->stop();

        const auto elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - began).count();

        REQUIRE_FALSE(pMic->capturing());

        const auto frames = pMic->available_in_frames();
        const auto expected = elapsed * 8000.0;

        INFO("captured " << frames << " frames over " << elapsed << "s, expected about " << expected);

        REQUIRE(frames > 0);
        REQUIRE(static_cast<double>(frames) < expected * 1.5);

        SECTION("which become a sound describing them")
        {
            auto pSound = pMic->take();

            REQUIRE(pSound != nullptr);
            REQUIRE(pSound->channel_count() == 1);
            REQUIRE(pSound->sample_rate() == 8000);
            REQUIRE(pSound->duration() == Approx(static_cast<double>(frames) / 8000.0));

            REQUIRE(pMic->available_in_frames() == 0);
        }

        SECTION("or are read out in chunks")
        {
            std::vector<unsigned char> out(200);

            const auto read = pMic->read(std::as_writable_bytes(std::span(out)));

            REQUIRE(read == 100);
            REQUIRE(pMic->available_in_frames() == frames - 100);
        }
    }
}
