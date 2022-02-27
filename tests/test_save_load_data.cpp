#include "gtest/gtest.h"
#include "load-data.hpp"
#include "task.hpp"
#include "channel.hpp"
#include <vector>
#include <thread>
#include <random>
#include <filesystem>
#include <boost/filesystem.hpp>

namespace testing {
    int channel_count = 0;
    void add_channel(int size, std::vector<PiEar::channel*> *cha) {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(0, 1);
        for (int i = 0; i < size; ++i) {
            cha->push_back(new PiEar::channel(channel_count, channel_count + i, std::string("Channel " + std::to_string(i)) , ((int(dist(mt) * 2) % 2) == 0)));
            channel_count++;
        }
    }

    std::vector<PiEar::channel*> *load_from_file(const boost::filesystem::path& path) {
        std::string data = PiEar::get_file_contents(path.string());
        std::string key = "channels";
        return PiEar::process_array(&data,  key);
    }

    TEST(testPiEar, task_load) {
        auto cha = new std::vector<PiEar::channel*>;
        add_channel(3, cha);
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
        add_channel(2, cha);
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
}
