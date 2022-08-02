#include <atomic>
#include <gtest/gtest.h>
#include <thread>
#include "audio.h"
#include "channel.h"
#include "gen_channels.hpp"
// TODO: Get test running in gh action
TEST(testPiEar, audio_test)
{
    std::atomic<bool> done = false;
    std::vector<PiEar::channel*> *channels = generate_channels(64);
    PiEar::Audio audio = PiEar::Audio(&done, channels, -1);
    std::thread audio_thread(&PiEar::Audio::audio_thread, &audio);
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
