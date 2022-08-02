#include <cstdio>
#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>
#include <vector>
#include "channel.h"
#include "gen_channels.hpp"
#include "load-data.hpp"
#include "task.hpp"

#define DIRECTORY_SEPARATOR "/"
auto load_from_file(const std::string& path, int &pass_audio_index) -> std::vector<PiEar::channel*> * {
    std::string json = PiEar::get_file_contents(path);
    nlohmann::json data = nlohmann::json::parse(json);
    pass_audio_index = data["audio_index"];
    return PiEar::process_array(data, "channels");
}
TEST(testPiEar, task_load) {
    auto cha = generate_channels(3);
    std::string full_path = std::filesystem::temp_directory_path().string() + DIRECTORY_SEPARATOR + "piear_test_save_load.json";
    // If fullpath exists, delete it.
    if (std::filesystem::exists(full_path)) {
        std::filesystem::remove(full_path);
    }
    std::atomic<int> audio_index(-1);
    auto task = PiEar::task(cha, full_path, 1, &audio_index);
    task.async_run_save_task();
    while (!std::filesystem::exists(full_path)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    sleep(1);
    int received_audio_index;
    std::vector<PiEar::channel *> *data = load_from_file(full_path, received_audio_index);
    ASSERT_EQ(received_audio_index, -1);
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
        *task.audio_index = 11;
        sleep(2);
        for (auto channel: *data) {
            delete channel;
        }
        delete data;
        data = load_from_file(full_path, received_audio_index);
        ASSERT_EQ(received_audio_index, 11);
    }
    task.async_stop_save_task();
    std::cout << "State: " << (task.is_running ? "True" : "False") << std::endl;
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
