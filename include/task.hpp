#ifndef PIEAR_SERVER_TASK_HPP
#define PIEAR_SERVER_TASK_HPP

#include "channel.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#ifndef SAVE_TIME_INTERVAL_SECONDS
    #define SAVE_TIME_INTERVAL_SECONDS 60
#endif

using namespace boost::placeholders;

boost::asio::io_service io;
boost::posix_time::seconds interval(SAVE_TIME_INTERVAL_SECONDS);
boost::asio::deadline_timer t(io, interval);

// Globals
std::vector<PiEar::channel*>  *channels;
std::string filename;
bool *kill_task;

// Functions
std::string create_json();
void save_task(const boost::system::error_code &);
void task(std::vector<PiEar::channel*> *, std::string, bool*);

std::string create_json() {
    std::ostringstream s;
    s << "{\"channels\":[";
    for(auto it = std::begin(*channels); it != std::end(*channels); it++) {
        s << (std::string)**it;
        if (std::end(*channels) != it + 1) {
            s << ',';
        }
    }
    s << "]}";
    return s.str();
}

void save_task(const boost::system::error_code & /*e*/) {
    std::ofstream fs;
    fs.open(filename.c_str());
    fs << create_json();
    fs.close();
    if (!*kill_task) {
        t.expires_at(t.expires_at() + interval);
        t.async_wait([](auto && PH1) { return save_task(std::forward<decltype(PH1)>(PH1)); });
    }
}

void task(std::vector<PiEar::channel*> *c, std::string f, bool *kill) {
    channels = c;
    filename = f;
    kill_task = kill;
    t.async_wait([](auto && PH1) { return save_task(std::forward<decltype(PH1)>(PH1)); });
    io.run();
}

#endif //PIEAR_SERVER_TASK_HPP
