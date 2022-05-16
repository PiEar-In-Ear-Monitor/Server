//******************************************************************************
// Copyright © 2021  Alexander O'Connor
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//******************************************************************************

#include "multicast-server.h"
#include "http-server.h"
#include "channel.hpp"
#include "click.h"
#include "audio.h"
#include "queue.hpp"
#include <thread>
#include <atomic>
#include <iostream>

std::string random_string(std::size_t length) {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);
    std::string random_string;
    for (std::size_t i = 0; i < length; ++i) {
        random_string += CHARACTERS[distribution(generator)];
    }
    return random_string;
}

int main(int argc, char *argv[]) {
    // Click
    std::atomic<bool> click = false, kill_click = false, kill_http = false, kill_multicast = false;
    std::atomic<int> cpm = 100;
    queue<uint16_t*> audio_queue;
    std::thread click_thread(PiEar::mainloop_click, &cpm, &click, &kill_click);
    int number_of_audio_channels = 11;
    std::vector<PiEar::channel*> channels;
    for (int i = 0; i < number_of_audio_channels; i++) {
        channels.push_back(new PiEar::channel(i, i, "Channel " + std::to_string(i), true));
    }
    std::thread webserver_thread(PiEar::mainloop_http_server, &kill_http, &channels, &cpm, random_string(20), 1);
    std::thread multicast_thread(PiEar::mainloop_multicast_server, &channels, &click, &kill_multicast);
    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();
    kill_click = true;
    kill_http = true;
    kill_multicast = true;
    click_thread.join();
    webserver_thread.join();
    multicast_thread.join();
}
