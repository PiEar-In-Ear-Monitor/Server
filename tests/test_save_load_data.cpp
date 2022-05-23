#include <cstdio>
#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <vector>
#include "channel.hpp"
#include "gen_channels.hpp"
#include "load-data.hpp"
#include "task.hpp"

#define DIRECTORY_SEPARATOR "/"
std::vector<PiEar::channel*> *load_from_file(const std::string& path) {
    std::string data = PiEar::get_file_contents(path);
    return PiEar::process_array(&data, "channels");
}
TEST(testPiEar, task_load) {
    auto cha = generate_channels(3);
    std::string full_path = std::filesystem::temp_directory_path().string() + DIRECTORY_SEPARATOR + "piear_test_save_load.json";
    auto task = PiEar::task(cha, full_path, 1);
    task.async_run_save_task();
    while (!std::filesystem::exists(full_path)) std::this_thread::yield();
    sleep(1);
    std::vector<PiEar::channel *> *data = load_from_file(full_path);
    ASSERT_EQ(cha->size(), data->size());
    for (int _ = 0; _ < 2; _++) {
        ASSERT_EQ(cha->size(), data->size());
        for (unsigned i = 0; i < cha->size(); i++) {
            ASSERT_EQ(cha->at(i)->piear_id, data->at(i)->piear_id);
            ASSERT_EQ(cha->at(i)->channel_name, data->at(i)->channel_name);
            ASSERT_EQ(cha->at(i)->enabled, data->at(i)->enabled);
        }
        if (_ == 1) {
            break;
        }
        generate_channels(2, cha);
        sleep(2);
        for (auto channel: *data) {
            delete channel;
        }
        delete data;
        data = load_from_file(full_path);
    }
    task.async_stop_save_task();
    remove(full_path.c_str());
    for (auto channel: *cha) {
        delete channel;
    }
    delete cha;
    for (auto channel: *data) {
        delete channel;
    }
    delete data;
}
