#include <atomic>
#include <iostream>
#include <portaudio.h>
#include <random>
#include <csignal>
#include <thread>
#include "audio.h"
#include "channel.hpp"
#include "click.h"
#include "http-server.h"
#include "multicast-server.h"
#include "task.hpp"

// For getting the home directory.
#include <pwd.h>
#include <unistd.h>

#define PIEAR_SETTINGS_FILE ".config/piear/piear_settings.json"

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

bool SIG_HANDLER_KILL_SERVER = false;

int main(int argc, char *argv[]) {
    // Click
    std::atomic<bool> click = false, kill_audio = false, kill_click = false, kill_http = false, kill_multicast = false, change_audio_source = false;
    std::atomic<int> cpm = 100, audio_index = 0;
    std::vector<PiEar::channel*> channels;
    std::string filename;
    if (getenv("HOME") == nullptr) {
        filename = getpwuid(getuid())->pw_dir;
    } else {
        filename = getenv("HOME");
    }
    filename += PIEAR_SETTINGS_FILE;
    PiEar::task task = PiEar::task(&channels, filename, 3); // TODO: Change to 30
    audio_index = task.audio_index;
    int err = Pa_Initialize();
    if( err != paNoError ){
        throw std::runtime_error("PortAudio error initializing");
    }
    PiEar::Audio audio = PiEar::Audio(&kill_audio, &channels, audio_index);
    if (audio_index != audio.audio_index) {
        audio_index = audio.audio_index;
    }
    std::vector<PiEar::audioDevice> info = audio.get_audio_devices();
    if (audio.channel_count(audio.audio_index) < 1) {
        std::cout << "No audio channels found" << std::endl;
        return 1;
    }
    for (int i = 0; i < audio.channel_count(audio.audio_index); i++) {
        channels.push_back(new PiEar::channel(i));
    }
    task.load_from_file();
    task.async_run_save_task();
    std::thread audio_thread(&PiEar::Audio::audio_thread, &audio); // Determine if test can run on gh runner
    std::thread click_thread(PiEar::mainloop_click, &cpm, &click, &kill_click);
    std::thread webserver_thread(PiEar::mainloop_http_server, &kill_http, &channels, &cpm, random_string(20), 1, info, &audio_index, &change_audio_source);
    std::thread multicast_thread(PiEar::mainloop_multicast_server, &channels, &click, &kill_multicast);
    signal(SIGINT, [](int signum) { SIG_HANDLER_KILL_SERVER = true; });
    while (!SIG_HANDLER_KILL_SERVER) {
        if (change_audio_source) {
            kill_audio = kill_http = kill_multicast = true;
            task.async_stop_save_task();
            audio_thread.join();
            webserver_thread.join();
            multicast_thread.join();
            change_audio_source = kill_audio = kill_http = kill_multicast = false;
            task.audio_index = audio_index;
            audio = PiEar::Audio(&kill_audio, &channels, audio_index);
            if (audio_index != audio.audio_index) {
                audio_index = audio.audio_index;
            }
            if (audio.channel_count(audio.audio_index) < 1) {
                std::cout << "No audio channels found" << std::endl;
                return 1;
            }
            while ( audio.channel_count(audio.audio_index) != channels.size() ) {
                if ( audio.channel_count(audio.audio_index) < channels.size() ) {
                    delete channels.back();
                    channels.pop_back();
                } else if ( audio.channel_count(audio.audio_index) > channels.size() ) {
                    channels.push_back(new PiEar::channel(channels.size()));
                }
            }
            task.load_from_file();
            task.async_run_save_task();
            audio_thread = std::thread(&PiEar::Audio::audio_thread, &audio);
            webserver_thread = std::thread(PiEar::mainloop_http_server, &kill_http, &channels, &cpm, random_string(20), 1, info, &audio_index, &change_audio_source);
            multicast_thread = std::thread(PiEar::mainloop_multicast_server, &channels, &click, &kill_multicast);
        } else {
            std::this_thread::yield();
        }
    }
    kill_click = kill_audio = kill_http = kill_multicast = true;
    task.async_stop_save_task();
    audio_thread.join();
    click_thread.join();
    webserver_thread.join();
    multicast_thread.join();
    Pa_Terminate();
    for (auto & channel : channels) {
        delete channel;
    }
}
