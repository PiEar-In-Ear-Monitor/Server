#ifndef PIEAR_SERVER_TASK_HPP
#define PIEAR_SERVER_TASK_HPP

#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include "channel.hpp"

namespace PiEar {
    class task {
    public:
        std::vector<PiEar::channel *> *channels; //!< Channels to save
        std::string file_path;                   //!< File path to save to
        bool kill_task;                          //!< When set true, the task will not reschedule itself
        std::thread save_task_thread;            //!< Thread to run the task on
        bool is_running;                         //!< Is the task running?
        int save_interval;                       //!< How often to save the channels
        int audio_index;                         //!< Index of the audio channel to use
        /**
         * function that initializes `save_task` for its
         * first run, also loads audio_index.
         *
         * @param std::vector<PiEar::channel*>* Is a pointer to a std::vector
         * of pointers to PiEar::channel objects.
         * @param std::string file_path to save file
         * @param int Is the time between writing to the save file
         */
        task(std::vector<PiEar::channel *> *c, std::string f, int save_pause);
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
         *              "channel": <string>,
         *              "enabled": <bool>
         *          },
         *          ...
         *      ]
         * }
         * @return std::string representation of a JSON object
         */
        [[nodiscard]] std::string create_json() const;
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
            while (std::chrono::duration<double>(std::chrono::system_clock::now() - start_time).count() < (task->save_interval));
            std::ofstream fs;
            fs.open(task->file_path.c_str());
            fs << task->create_json();
            fs.close();
            start_time = std::chrono::system_clock::now();
        }
    }
    std::string task::create_json() const {
        std::ostringstream s;
        s << "{\"channels\":[";
        for (auto it = std::begin(*this->channels); it != std::end(*this->channels); it++) {
            s << (std::string) **it;
            if (std::end(*this->channels) != it + 1) {
                s << ',';
            }
        }
        s << "],\"audio_index\":" << this->audio_index << "}";
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
                if (json_obj["channels"].size() < i) {
                    this->channels->at(i)->channel_name = json_obj["channels"][i]["channel"];
                    this->channels->at(i)->enabled = json_obj["channels"][i]["enabled"];
                } else {
                    this->channels->at(i)->channel_name = "Channel " + std::to_string(i);
                    this->channels->at(i)->enabled = true;
                }
            }
        }
    }
    task::task(std::vector<PiEar::channel*> *c, std::string f, int save_pause)
            : channels(c), file_path(std::move(f)), kill_task(false), is_running(false), save_interval(save_pause) {
        std::ifstream fs;
        fs.open(this->file_path.c_str());
        if (fs.is_open()) {
            std::string json_file_contents;
            std::getline(fs, json_file_contents);
            fs.close();
            auto json_obj = nlohmann::json::parse(json_file_contents);
            this->audio_index = (json_obj.contains("audio_index") ? (int) json_obj["audio_index"] : -1);
        } else {
            fs.close();
            this->audio_index = -1;
            for (int i = 0; i < this->channels->size(); i++) {
                this->channels->at(i)->channel_name = "Channel " + std::to_string(i);
                this->channels->at(i)->enabled = true;
            }
        }
    }
    void task::async_run_save_task() {
        if (!this->is_running) {
            this->is_running = true;
            save_task_thread = std::thread(save_task, this);
        }
    }
    void task::async_stop_save_task() {
        this->kill_task = true;
        save_task_thread.join();
        this->is_running = false;
    }
}

#endif //PIEAR_SERVER_TASK_HPP
