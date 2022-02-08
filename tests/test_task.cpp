#include "gtest/gtest.h"
#include <vector>
#include <cstdio>
#define SAVE_TIME_INTERVAL_SECONDS 1
#include "channel.h"
#include "task.hpp"
#include <thread>
#include <filesystem>
#include <boost/filesystem.hpp>

std::string read_file(std::string full_path) {
    std::ifstream ifs(full_path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    std::vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);
    return std::string(bytes.data(), fileSize);
}

TEST(testPiEar, task_basic_usage)
{
    auto *cha = new std::vector<PiEar::channel*>;
    cha->push_back(new PiEar::channel(1, 1, "Name1", true));
    cha->push_back(new PiEar::channel(2, 5, "Name2", false));
    cha->push_back(new PiEar::channel(3, 3, "Name3", true));
    bool *kill_test_task = new bool(false);
    boost::filesystem::path system_tmp_dir (std::filesystem::temp_directory_path());
    boost::filesystem::path test_file ("piear_test.json");
    boost::filesystem::path full_path = system_tmp_dir / test_file;
    std::thread test_thread (task, cha, full_path.string(), kill_test_task);
    sleep(2);
    std::string original = read_file(full_path.string());
    cha->push_back(new PiEar::channel(3, 3, "Name3", true));
    sleep(2);
    std::string modified = read_file(full_path.string());
    cha->push_back(new PiEar::channel(4, 2, "Name4", true));
    *kill_test_task = true;
    test_thread.join();
    EXPECT_NE(original, modified);
//    remove(full_path);
}
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}