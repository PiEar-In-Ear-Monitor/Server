#include <atomic>
#include <iostream>
#include <random>
#include <thread>
#include "audio.h"
#include "channel.hpp"
#include "click.h"
#include "http-server.h"
#include "multicast-server.h"

std::string random_string(std::size_t length) {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, 61); // 61 is length of CHARACTERS
    std::string random_string;
    for (std::size_t i = 0; i < length; ++i) {
        random_string += CHARACTERS[distribution(generator)];
    }
    return random_string;
}
int main(int argc, char *argv[]) {
    // Click
    std::atomic<bool> click = false, kill_audio = false, kill_click = false, kill_http = false, kill_multicast = false;
    std::atomic<int> cpm = 100;
    std::vector<PiEar::channel*> channels;
    PiEar::Audio audio = PiEar::Audio(&kill_audio, &channels);
    if (audio.channel_count() < 1) {
        std::cout << "No audio channels found" << std::endl;
        return 1;
    }
    for (int i = 0; i < audio.channel_count(); i++) {
        channels.push_back(new PiEar::channel(i, "Channel " + std::to_string(i)));
    }
    // Start threads
    std::thread audio_thread(&PiEar::Audio::audio_thread, &audio); // Determine if test can run on gh runner
    std::thread click_thread(PiEar::mainloop_click, &cpm, &click, &kill_click);
    std::thread webserver_thread(PiEar::mainloop_http_server, &kill_http, &channels, &cpm, random_string(20), 1);
    std::thread multicast_thread(PiEar::mainloop_multicast_server, &channels, &click, &kill_multicast);
    // Wait for any input
    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();
    // Kill threads
    kill_audio = true;
    kill_click = true;
    kill_http = true;
    kill_multicast = true;
    audio_thread.join();
    click_thread.join();
    webserver_thread.join();
    multicast_thread.join();
    // Clean up
    for (auto & channel : channels) {
        delete channel;
    }
}
