#include "gtest/gtest.h"
#include "channel.h"
#include <string>

TEST(testPiEar, channel_basic_usage) {
    PiEar::channel to_test(1, 1, std::string("Hello World"), true);
    EXPECT_TRUE(to_test);
    EXPECT_EQ(to_test, 1);
}

TEST(testPiEar, channel_bool_test) {
    PiEar::channel to_test(1, 1, std::string("Hello World"), true);
    EXPECT_TRUE(to_test);
    bool test_param = (bool)to_test;
    for (int i = 0; i < 100; i++) {
        test_param = !test_param;
        to_test.enabled = test_param;
        if (test_param) {
            EXPECT_TRUE(to_test);
        } else {
            EXPECT_FALSE(to_test);
        }
    }
}

TEST(testPiEar, channel_int_test) {
    PiEar::channel to_test(1, 1, std::string("Hello World"), true);
    EXPECT_EQ(to_test, 1);
    for (int i = 2; i < 102; i++) {
        to_test.piear_id = i;
        EXPECT_EQ(to_test, i);
        EXPECT_EQ((int)to_test, i);
    }
}

TEST(testPiEar, channel_string_test) {
    PiEar::channel to_test(1, 1, std::string("Hello World"), true);
    EXPECT_EQ(to_test, 1);
    EXPECT_EQ((std::string)to_test, std::string("{\"pipewire_id\":1,\"piear_id\":1,\"channel_name\":\"Hello World\",\"enabled\":true}"));
    to_test.pipewire_id = 2;
    to_test.piear_id = 4;
    to_test.enabled = false;
    to_test.channel_name = std::string("New Name");
    EXPECT_EQ((std::string)to_test, std::string("{\"pipewire_id\":2,\"piear_id\":4,\"channel_name\":\"New Name\",\"enabled\":false}"));
}
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}