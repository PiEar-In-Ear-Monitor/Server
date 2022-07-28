#include <atomic>
#include <csignal>
#include <iostream>
#include <portaudio.h>
#include <random>
#include <thread>
#include "audio.h"
#include "channel.hpp"
#include "click.h"
#include "http-server.h"
#include "logger.h"
#include "multicast-server.h"
#include "task.hpp"

// For getting the home directory.
#include <pwd.h>
#include <unistd.h>

#define PIEAR_SETTINGS_FILE ".config/piear/piear_settings.json"

namespace PiEar {
    auto random_string(std::size_t length) -> std::string {
        const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device random_device;
        std::mt19937 generator(random_device());
        std::uniform_int_distribution<> distribution(0, 61); // 61 is length of CHARACTERS
        std::string random_string;
        for (std::size_t i = 0; i < length; ++i) {
            random_string += CHARACTERS[distribution(generator)];
        }
        PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Generated random string: " << random_string;
        return random_string;
    }

    bool SIG_HANDLER_KILL_SERVER = false;
}

auto main(int argc, char *argv[]) -> int {
    if (argc > 1 && strcmp(argv[1], "-v") == 0) {
        PiEar::Logger::init(boost::log::trivial::trace);
    } else if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        PiEar::Logger::init(boost::log::trivial::debug);
    } else {
        PiEar::Logger::init();
    }
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Starting PiEar...";
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
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Loading settings from: " << filename;
    PiEar::task task = PiEar::task(&channels, filename, 5);
    audio_index = task.audio_index;
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Initializing PortAudio";
    int err = Pa_Initialize();
    if( err != paNoError ){
        throw std::runtime_error("PortAudio error initializing");
    }
    PiEar::Audio audio = PiEar::Audio(&kill_audio, &channels, task.audio_index);
    if (task.audio_index != audio.get_audio_index()) {
        task.audio_index = audio.get_audio_index();
    }
    std::vector<PiEar::audioDevice> info = audio.get_audio_devices();
    if (audio.channel_count(audio.get_audio_index()) < 1) {
        std::cout << "No audio channels found" << std::endl;
        return 1;
    }
    for (int i = 0; i < audio.channel_count(audio.get_audio_index()); i++) {
        channels.push_back(new PiEar::channel(i));
    }
    task.load_from_file();
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Starting config file save task";
    task.async_run_save_task();
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Starting audio task";
    std::thread audio_thread(&PiEar::Audio::audio_thread, &audio); // Determine if test can run on gh runner
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Starting click task";
    std::thread click_thread(PiEar::mainloop_click, &cpm, &click, &kill_click);
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Starting HTTP server task";
    std::thread webserver_thread(PiEar::mainloop_http_server, &kill_http, &channels, &cpm, PiEar::random_string(20), 1, info, &audio_index, &change_audio_source);
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Starting multicast server task";
    std::thread multicast_thread(PiEar::mainloop_multicast_server, &channels, &click, &kill_multicast);
    signal(SIGINT, [](int signum) { PiEar::SIG_HANDLER_KILL_SERVER = true; });
    signal(SIGTERM, [](int signum) { PiEar::SIG_HANDLER_KILL_SERVER = true; });
    signal(SIGABRT, [](int signum) { PiEar::SIG_HANDLER_KILL_SERVER = true; });
    while (!PiEar::SIG_HANDLER_KILL_SERVER) {
        if (change_audio_source) {
            kill_audio = kill_http = kill_multicast = true;
            task.async_stop_save_task();
            audio_thread.join();
            webserver_thread.join();
            multicast_thread.join();
            change_audio_source = kill_audio = kill_http = kill_multicast = false;
            task.audio_index = audio_index;
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Changing audio source to " << task.audio_index;
            audio.set_audio_device(task.audio_index);
            if (task.audio_index != audio.get_audio_index()) {
                task.audio_index = audio.get_audio_index();
            }
            if (audio.channel_count(audio.get_audio_index()) < 1) {
                PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "No audio channels found on new source: " << task.audio_index;
                kill_click = true;
                click_thread.join();
                Pa_Terminate();
                for (auto & channel : channels) {
                    delete channel;
                }
                return 1;
            }
            while ( audio.channel_count(audio.get_audio_index()) != channels.size() ) {
                if ( audio.channel_count(audio.get_audio_index()) < channels.size() ) {
                    delete channels.back();
                    channels.pop_back();
                } else if ( audio.channel_count(audio.get_audio_index()) > channels.size() ) {
                    channels.push_back(new PiEar::channel(channels.size()));
                }
            }
            task.load_from_file();
            PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::trace) << "Starting config file save task";
            task.async_run_save_task();
            PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::trace) << "Starting audio task";
            audio_thread = std::thread(&PiEar::Audio::audio_thread, &audio);
            PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::trace) << "Starting click task";
            webserver_thread = std::thread(PiEar::mainloop_http_server, &kill_http, &channels, &cpm, PiEar::random_string(20), 1, info, &audio_index, &change_audio_source);
            PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::trace) << "Starting multicast server task";
            multicast_thread = std::thread(PiEar::mainloop_multicast_server, &channels, &click, &kill_multicast);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Killing threads";
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
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Terminating PiEar";
}
