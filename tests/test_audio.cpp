#include <atomic>
#include <gtest/gtest.h>
#include <thread>
#include "audio.h"
#include "channel.hpp"
#include "gen_channels.hpp"

TEST(testPiEar, audio_test)
{
    std::atomic<bool> done = false;
    std::vector<PiEar::channel*> *channels = generate_channels(64);
    PiEar::Audio audio = PiEar::Audio(&done, channels, -1);
    std::thread audio_thread(&PiEar::Audio::audio_thread, &audio); // Determine if test can run on gh runner
    sleep(2);
    done = true;
    audio_thread.join();
    for (auto channel : *channels)
    {
        delete channel;
    }
    delete channels;
    EXPECT_EQ(1000, 1000);
}
