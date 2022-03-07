#include "gtest/gtest.h"
#include "http-server.h"
#include "gen_channels.h"
#include "channel.hpp"
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>

TEST(testPiEar, http_server) {
    std::atomic<bool> kill = false;
    std::atomic<int> bpm = 100;
    std::vector<PiEar::channel*> *chans = generate_channels(5);
    std::thread server( PiEar::mainloop_http_server, &kill, chans, &bpm);
    sleep(10);
    kill = true;
    server.join();
    for (auto &chan : *chans) {
        std::cout << (std::string)*chan << std::endl;
    }
    std::cout << "BPM: " << bpm << std::endl;
    EXPECT_EQ(1000, 1000);
}
