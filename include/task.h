//
// Created by alex on 2/7/22.
//

#ifndef PIEAR_SERVER_TASK_H
#define PIEAR_SERVER_TASK_H

#include "json_data.h"
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>

#define SAVE_TIME_INTERVAL_SECONDS 60

class task {
private:
    boost::asio::io_context static io;
    boost::asio::chrono::seconds static interval;
    boost::asio::steady_timer static t;
    json_data static *data;
    void static save_task(const boost::system::error_code&);
public:
    task(json_data*);
};

#endif //PIEAR_SERVER_TASK_H
