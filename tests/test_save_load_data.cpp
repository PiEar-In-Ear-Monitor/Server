#include "gtest/gtest.h"
#include "load-data.hpp"
#include "channel.hpp"
#include "task.hpp"
#include "gen_channels.h"
#include <vector>
#include <thread>
#include <filesystem>
#include <iostream>
#include <boost/filesystem.hpp>

std::vector<PiEar::channel*> *load_from_file(const boost::filesystem::path& path) {
    std::string data = PiEar::get_file_contents(path.string());
    std::string key = "channels";
    return PiEar::process_array(&data, key);
}

TEST(testPiEar, task_load) {
    auto cha = generate_channels(3);
    boost::filesystem::path full_path = boost::filesystem::path(std::filesystem::temp_directory_path()) / "piear_test_save_load.json";
    auto task = PiEar::task(cha, full_path.string(), 1);
    task.async_run_save_task();
    sleep(2);
    auto data = load_from_file(full_path);
    ASSERT_EQ(cha->size(), data->size());
    auto initial_count = cha->size();
    for (unsigned i = 0; i < cha->size(); i++) {
        ASSERT_EQ(cha->at(i)->pipewire_id, data->at(i)->pipewire_id);
        ASSERT_EQ(cha->at(i)->piear_id, data->at(i)->piear_id);
        ASSERT_EQ(cha->at(i)->channel_name, data->at(i)->channel_name);
        ASSERT_EQ(cha->at(i)->enabled, data->at(i)->enabled);
    }
    generate_channels(2, cha);
    sleep(2);
    data = load_from_file(full_path);
    ASSERT_EQ(cha->size(), data->size());
    ASSERT_NE(initial_count, cha->size());
    for (unsigned i = 0; i < cha->size(); i++) {
        ASSERT_EQ(cha->at(i)->pipewire_id, data->at(i)->pipewire_id);
        ASSERT_EQ(cha->at(i)->piear_id, data->at(i)->piear_id);
        ASSERT_EQ(cha->at(i)->channel_name, data->at(i)->channel_name);
        ASSERT_EQ(cha->at(i)->enabled, data->at(i)->enabled);
    }
    task.async_stop_save_task();
    remove(full_path.c_str());
}
