//
// Created by alex on 2/7/22.
//

#include "task.h"
#include "json_data.h"
#include <json-c/json.h>
#include <boost/asio/steady_timer.hpp>
#include <fstream>


void task::save_task(const boost::system::error_code & /*e*/) {
    std::ofstream file_stream;
    file_stream.open(data->filename);
    file_stream << json_object_to_json_string_ext(data->data, JSON_C_TO_STRING_PRETTY|JSON_C_TO_STRING_SPACED);
    file_stream.close();
    t.expires_at(t.expires_at() + interval);
    t.async_wait(save_task);
}

task::task(json_data *json) {
    data = json;
    interval = boost::asio::chrono::seconds(SAVE_TIME_INTERVAL_SECONDS);
    t = boost::asio::steady_timer(io, interval);
}