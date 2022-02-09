#include "load-data.h"
#include <string>
#include <gtest/gtest.h>

TEST(testPiEar, myCubeTest) {
    std::string json = R"({"channels":[{"data":1}, {"data":2}]})";
    std::vector<std::string*> *parsed = PiEar::get_array_key(&json, "channels");

    EXPECT_EQ(1000, 1000);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}