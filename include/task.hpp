/**
 * @file task.hpp
 * @brief Provides File save of current channel names every minute
 * @author Alex O'Connor
 * @date 8 February 2022
 */

#ifndef PIEAR_SERVER_TASK_HPP
#define PIEAR_SERVER_TASK_HPP

#include "channel.h"
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#ifndef SAVE_TIME_INTERVAL_SECONDS
    #define SAVE_TIME_INTERVAL_SECONDS 60  //!< Defines, in seconds, the save interval
#endif

namespace PiEar {
    boost::asio::io_service io;  //!< io_service for task
    boost::posix_time::seconds interval(SAVE_TIME_INTERVAL_SECONDS); //!< Time for `t`
    boost::asio::deadline_timer t(io, interval); //!< Timer that makes it all possible

    // Globals
    std::vector<PiEar::channel *> *channels; //!< Channels to save
    std::string file_path;                   //!< File path to save to
    bool *kill_task;                         //!< When set true, the task will not reschedule itself

    // Functions
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
 *              },
     *          ...
     *      ]
     * }
     * @return std::string representation of a JSON object
     */
    std::string create_json();

    /**
     * Task that saves `channels`
     */
    void save_task(const boost::system::error_code &);

    /**
     * function that initializes `save_task` for its
     * first run.
     *
     * @param std::vector<PiEar::channel*>* Is a pointer to a std::vector
     * of pointers to PiEar::channel objects. These are all pointers so
     * that this task can have access to the most current data for saving.
     * @param std::string file_path to save file
     * @param bool* when set `true`, the service will stop after next save.
     */
    void task(std::vector<PiEar::channel *> *, std::string, bool *);

    std::string create_json() {
        std::ostringstream s;
        s << "{\"channels\":[";
        for (auto it = std::begin(*channels); it != std::end(*channels); it++) {
            s << (std::string) **it;
            if (std::end(*channels) != it + 1) {
                s << ',';
            }
        }
        s << "]}";
        return s.str();
    }

    void save_task(const boost::system::error_code & /*e*/) {
        std::ofstream fs;
        fs.open(file_path.c_str());
        fs << create_json();
        fs.close();
        if (!*kill_task) {
            t.expires_at(t.expires_at() + interval);
            t.async_wait([](auto &&PH1) { return save_task(std::forward<decltype(PH1)>(PH1)); });
        }
    }

    void task(std::vector<PiEar::channel *> *c, std::string f, bool *kill) {
        channels = c;
        file_path = std::move(f);
        kill_task = kill;
        t.async_wait([](auto &&PH1) { return save_task(std::forward<decltype(PH1)>(PH1)); });
        io.run();
    }
}

#endif //PIEAR_SERVER_TASK_HPP
