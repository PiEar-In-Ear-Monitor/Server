#include "gtest/gtest.h"
#include "audio.h"
#include "channel.hpp"
#include <thread>
#include <atomic>

TEST(testPiEar, audio_test)
{
    std::atomic<bool> done = false;
    std::vector<PiEar::channel*> channels;
    PiEar::Audio audio = PiEar::Audio(&done, &channels);
    std::thread audio_thread(&PiEar::Audio::audio_thread, &audio);
    sleep(2);
    done = true;
    audio_thread.join();
    EXPECT_EQ(1000, 1000);
}
