#include "gtest/gtest.h"
#include "http-server.h"
#include "gen_channels.hpp"
#include "channel.hpp"
#include <atomic>
#include <thread>
#include <vector>

TEST(testPiEar, http_server) {
    std::atomic<bool> kill = false;
    std::atomic<int> bpm = 100;
    std::vector<PiEar::channel*> *channels = generate_channels(5);
    std::thread server(PiEar::mainloop_http_server, &kill, channels, &bpm, "ws_test", 2);
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
    sleep(2); // Wait for server to initialize
    node_thread = fork();
    if (!node_thread) {
        std::vector<char*> server_args;
        server_args.push_back(const_cast<char*>("afterSetup.js"));
        server_args.push_back(nullptr);
        char **command = server_args.data();
        execve("../PiEar_HTTP_Server/tests/afterSetup.js", &command[0], nullptr);
        return; // Redundant
    }
    waitpid(node_thread, nullptr, 0);
    node_thread = fork();
    if (!node_thread) {
        std::vector<char*> server_args;
        server_args.push_back(const_cast<char*>("testValidators.js"));
        server_args.push_back(nullptr);
        char **command = server_args.data();
        execve("../PiEar_HTTP_Server/tests/testValidators.js", &command[0], nullptr);
        return; // Redundant
    }
    waitpid(node_thread, nullptr, 0);
//    while(true); // For manual testing
    kill = true;
    server.join();
    EXPECT_EQ((*channels)[1]->channel_name, "U29tZV9OYW1l"); // Some_Name
    EXPECT_EQ((*channels)[2]->channel_name, "S2V5Ym9hcmQ="); // Keyboard
    EXPECT_EQ((*channels)[3]->channel_name, "R3VpdGFy"); // Guitar
    EXPECT_EQ((*channels)[4]->channel_name, "TmV3X0NoYW5uZWw="); // New_Channel
    EXPECT_EQ(bpm, 200);
    for (int i = 0; i < 5; i++) {
        delete (*channels)[i];
    }
    delete channels;
}
