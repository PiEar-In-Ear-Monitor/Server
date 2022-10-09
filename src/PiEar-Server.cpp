#include <atomic>
#include <csignal>
#include <iostream>
#include <portaudio.h>
#include <random>
#include <thread>
#include "audio.h"
#include "channel.h"
#include "click.h"
#include "http-server.h"
#include "logger.h"
#include "udp-server.h"
#include "task.hpp"

// For getting the home directory.
// TODO: Make this cross-platform.
#include <pwd.h>
#include <unistd.h>

#define PIEAR_SETTINGS_FILE "/.config/piear/piear_settings.json"

namespace PiEar {
    enum {
        PIEAR_SAVE_INTERVAL_DEFAULT = 10
    };
    struct main_loop {
    public:
        ~main_loop() {
            delete this->task;
            delete this->audio;
            for (auto &channel: *this->channels) {
                delete channel;
            }
            delete this->channels;
        }
        auto init_settings() -> void {
            this->init_filename();
            this->task = new PiEar::task(this->channels, this->filename, PIEAR_SAVE_INTERVAL_DEFAULT, &this->audio_index);
        }
        auto init_audio_and_channels() -> void {
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Initializing audio and channels...";
            this->audio = new PiEar::Audio(&this->audio_thread_kill, this->channels, this->audio_index);
            if (this->audio_index != this->audio->get_audio_index()) {
                PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::warning) << "Audio index changed from " << this->audio_index << " to " << this->audio->get_audio_index();
                this->audio_index = this->audio->get_audio_index();
            }
            if (audio->channel_count(audio->get_audio_index()) < 1) {
                PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "No audio devices found on device " << this->audio_index;
                throw std::runtime_error("No audio devices found on device " + std::to_string(this->audio_index));
            }
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Loading channel names...";
            this->sync_channels_with_task();
        }
        auto start_everything() -> void {
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Starting save task...";
            this->task->async_run_save_task();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Starting click thread...";
            this->start_click_thread();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Starting http server...";
            this->start_http_thread();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Starting udp server...";
            this->start_udp_thread();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Starting audio server...";
            this->start_audio_thread();
        }
        auto triggered_change_audio_index() -> bool {
            return this->change_audio_source;
        }
        auto change_audio_index() -> void {
            this->change_audio_source = false;
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Stopping save task";
            this->task->async_stop_save_task();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Joining webserver thread";
            stop_http_thread();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Joining audio thread";
            stop_audio_thread();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Joining udp thread";
            stop_udp_thread();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Changing audio source to " << audio_index;
            audio->set_audio_device(audio_index);
            if (audio_index != audio->get_audio_index()) {
                PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::warning) << "Failed to change audio source to that selected, using default";
                audio_index = audio->get_audio_index();
            }
            if (audio->channel_count(audio->get_audio_index()) < 1) {
                PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "No audio channels found on new source: " << audio_index;
                stop_click_thread();
                throw std::runtime_error("No audio channels found");
            }
            this->sync_channels_with_task();
            task->load_from_file();
            task->async_run_save_task();
            start_http_thread();
            start_udp_thread();
            start_audio_thread();
        }
        auto end_everything() -> void {
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Stopping save task";
            this->task->async_stop_save_task();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Joining webserver thread";
            stop_http_thread();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Joining audio thread";
            stop_audio_thread();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Joining udp thread";
            stop_udp_thread();
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Joining click thread";
            stop_click_thread();
        }
    private:
        std::thread audio_thread;
        std::atomic<bool> audio_thread_kill;
        std::atomic<int> audio_index;
        PiEar::Audio *audio = nullptr;
        void start_audio_thread() {
            audio_thread_kill = false;
            audio_thread = std::thread(&PiEar::Audio::audio_thread, audio);
        }
        void stop_audio_thread() {
            audio_thread_kill = true;
            audio_thread.join();
        }
        std::thread http_thread;
        std::atomic<bool> http_thread_kill;
        std::atomic<bool> change_audio_source;
        std::vector<PiEar::audioDevice> info_audio_devices = Audio::get_audio_devices();
        void start_http_thread() {
            http_thread_kill = false;
            http_thread = std::thread(PiEar::mainloop_http_server, &http_thread_kill, channels, &cpm, main_loop::random_string(20), 1, info_audio_devices, &audio_index, &change_audio_source);
        }
        void stop_http_thread() {
            http_thread_kill = true;
            http_thread.join();
        }
        static auto random_string(std::size_t length) -> std::string {
            const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
            std::random_device random_device;
            std::mt19937 generator(random_device());
            std::uniform_int_distribution<> distribution(0, CHARACTERS.length() - 1);
            std::string random_string;
            for (std::size_t i = 0; i < length; ++i) {
                random_string += CHARACTERS[distribution(generator)];
            }
            PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::debug) << "Generated random string: " << random_string;
            return random_string;
        }
        std::thread udp_thread;
        std::atomic<bool> udp_thread_kill;
        void start_udp_thread() {
            udp_thread_kill = false;
            udp_thread = std::thread(PiEar::mainloop_udp_server, channels, &click, &udp_thread_kill);
        }
        void stop_udp_thread() {
            udp_thread_kill = true;
            udp_thread.join();
        }
        std::thread click_thread;
        std::atomic<bool> click_thread_kill;
        std::atomic<bool> click;
        std::atomic<int> cpm = 100;
        void start_click_thread() {
            click_thread_kill = false;
            click_thread = std::thread(PiEar::mainloop_click, &cpm, &click, &click_thread_kill);
        }
        void stop_click_thread() {
            click_thread_kill = true;
            click_thread.join();
        }
        std::string filename;
        auto init_filename() -> void {
            if (getenv("HOME") == nullptr) { // TODO: Make this cross-platform.
                filename = getpwuid(getuid())->pw_dir;
            } else {
                filename = getenv("HOME");
            }
            filename += PIEAR_SETTINGS_FILE;
        }
        auto sync_channels_with_task() -> void {
            while (audio->channel_count(audio->get_audio_index()) != channels->size()) {
                if (audio->channel_count(audio->get_audio_index()) < channels->size()) {
                    delete channels->back();
                    channels->pop_back();
                } else if (audio->channel_count(audio->get_audio_index()) > channels->size()) {
                    channels->push_back(new PiEar::channel(channels->size()));
                }
            }
            task->load_from_file();
        }
        std::vector<PiEar::channel*> *channels = new std::vector<PiEar::channel*>();
        PiEar::task *task;
    };
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
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Adding SIG listeners...";
    signal(SIGINT, [](int signum) { PiEar::SIG_HANDLER_KILL_SERVER = true; });
    signal(SIGTERM, [](int signum) { PiEar::SIG_HANDLER_KILL_SERVER = true; });
    signal(SIGABRT, [](int signum) { PiEar::SIG_HANDLER_KILL_SERVER = true; });
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::trace) << "Initializing PortAudio...";
    int err = Pa_Initialize();
    if( err != paNoError ){
        PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "PortAudio error: " << Pa_GetErrorText(err);
        return 1;
    }
    PiEar::main_loop main_loop;
    PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Initializing settings...";
    main_loop.init_settings();
    try {
        main_loop.init_audio_and_channels();
    } catch (std::runtime_error &e) {
        PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "Error: " << e.what();
        return 1;
    }
    try {
        PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Starting threads...";
        main_loop.start_everything();
        while (!PiEar::SIG_HANDLER_KILL_SERVER) {
            if (main_loop.triggered_change_audio_index()) {
                PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Changing audio source...";
                main_loop.change_audio_index();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
        main_loop.end_everything();
        Pa_Terminate();
        PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Terminating PiEar";
        return 0;
    } catch ( std::exception &e) {
        Pa_Terminate();
        PIEAR_LOG_WITH_FILE_LOCATION(boost::log::trivial::error) << "Error in main loop: " << e.what();
        return 1;
    }
}
