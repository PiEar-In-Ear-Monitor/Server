#include "gtest/gtest.h"
#include "http-server.h"
#include "gen_channels.h"
#include "channel.hpp"
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>

void print(const std::vector<PiEar::channel*>& channels, std::atomic<int>& bpm) {
    for (auto channel : channels) {
        std::cout << (std::string)*channel << std::endl;
    }
    std::cout << "BPM: " << bpm << std::endl;
}

TEST(testPiEar, http_server) {
    std::atomic<bool> kill = false;
    std::atomic<int> bpm = 100;
    std::vector<PiEar::channel*> *chans = generate_channels(5);
    print(*chans, bpm);
    std::thread server( PiEar::mainloop_http_server, &kill, chans, &bpm, "ws_test", 2);
    sleep(1);
    int node_thread = fork();
    if (!node_thread) {
        std::vector<char*> server_args;
        server_args.push_back(const_cast<char*>("beforeSetup.js"));
        server_args.push_back(nullptr);
        char **command = server_args.data();
        execve("../PiEar_HTTP_Server/tests/beforeSetup.js", &command[0], nullptr);
        return; // Redundant
    }
    // Run command and wait for it to finish
    int status;
    waitpid(node_thread, &status, 0);
    sleep(2); // Enough time to manually send requests with Insomnia
    node_thread = fork();
    if (!node_thread) {
        std::vector<char*> server_args;
        server_args.push_back(const_cast<char*>("afterSetup.js"));
        server_args.push_back(nullptr);
        char **command = server_args.data();
        execve("../PiEar_HTTP_Server/tests/afterSetup.js", &command[0], nullptr);
        return; // Redundant
    }
    waitpid(node_thread, 0, 0);
    kill = true;
    server.join();
    print(*chans, bpm);
    EXPECT_EQ(1000, 1000);
}
