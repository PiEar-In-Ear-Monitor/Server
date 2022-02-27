/**
 * @file task.hpp
 * @brief Provides File save of current channel names every minute
 * @author Alex O'Connor
 * @date 8 February 2022
 */

#ifndef PIEAR_SERVER_TASK_HPP
#define PIEAR_SERVER_TASK_HPP

#include "channel.hpp"
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <thread>
#include <iostream>
#include <chrono>

namespace PiEar {
    class task {
    public:
        std::vector<PiEar::channel *> *channels; //!< Channels to save
        std::string file_path;                   //!< File path to save to
        bool kill_task;                         //!< When set true, the task will not reschedule itself
        std::thread save_task_thread;
        bool is_running;
        int save_interval;

        /**
         * function that initializes `save_task` for its
         * first run.
         *
         * @param std::vector<PiEar::channel*>* Is a pointer to a std::vector
         * of pointers to PiEar::channel objects.
         * @param std::string file_path to save file
         * @param int Is the time between writing to the save file
         */
        task(std::vector<PiEar::channel *> *c, std::string f, int save_pause);

        // Functions
        void async_run_save_task();

        void async_stop_save_task();

        /**
         * Creates a JSON representation of `channels`
         * {
         *      "channels" :
         *      [
         *          {
         *              "piear_id": <int>,
         *              "pipewire_id": <int>,
         *              "channel": <string>,
         *              "enabled": <bool>
         *          },
         *          ...
         *      ]
         * }
         * @return std::string representation of a JSON object
         */
        std::string create_json() {
            std::ostringstream s;
            s << "{\"channels\":[";
            for (auto it = std::begin(*this->channels); it != std::end(*this->channels); it++) {
                s << (std::string) **it;
                if (std::end(*this->channels) != it + 1) {
                    s << ',';
                }
            }
            s << "]}";
            return s.str();
        }

        /**
         * Task that saves `channels`
         */
        static void save_task(task *task) {
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
    };

    task::task(std::vector<PiEar::channel*> *c, std::string f, int save_pause){
        this->channels = c;
        this->file_path = std::move(f);
        this->kill_task = false;
        this->save_interval = save_pause;
        this->is_running = false;
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
