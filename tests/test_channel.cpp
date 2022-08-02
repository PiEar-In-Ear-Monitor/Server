#include <gtest/gtest.h>
#include <string>
#include "channel.h"
#include "gen_channels.hpp"

// TODO: Test audio push/pop

TEST(testPiEar, channel_basic_usage) {
    PiEar::channel to_test(1, std::string("Hello World"));
    EXPECT_TRUE(to_test);
    EXPECT_EQ(to_test, 1);
    EXPECT_EQ(to_test.piear_id, 1);
}
TEST(testPiEar, channel_bool_test) {
    PiEar::channel to_test(1, std::string("Hello World"));
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
    PiEar::channel to_test(1, std::string("Hello World"));
    EXPECT_EQ(to_test, 1);
    for (int i = 2; i < 102; i++) {
        to_test.piear_id = i;
        EXPECT_EQ(to_test, i);
        EXPECT_EQ((int)to_test, i);
        EXPECT_NE((int)to_test, i + 2);
        EXPECT_TRUE(to_test == i);
        EXPECT_FALSE(to_test == i + 2);
    }
}
TEST(testPiEar, channel_string_test) {
    PiEar::channel to_test(1, std::string("Hello World"));
    EXPECT_EQ(to_test, 1);
    EXPECT_EQ((std::string)to_test, std::string("{\"piear_id\":1,\"channel_name\":\"SGVsbG8gV29ybGQ=\",\"enabled\":true}"));
    to_test.piear_id = 4;
    to_test.enabled = false;
    to_test.channel_name = PiEar::channel::base64_encode("New Name");
    EXPECT_EQ(to_test.channel_name, std::string("TmV3IE5hbWU="));
    EXPECT_EQ((std::string)to_test, std::string("{\"piear_id\":4,\"channel_name\":\"TmV3IE5hbWU=\",\"enabled\":false}"));
}
TEST(testPiEar, channel_base64_encode_test) {
    EXPECT_EQ(PiEar::channel::base64_encode("Hello World"), std::string("SGVsbG8gV29ybGQ="));
    EXPECT_EQ(PiEar::channel::base64_encode(""), std::string(""));
    EXPECT_EQ(PiEar::channel::base64_encode("\0"), std::string(""));
    EXPECT_EQ(PiEar::channel::base64_encode("😀"), std::string("8J+YgA=="));
}
TEST(testPiEar, channel_gen_channel_test) {
    std::vector<PiEar::channel*> *to_test = generate_channels(4);
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ((*to_test)[i]->piear_id, i);
        EXPECT_EQ((*to_test)[i]->channel_name, PiEar::channel::base64_encode(std::string("Channel " + std::to_string(i))));
        EXPECT_TRUE((*to_test)[i]->enabled);
    }
    for (int i = 0; i < 4; i++) {
        delete (*to_test)[i];
    }
    delete to_test;
}