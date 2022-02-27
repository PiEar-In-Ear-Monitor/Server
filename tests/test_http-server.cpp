#include "gtest/gtest.h"
#include "http-server.h"
#include <atomic>

TEST(testPiEar, http_server)
{
    std::atomic<int> cpm = 3;
    PiEar::mainloop_http_server(&cpm, 5, "Hello");
    EXPECT_EQ(1000, 1000);
}
