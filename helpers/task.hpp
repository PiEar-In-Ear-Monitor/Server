#ifndef PIEAR_SERVER_TASK_HPP
#define PIEAR_SERVER_TASK_HPP

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "channel.h"

namespace PiEar {
    class task {
    public:
        std::vector<PiEar::channel *> *channels; //!< Channels to save
        std::string file_path;                   //!< File path to save to
        std::atomic<bool> kill_task = false;     //!< When set true, the task will not reschedule itself
        std::thread save_task_thread;            //!< Thread to run the task on
        bool is_running = false;                 //!< Is the task running?
        int save_interval;                       //!< How often to save the channels
        std::atomic<int> *audio_index;           //!< Index of the audio channel to use
        /**
         * function that initializes `save_task` for its
         * first run, also loads audio_index.
         *
         * @param std::vector<PiEar::channel*>* Is a pointer to a std::vector
         * of pointers to PiEar::channel objects.
         * @param std::string file_path to save file
         * @param int Is the time between writing to the save file
         * @param std::atomic<int>* Is a pointer to an atomic integer
         */
        task(std::vector<PiEar::channel *> *c, std::string f, int save_pause, std::atomic<int> *ai);
        /**
         * function begins the task
         */
        void async_run_save_task();
        /**
         * function stops the task
         */
        void async_stop_save_task();
        /**
         * Creates a JSON representation of `channels`
         * {
         *      "channels" :
         *      [
         *          {
         *              "piear_id": <int>,
         *              "channel_name": <string>,
         *              "enabled": <bool>
         *          },
         *          ...
         *      ]
         * }
         * @return std::string representation of a JSON object
         */
        auto create_json() -> std::string;
        /**
         * Task that saves `channels`
         */
        static void save_task(task *task);
        /**
         * @brief Loads `channels` from `file_path`
         */
        void load_from_file();
    };
    void task::save_task(task *task) {
        auto start_time = std::chrono::system_clock::now();
        while (!task->kill_task) {
            while ((std::chrono::duration<double>(std::chrono::system_clock::now() - start_time).count() < (task->save_interval)) && (!task->kill_task));
            std::ofstream fs;
            fs.open(task->file_path.c_str());
            fs << task->create_json();
            fs.close();
            start_time = std::chrono::system_clock::now();
        }
    }
//    This function cannot be const
#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-make-member-function-const"
    auto task::create_json() -> std::string {
        std::ostringstream s;
        s << "{\"channels\":[";
        for (auto it = std::begin(*this->channels); it != std::end(*this->channels); it++) {
            s << (std::string) **it;
            if (std::end(*this->channels) != it + 1) {
                s << ',';
            }
        }
        s << "],\"audio_index\":" << *(this->audio_index) << "}";
        return s.str();
    }
    void task::load_from_file() {
        std::ifstream fs;
        fs.open(this->file_path.c_str());
        if (fs.is_open()) {
            std::string json_file_contents;
            std::getline(fs, json_file_contents);
            fs.close();
            auto json_obj = nlohmann::json::parse(json_file_contents);
            for (int i = 0; i < this->channels->size(); i++) {
                if (i < json_obj["channels"].size()) {
                    this->channels->at(i)->channel_name = json_obj["channels"][i]["channel_name"];
                    this->channels->at(i)->enabled = json_obj["channels"][i]["enabled"];
                } else {
                    this->channels->at(i)->channel_name = PiEar::channel::base64_encode("Channel " + std::to_string(this->channels->at(i)->piear_id));
                    this->channels->at(i)->enabled = true;
                }
            }
        } else {
            for (auto & i : *this->channels) {
                i->channel_name = PiEar::channel::base64_encode("Channel " + std::to_string(i->piear_id));
                i->enabled = true;
            }
        }
    }
#pragma clang diagnostic pop
    task::task(std::vector<PiEar::channel*> *c, std::string f, int save_pause, std::atomic<int> *ai)
            : channels(c), file_path(std::move(f)), save_interval(save_pause), audio_index(ai) {
        PIEAR_LOG_WITHOUT_FILE_LOCATION(boost::log::trivial::info) << "Loading settings from: " << this->file_path;
        std::ifstream fs;
        if (std::filesystem::exists(this->file_path)) {
            fs.open(this->file_path.c_str());
            if (fs.is_open()) {
                std::string json_file_contents;
                std::getline(fs, json_file_contents);
                fs.close();
                auto json_obj = nlohmann::json::parse(json_file_contents);
                *(this->audio_index) = json_obj["audio_index"];
            }
        } else {
            *(this->audio_index) = -1;
            std::filesystem::create_directory(std::filesystem::path(this->file_path).parent_path());
        }
    }
    void task::async_run_save_task() {
        if (!this->is_running) {
            this->kill_task = false;
            this->is_running = true;
            save_task_thread = std::thread(save_task, this);
        }
    }
    void task::async_stop_save_task() {
        if (this->is_running) {
            this->kill_task = true;
            save_task_thread.join();
            this->is_running = false;
        }
    }
}

#endif //PIEAR_SERVER_TASK_HPP
